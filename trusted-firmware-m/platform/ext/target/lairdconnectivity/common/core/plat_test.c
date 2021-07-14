/*
 * Copyright (c) 2020 Nordic Semiconductor ASA. All rights reserved.
 * Copyright (c) 2021 Laird Connectivity. All rights reserved.
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
#include <hal/nrf_twim.h>
#include <drivers/include/nrfx_twim.h>
#include <drivers/include/nrfx_twi_twim.h>
#include <lcz_board.h>
#include <tfm_platform_api.h>
#include <log/tfm_log.h>

#ifndef RTE_TWIM2
#error "RTE_TWIM2 must be defined to enable use of the I2C (TWI) GPIO port expander"
#endif

#define TIMER_RELOAD_VALUE          (1*1000*1000)

/* Area used by psa-arch-tests to keep state. */
#define PSA_TEST_SCRATCH_AREA_SIZE  (0x400)

/* Port expander defines */
#define TCA9538_REG_INPUT           (0x00)
#define TCA9538_REG_OUTPUT          (0x01)
#define TCA9538_REG_CONFIG          (0x03)
#define TCA9538_DEFAULT_OUTPUT      (0xff)
#define TWI_BUFFER_SIZE             (2)

static bool initialized = false;

static const nrfx_twim_t TWIM0 = NRFX_TWIM_INSTANCE(TWI_INSTANCE_NUMBER);
static const nrfx_twim_config_t TWIMConfig = {
    .scl = RTE_TWIM2_SCL_PIN,
    .sda = RTE_TWIM2_SDA_PIN,
    .frequency = TWI_FREQUENCY
};

static nrfx_twim_xfer_desc_t TWIData = {
    .address = PORT_EXPANDER_I2C_ADDRESS,
};

static uint8_t TWIBuffer[TWI_BUFFER_SIZE];

static void gpio_init(void)
{
    nrfx_err_t err;

    /* Configure I2C (TWI) port */
    err = nrfx_twim_init(&TWIM0, &TWIMConfig, NULL, NULL);

    if (err != NRFX_SUCCESS)
    {
        LOG_MSG("I2C (TWIM) init failed %d", err);
        return;
    }

    nrfx_twim_enable(&TWIM0);

    /* Configure IO pins on port expander */
    TWIData.type = NRFX_TWIM_XFER_TX;
    TWIData.p_primary_buf = TWIBuffer;
    TWIData.primary_length = 2;
    TWIBuffer[0] = TCA9538_REG_CONFIG;
    TWIBuffer[1] = PORT_EXPANDER_IO_CONFIG;
    TWIData.p_secondary_buf = NULL;
    TWIData.secondary_length = 0;

    err = nrfx_twim_xfer(&TWIM0, &TWIData, 0);

    if (err != NRFX_SUCCESS)
    {
        LOG_MSG("I2C (TWIM) transfer failed %d", err);
        return;
    }

    /* Configure port expander interrupt pin */
    nrf_gpio_cfg_input(PORT_EXPANDER_INTERRUPT_PIN, PORT_EXPANDER_INTERRUPT_PULL);

    initialized = true;
}

void tfm_plat_test_wait_user_button_pressed(void)
{
    nrfx_err_t err;

    if (!initialized) gpio_init();

    /* Wait until button is pressed */
    while (1)
    {
        /* Wait for port expander interrupt pint to be asserted */
        while (nrf_gpio_pin_read(PORT_EXPANDER_INTERRUPT_PIN) != PORT_EXPANDER_INTERRUPT_ACTIVE_LEVEL) { ; }

        /* Read the state of the IOs from the port expander */
        TWIData.type = NRFX_TWIM_XFER_TX;
        TWIData.p_primary_buf = TWIBuffer;
        TWIData.primary_length = 1;
        TWIBuffer[0] = TCA9538_REG_INPUT;
        TWIData.p_secondary_buf = NULL;
        TWIData.secondary_length = 0;

        err = nrfx_twim_xfer(&TWIM0, &TWIData, NRFX_TWIM_FLAG_TX_NO_STOP);

        if (err != NRFX_SUCCESS)
        {
            LOG_MSG("I2C (TWIM) transfer failed %d", err);
            return;
        }

        TWIData.type = NRFX_TWIM_XFER_RX;
        TWIData.p_primary_buf = TWIBuffer;
        TWIData.primary_length = 1;
        TWIBuffer[0] = 0;
        TWIData.p_secondary_buf = NULL;
        TWIData.secondary_length = 0;

        err = nrfx_twim_xfer(&TWIM0, &TWIData, 0);

        if (err != NRFX_SUCCESS)
        {
            LOG_MSG("I2C (TWIM) transfer failed %d", err);
            return;
        }

        if ((TWIBuffer[0] & PORT_EXPANDER_BUTTON1_IO) == PORT_EXPANDER_BUTTON1_ACTIVE_LEVEL)
        {
            /* Button has been pressed, break from loop */
            break;
        }
    }
}

