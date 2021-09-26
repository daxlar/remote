/**
 * Copyright (c) 2014 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
/** 
 * Perhipheral: nRF52 SAADC
 * Compatibility: nRF52832/nRF52840/nRF52833, nRF5 SDK 17.0.0
 * Softdevice used: No softdevice
 *

 * This example enables the RTC timer to periodically trigger SAADC sampling. RTC is chosen here instead of 
 * TIMER because it is low power. The example samples on a single input pin, the AIN0, which maps to physical pin P0.02 on the nRF52832 IC.
 * This SAADC example shows the following features:
 * - Low Power -> Enabled with initializing SAADC when sampling and uninitializing when sampling is complete.
 *                Low power can only be obtained when UART_PRINTING_ENABLED is not defined and
 *                SAADC_SAMPLES_IN_BUFFER is 1
 * - Oversampling -> This reduces SAADC noise level, especially for higher SAADC resolutions, see
 *                   https://devzone.nordicsemi.com/question/83938/nrf52832-saadc-sampling/?comment=84340#comment-84340
 *                   Configured with the SAADC_OVERSAMPLE constant.
 * - BURST mode -> Burst mode can be combined with oversampling, which makes the SAADC sample all oversamples as fast
 *                 as it can with one SAMPLE task trigger. Set the SAADC_BURST_MODE constant to enable BURST mode.
 * - Offset Calibration -> SAADC needs to be occasionally calibrated. The desired calibration interval depends on the
 *                         expected temperature change rate, see the nRF52832 PS for more information. The
 *                         calibration interval can be adjusted with configuring the SAADC_CALIBRATION_INTERVAL
 *                         constant.
 * The SAADC sample result is printed on UART. To see the UART output, a UART terminal (e.g. Realterm) can be configured on 
 * your PC with the UART configuration set in the uart_config function, which is also described in the saadc example documentation -> 
 * http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk5.v11.0.0/nrf_dev_saadc_example.html?cp=5_0_0_4_5_24
 *

 * Indicators on the nRF52-DK board:
 * LED1: SAADC Sampling triggered 
 * LED2: SAADC sampling buffer full and event received
 * LED3: SAADC Offset calibration complete
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_delay.h"
#include <joystick.h>
#include <st7735_display.h>
#include <buttons.h>
#include <debouncer.h>
 
int main(void)
{
    NRF_POWER->DCDCEN = 1;
    ret_code_t err_code;

    //Configure Logging. LOGGING is used to show the SAADC sampled result. Default is UART, but RTT can be configured in sdk_config.h
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);                     
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    
    joystick_init();
    gfx_initialization();
    buttons_init();
    debouncer_init();

    int16_t joystick_x_value;
    int16_t joystick_y_value;

    display_remote_settings();


    while(1){
      joystick_read_x_y_values(&joystick_x_value, &joystick_y_value);
      //NRF_LOG_INFO("x_value: %d y_value: %d", joystick_x_value, joystick_y_value);
      while(NRF_LOG_PROCESS() != NRF_SUCCESS);
    }
}

