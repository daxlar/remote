#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_delay.h"

#include <nrfx_timer.h>
#include <debouncer.h>
#include <buttons.h>
#include <joystick.h>



#define TIMER_NUMBER  3 // timer 3 on the nrf52840 has 6 compare registers, and this application has 5 buttons
#define BUTTON_COUNT  5 // there are 5 total buttons in the system

// timer operates at 1 MHz, each tick is 1000 ns
// maximum bounce duration is 6.2 ms -> https://www.eejournal.com/article/ultimate-guide-to-switch-debounce-part-4/#:~:text=As%20Jack%20Ganssle%20reported%20in,microseconds%20(or%206.2%20ms).
// use 20 ms bounce duration to be safe
// 20 ms = 20000000 ns
// 20000000 / 1000 = 20000 ticks
#define DEBOUNCE_TICK_COUNT 20000
#define UINT_16T_MAX_VALUE  65535

#define MS_TO_WAIT  20 

volatile bool channel_debouncing_states[BUTTON_COUNT] = {false};
static nrfx_timer_t timerInstance = NRFX_TIMER_INSTANCE(TIMER_NUMBER);

bool get_channel_debouncing_state(int channel_num){
  return channel_debouncing_states[channel_num];
}


void timer_interrupt_handler(nrf_timer_event_t event_type, void* p_context){
  
  // re-enable the button interrupts here
  // channel 0 corresponds to the joystick button
  // channel 1 corresponds to the left button
  // channel 2 corresponds to the right button 
  // channel 3 corresponds to the down button 
  // channel 4 corresponds to the up button
  
  uint32_t channel_num;
  uint32_t current_tick_count;
  
  switch(event_type){
    case NRF_TIMER_EVENT_COMPARE0:
      channel_num = JOYSTICK_BUTTON;
      current_tick_count = nrfx_timer_capture(&timerInstance, JOYSTICK_BUTTON);
      enable_joystick_button();
      break;
    case NRF_TIMER_EVENT_COMPARE1:
      channel_num = LEFT_BUTTON;
      current_tick_count = nrfx_timer_capture(&timerInstance, LEFT_BUTTON);
      enable_left_button();
      break;
    case NRF_TIMER_EVENT_COMPARE2:
      channel_num = RIGHT_BUTTON;
      current_tick_count = nrfx_timer_capture(&timerInstance, RIGHT_BUTTON);
      enable_right_button();
      break;
    case NRF_TIMER_EVENT_COMPARE3:
      channel_num = DOWN_BUTTON;
      current_tick_count = nrfx_timer_capture(&timerInstance, DOWN_BUTTON);
      enable_down_button();
      break;
    case NRF_TIMER_EVENT_COMPARE4:
      channel_num = UP_BUTTON;
      current_tick_count = nrfx_timer_capture(&timerInstance, UP_BUTTON);
      enable_up_button();
      break;
  }
  
  nrfx_timer_compare_int_disable(&timerInstance, channel_num);
  NRF_LOG_INFO(" timer interrupt on channel: %d at tick count: %d", channel_num, current_tick_count);

}


void debouncer_init(void){
  nrfx_timer_config_t timerConfig = NRFX_TIMER_DEFAULT_CONFIG;
  nrfx_timer_init(&timerInstance, &timerConfig, timer_interrupt_handler);
 
  nrfx_timer_compare_int_disable(&timerInstance, NRF_TIMER_CC_CHANNEL0);
  nrfx_timer_compare_int_disable(&timerInstance, NRF_TIMER_CC_CHANNEL1);
  nrfx_timer_compare_int_disable(&timerInstance, NRF_TIMER_CC_CHANNEL2);
  nrfx_timer_compare_int_disable(&timerInstance, NRF_TIMER_CC_CHANNEL3);
  nrfx_timer_compare_int_disable(&timerInstance, NRF_TIMER_CC_CHANNEL4);

  nrfx_timer_enable(&timerInstance);
}

void start_debouncing(button button_choice){
  
  uint32_t channel_num = button_choice;
  uint32_t start_tick_count = nrfx_timer_capture(&timerInstance, channel_num);
  uint32_t debounce_tick_count = nrfx_timer_ms_to_ticks(&timerInstance ,MS_TO_WAIT);
  uint32_t end_tick_count = (start_tick_count + debounce_tick_count) % UINT_16T_MAX_VALUE;

  switch(button_choice){
    case JOYSTICK_BUTTON:
      nrfx_timer_compare(&timerInstance, channel_num, end_tick_count, false);
      NRF_LOG_INFO("start debouncing: JOYSTICK_BUTTON");
      break;
    case LEFT_BUTTON:
      nrfx_timer_compare(&timerInstance, channel_num, end_tick_count, false);
      NRF_LOG_INFO("start debouncing: LEFT_BUTTON");
      break;
    case RIGHT_BUTTON:
      nrfx_timer_compare(&timerInstance, channel_num, end_tick_count, false);
      NRF_LOG_INFO("start debouncing: RIGHT_BUTTON");
      break;
    case DOWN_BUTTON:
      nrfx_timer_compare(&timerInstance, channel_num, end_tick_count, false);
      break;
    case UP_BUTTON:
      nrfx_timer_compare(&timerInstance, channel_num, end_tick_count, false);
      break;
  }
  nrfx_timer_compare_int_enable(&timerInstance, channel_num);
  NRF_LOG_INFO("debounce start tick: %d, end tick: %d at button: %d", start_tick_count, end_tick_count, channel_num);
}