void tfm_plat_test_wait_user_button_released(void)
{
    nrfx_err_t err;

    if (!initialized) gpio_init();

    /* Wait until button is released */
    while (1)
    {
        while (nrf_gpio_pin_read(PORT_EXPANDER_INTERRUPT_PIN) != PORT_EXPANDER_INTERRUPT_ACTIVE_LEVEL) { ; }

        /* Read the state of the IOs from the port expander */
        TWIData.type = NRFX_TWIM_XFER_TX;
        TWIData.p_primary_buf = TWIBuffer;
        TWIData.primary_length = 1;
        TWIBuffer[0] = TCA9538_REG_INPUT;
        TWIData.p_secondary_buf = NULL;
        TWIData.secondary_length = 0;

        err = nrfx_twim_xfer(&TWIM0, &TWIData, NRFX_TWIM_FLAG_TX_NO_STOP);

        if (err != NRFX_SUCCESS)
        {
            LOG_MSG("I2C (TWIM) transfer failed %d", err);
            return;
        }

        TWIData.type = NRFX_TWIM_XFER_RX;
        TWIData.p_primary_buf = TWIBuffer;
        TWIData.primary_length = 1;
        TWIBuffer[0] = 0;
        TWIData.p_secondary_buf = NULL;
        TWIData.secondary_length = 0;

        err = nrfx_twim_xfer(&TWIM0, &TWIData, 0);

        if (err != NRFX_SUCCESS)
        {
            LOG_MSG("I2C (TWIM) transfer failed %d", err);
            return;
        }

        if ((TWIBuffer[0] & PORT_EXPANDER_BUTTON1_IO) != PORT_EXPANDER_BUTTON1_ACTIVE_LEVEL)
        {
            break;
        }
    }
}

uint32_t tfm_plat_test_get_led_status(void)
{
    nrfx_err_t err;

    if (!initialized) gpio_init();

    /* Read the state of the IOs from the port expander */
    TWIData.type = NRFX_TWIM_XFER_TX;
    TWIData.p_primary_buf = TWIBuffer;
    TWIData.primary_length = 1;
    TWIBuffer[0] = TCA9538_REG_INPUT;
    TWIData.p_secondary_buf = NULL;
    TWIData.secondary_length = 0;

    err = nrfx_twim_xfer(&TWIM0, &TWIData, NRFX_TWIM_FLAG_TX_NO_STOP);

    if (err != NRFX_SUCCESS)
    {
        LOG_MSG("I2C (TWIM) transfer failed %d", err);
        return;
    }

    TWIData.type = NRFX_TWIM_XFER_RX;
    TWIData.p_primary_buf = TWIBuffer;
    TWIData.primary_length = 1;
    TWIBuffer[0] = 0;
    TWIData.p_secondary_buf = NULL;
    TWIData.secondary_length = 0;

    err = nrfx_twim_xfer(&TWIM0, &TWIData, 0);

    if (err != NRFX_SUCCESS)
    {
        LOG_MSG("I2C (TWIM) transfer failed %d", err);
        return;
    }

    return ((TWIBuffer[0] & PORT_EXPANDER_LED1_IO) == PORT_EXPANDER_LED1_ACTIVE_LEVEL);
}

void tfm_plat_test_set_led_status(uint32_t status)
{
    nrfx_err_t err;

    if (!initialized) gpio_init();

    /* Change output state on port expander */
    TWIData.type = NRFX_TWIM_XFER_TX;
    TWIData.p_primary_buf = TWIBuffer;
    TWIData.primary_length = 2;
    TWIBuffer[0] = TCA9538_REG_OUTPUT;
    if (status == 0)
    {
        /* Turn LED off */
        TWIBuffer[1] = TCA9538_DEFAULT_OUTPUT;
    }
    else
    {
        /* Turn LED on */
        TWIBuffer[1] = (TCA9538_DEFAULT_OUTPUT & (~PORT_EXPANDER_LED1_IO));
    }
    TWIData.p_secondary_buf = NULL;
    TWIData.secondary_length = 0;

    err = nrfx_twim_xfer(&TWIM0, &TWIData, 0);

    if (err != NRFX_SUCCESS)
    {
        LOG_MSG("I2C (TWIM) transfer failed %d", err);
        return;
    }
}

uint32_t tfm_plat_test_get_userled_mask(void)
{
    return PORT_EXPANDER_LED1_IO;
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
