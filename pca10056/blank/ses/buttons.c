#include <stdbool.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_delay.h"

#include <nrfx_gpiote.h>
#include <nrfx_timer.h>
#include <debouncer.h>

#define LEFT_BUTTON_PIN_NUM   11
#define RIGHT_BUTTON_PIN_NUM  12
#define DOWN_BUTTON_PIN_NUM   24
#define UP_BUTTON_PIN_NUM     25
#define TIMER_NUMBER          1

#define NON_JOYSTICK_BUTTON_COUNT 4
#define LEFT_BUTTON_STORAGE_INDEX   0
#define RIGHT_BUTTON_STORAGE_INDEX  1
#define DOWN_BUTTON_STORAGE_INDEX   2
#define UP_BUTTON_STORAGE_INDEX     3

volatile bool button_clicked_storage[NON_JOYSTICK_BUTTON_COUNT];

void gpiote_interrupt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action){
  
  if(pin == LEFT_BUTTON_PIN_NUM && action == NRF_GPIOTE_POLARITY_HITOLO){
    button_clicked_storage[LEFT_BUTTON_STORAGE_INDEX] = true;
    nrfx_gpiote_in_event_disable(LEFT_BUTTON_PIN_NUM);
    start_debouncing(LEFT_BUTTON);
    NRF_LOG_INFO("left button clicked");
  }

  if(pin == RIGHT_BUTTON_PIN_NUM && action == NRF_GPIOTE_POLARITY_HITOLO){
    button_clicked_storage[RIGHT_BUTTON_STORAGE_INDEX] = true;
    nrfx_gpiote_in_event_disable(RIGHT_BUTTON_PIN_NUM);
    start_debouncing(RIGHT_BUTTON);
    NRF_LOG_INFO("right button clicked");
  }

  if(pin == DOWN_BUTTON_PIN_NUM && action == NRF_GPIOTE_POLARITY_HITOLO){
    button_clicked_storage[DOWN_BUTTON_STORAGE_INDEX] = true;
    nrfx_gpiote_in_event_disable(DOWN_BUTTON_PIN_NUM);
    start_debouncing(DOWN_BUTTON);
    NRF_LOG_INFO("down button clicked");
  }

  if(pin == UP_BUTTON_PIN_NUM && action == NRF_GPIOTE_POLARITY_HITOLO){
    button_clicked_storage[UP_BUTTON_STORAGE_INDEX] = true;
    nrfx_gpiote_in_event_disable(UP_BUTTON_PIN_NUM);
    start_debouncing(UP_BUTTON);
    NRF_LOG_INFO("up button clicked");
  }
  
   NRF_LOG_INFO("interrupt on: %d pin with event %d", pin, action);
}


void buttons_init(void){

  if(!nrfx_gpiote_is_init()){
    nrfx_gpiote_init();
  }
  
  nrfx_gpiote_in_config_t gpioteConfig = {.hi_accuracy      = true,
                                          .is_watcher       = false,
                                          .pull             = NRF_GPIO_PIN_PULLUP,
                                          .sense            = NRF_GPIOTE_POLARITY_HITOLO,
                                          .skip_gpio_setup  = false};
  

  nrfx_gpiote_in_init(LEFT_BUTTON_PIN_NUM, &gpioteConfig, gpiote_interrupt_handler);
  nrfx_gpiote_in_init(RIGHT_BUTTON_PIN_NUM, &gpioteConfig, gpiote_interrupt_handler);
  nrfx_gpiote_in_init(DOWN_BUTTON_PIN_NUM, &gpioteConfig, gpiote_interrupt_handler);
  nrfx_gpiote_in_init(UP_BUTTON_PIN_NUM, &gpioteConfig, gpiote_interrupt_handler);

  nrfx_gpiote_in_event_enable(LEFT_BUTTON_PIN_NUM, true);
  nrfx_gpiote_in_event_enable(RIGHT_BUTTON_PIN_NUM, true);
  nrfx_gpiote_in_event_enable(DOWN_BUTTON_PIN_NUM, true);
  nrfx_gpiote_in_event_enable(UP_BUTTON_PIN_NUM, true);
}

bool get_button_value(int button_number){
  // should be disabling debouncer to check for button state
  return button_clicked_storage[button_number];
}

void enable_left_button(void){
 nrfx_gpiote_in_event_enable(LEFT_BUTTON_PIN_NUM, true);
}

void enable_right_button(void){
 nrfx_gpiote_in_event_enable(RIGHT_BUTTON_PIN_NUM, true);
}

void enable_down_button(void){
 nrfx_gpiote_in_event_enable(DOWN_BUTTON_PIN_NUM, true);
}

void enable_up_button(void){
 nrfx_gpiote_in_event_enable(UP_BUTTON_PIN_NUM, true);
}


void set_button_functions(void){
}

void get_button_functions(void){
}

