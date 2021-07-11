#include <stdbool.h>
#include <stdint.h>
#include <nrfx_saadc.h>
#include <nrfx_gpiote.h>

#define JOYSTICK_X_PIN      NRF_SAADC_INPUT_AIN0
#define JOYSTICK_X_REF_PIN  NRF_SAADC_INPUT_AIN1
#define JOYSTICK_Y_PIN      NRF_SAADC_INPUT_AIN2
#define JOYSTICK_Y_REF_PIN  NRF_SAADC_INPUT_AIN3
#define JOYSTICK_SWITCH_PIN NRF_GPIO_PIN_MAP(0,6)
#define SAADC_CHANNEL_COUNT   2

static nrf_saadc_value_t samples[SAADC_CHANNEL_COUNT];
static nrfx_saadc_channel_t channels[SAADC_CHANNEL_COUNT] = {NRFX_SAADC_DEFAULT_CHANNEL_DIFFERENTIAL(JOYSTICK_X_PIN, JOYSTICK_X_REF_PIN, 0),
                                                             NRFX_SAADC_DEFAULT_CHANNEL_DIFFERENTIAL(JOYSTICK_Y_PIN, JOYSTICK_Y_REF_PIN, 1)};

volatile int interruptDebugger = 0;

static void pin_event_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action){
  interruptDebugger++;
  interruptDebugger %= 1000;                                              
}

void joystick_read_x_y_values(int16_t* x_value, int16_t* y_value){
      ret_code_t err_code;
      err_code = nrfx_saadc_simple_mode_set((1<<0|1<<1),
                                            NRF_SAADC_RESOLUTION_12BIT,
                                            NRF_SAADC_OVERSAMPLE_4X,
                                            NULL);
      
      APP_ERROR_CHECK(err_code);
      
      err_code = nrfx_saadc_buffer_set(samples, SAADC_CHANNEL_COUNT);
      APP_ERROR_CHECK(err_code);

      while(nrfx_saadc_mode_trigger() != NRFX_SUCCESS);
      
      /* Applying workaround from Errata 212, otherwise current is stuck at 4-500uA during sleep after first sample. */
      volatile uint32_t temp1;
      volatile uint32_t temp2;
      volatile uint32_t temp3;

      temp1 = *(volatile uint32_t *)0x40007640ul;
      temp2 = *(volatile uint32_t *)0x40007644ul;
      temp3 = *(volatile uint32_t *)0x40007648ul;

      *(volatile uint32_t *)0x40007FFCul = 0ul; 
      *(volatile uint32_t *)0x40007FFCul; 
      *(volatile uint32_t *)0x40007FFCul = 1ul;

      *(volatile uint32_t *)0x40007640ul = temp1;
      *(volatile uint32_t *)0x40007644ul = temp2;
      *(volatile uint32_t *)0x40007648ul = temp3;
      
      *x_value = samples[0];
      *y_value = samples[1];

}

void joystick_init(void){
  volatile ret_code_t err_code;

  err_code = nrfx_gpiote_init();
  nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
  err_code = nrfx_gpiote_in_init(JOYSTICK_SWITCH_PIN, &config, pin_event_handler);
  nrfx_gpiote_in_event_enable(JOYSTICK_SWITCH_PIN, true);

  err_code = nrfx_saadc_init(NRFX_SAADC_CONFIG_IRQ_PRIORITY);
  APP_ERROR_CHECK(err_code);
  
  channels[0].channel_config.reference = NRF_SAADC_REFERENCE_VDD4;
  channels[0].channel_config.gain =  NRF_SAADC_GAIN1_4;
  channels[1].channel_config.reference = NRF_SAADC_REFERENCE_VDD4;
  channels[1].channel_config.gain =  NRF_SAADC_GAIN1_4;
   
  err_code = nrfx_saadc_channels_config(channels, SAADC_CHANNEL_COUNT);
  APP_ERROR_CHECK(err_code);

  while(nrfx_saadc_offset_calibrate(NULL) != NRFX_SUCCESS);
}

