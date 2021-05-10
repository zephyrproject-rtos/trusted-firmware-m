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

#include <string.h>
#include "tfm_plat_test.h"
#include <stdint.h>
#include <stdbool.h>
#include <hal/nrf_gpio.h>
#include <hal/nrf_timer.h>
#include <helpers/nrfx_reset_reason.h>
#include <nrf_board.h>
#include <region_defs.h>
#include <tfm_platform_api.h>
#include <log/tfm_log.h>

#define TIMER_RELOAD_VALUE (1*1000*1000)
#define USERLED_MASK       (1UL)

/* Area used by psa-arch-tests to keep state. */
#define PSA_TEST_SCRATCH_AREA_SIZE (0x400)

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

static void timer_init(NRF_TIMER_Type * TIMER, uint32_t ticks)
{
    nrf_timer_mode_set(TIMER, NRF_TIMER_MODE_TIMER);
    nrf_timer_bit_width_set(TIMER, NRF_TIMER_BIT_WIDTH_32);
    nrf_timer_frequency_set(TIMER, NRF_TIMER_FREQ_1MHz);
    nrf_timer_cc_set(TIMER, NRF_TIMER_CC_CHANNEL0, ticks);
    nrf_timer_one_shot_enable(TIMER, NRF_TIMER_CC_CHANNEL0);
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

    nrf_timer_task_trigger(TIMER, NRF_TIMER_TASK_CLEAR);
    nrf_timer_int_enable(TIMER, NRF_TIMER_INT_COMPARE0_MASK);

    nrf_timer_task_trigger(TIMER, NRF_TIMER_TASK_START);
}

void tfm_plat_test_secure_timer_start(void)
{
    timer_init(NRF_TIMER0, TIMER_RELOAD_VALUE);
    timer_start(NRF_TIMER0);
}

void tfm_plat_test_secure_timer_stop(void)
{
    timer_stop(NRF_TIMER0);
}

void tfm_plat_test_non_secure_timer_start(void)
{
    timer_init(NRF_TIMER1, TIMER_RELOAD_VALUE);
    timer_start(NRF_TIMER1);
}

void tfm_plat_test_non_secure_timer_stop(void)
{
    timer_stop(NRF_TIMER1);
}

void pal_timer_init_ns(uint32_t ticks)
{
    timer_init(NRF_TIMER1, ticks);
    NVIC_EnableIRQ(TIMER1_IRQn);
}

void pal_timer_start_ns(void)
{
    timer_start(NRF_TIMER1);
}

void pal_timer_stop_ns(void)
{
    timer_stop(NRF_TIMER1);
}

#if !defined(TFM_ENABLE_SLIH_TEST)
/* Watchdog timeout handler. */
void TIMER1_Handler(void)
{
    pal_timer_stop_ns();
    int ret = tfm_platform_system_reset();
    if (ret) {
        LOG_MSG("Reset failed: %d\n", ret);
    }
}
#endif

uint32_t pal_nvmem_get_addr(void)
{
    static __ALIGN(4) uint8_t __psa_scratch[PSA_TEST_SCRATCH_AREA_SIZE];
#ifdef NRF_TRUSTZONE_NONSECURE
    static bool psa_scratch_initialized = false;

    if (!psa_scratch_initialized) {
        uint32_t reset_reason = nrfx_reset_reason_get();
        nrfx_reset_reason_clear(reset_reason);

        int is_pinreset = reset_reason & NRFX_RESET_REASON_RESETPIN_MASK;
        if ((reset_reason == 0) || is_pinreset){
            /* PSA API tests expect this area to be initialized to all 0xFFs
            * after a power-on or pin reset.
            */
            memset(__psa_scratch, 0xFF, PSA_TEST_SCRATCH_AREA_SIZE);
        }
        psa_scratch_initialized = true;
    }
#endif
    return (uint32_t)__psa_scratch;
}
