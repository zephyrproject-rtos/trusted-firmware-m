/*
 * Copyright (c) 2020 Nordic Semiconductor ASA. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "tfm_plat_test.h"
#include <stdint.h>
#include <stdbool.h>
#include <hal/nrf_gpio.h>
#include <hal/nrf_timer.h>
#include <nrf_board.h>

#define TIMER_RELOAD_VALUE (16*1024*1024)
#define USERLED_MASK       (1UL)

static bool initialized = false;

static void gpio_init(void)
{
    nrf_gpio_cfg_input(BUTTON1_PIN, BUTTON1_PULL);
    nrf_gpio_cfg_output(LED1_PIN);
    initialized = true;
}

void tfm_plat_test_wait_user_button_pressed(void)
{
    if (!initialized) gpio_init();

    /* Wait until button is pressed */
    while (nrf_gpio_pin_read(BUTTON1_PIN) != BUTTON1_ACTIVE_LEVEL) { ; }
}

void tfm_plat_test_wait_user_button_released(void)
{
    if (!initialized) gpio_init();

    /* Wait until user button 0 is released */
    while (nrf_gpio_pin_read(BUTTON1_PIN) == BUTTON1_ACTIVE_LEVEL) { ; }
}

uint32_t tfm_plat_test_get_led_status(void)
{
    if (!initialized) gpio_init();

    return (nrf_gpio_pin_out_read(LED1_PIN) == LED1_ACTIVE_LEVEL);
}

void tfm_plat_test_set_led_status(uint32_t status)
{
    if (!initialized) gpio_init();

    status &= USERLED_MASK;
    nrf_gpio_pin_write(LED1_PIN, status == LED1_ACTIVE_LEVEL);
}

uint32_t tfm_plat_test_get_userled_mask(void)
{
    return USERLED_MASK;
}

static void timer_stop(NRF_TIMER_Type * TIMER)
{
    nrf_timer_task_trigger(TIMER, NRF_TIMER_TASK_STOP);
    nrf_timer_int_disable(TIMER, NRF_TIMER_INT_COMPARE0_MASK);
    nrf_timer_event_clear(TIMER, NRF_TIMER_EVENT_COMPARE0);
}

static void timer_start(NRF_TIMER_Type * TIMER)
{
    timer_stop(TIMER);
    nrf_timer_mode_set(TIMER, NRF_TIMER_MODE_TIMER);
    nrf_timer_bit_width_set(TIMER, NRF_TIMER_BIT_WIDTH_32);
    nrf_timer_frequency_set(TIMER, NRF_TIMER_FREQ_16MHz);
    nrf_timer_cc_set(TIMER, NRF_TIMER_CC_CHANNEL0, TIMER_RELOAD_VALUE);
    nrf_timer_one_shot_enable(TIMER, NRF_TIMER_CC_CHANNEL0);

    nrf_timer_task_trigger(TIMER, NRF_TIMER_TASK_CLEAR);
    nrf_timer_int_enable(TIMER, NRF_TIMER_INT_COMPARE0_MASK);

    nrf_timer_task_trigger(TIMER, NRF_TIMER_TASK_START);
}

void tfm_plat_test_secure_timer_start(void)
{
    timer_start(NRF_TIMER0);
}

void tfm_plat_test_secure_timer_stop(void)
{
    timer_stop(NRF_TIMER0);
}

void tfm_plat_test_non_secure_timer_start(void)
{
    timer_start(NRF_TIMER1);
}

void tfm_plat_test_non_secure_timer_stop(void)
{
    timer_stop(NRF_TIMER1);
}
