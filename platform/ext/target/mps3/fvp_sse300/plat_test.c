/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "platform_description.h"
#include "systimer_armv8-m_drv.h"
#include "syscounter_armv8-m_cntrl_drv.h"
#include "tfm_plat_test.h"
#include "device_definition.h"

#define USERLED_MASK                (0x3)
#define BTN_WAIT_INIT_COUNTER_VALUE (10000u)
#define TIMER_RELOAD_VALUE          (SystemCoreClock)

/**
 * \brief Store the state of the mocked LED
 *
 * This variable have to be linked to the data section of the partition
 * TFM_SP_CORE_TEST so that in case of in case of isolation within the secure
 * domain the Core Test service can access it.
 */
uint32_t led_status
    TFM_LINK_SET_RW_IN_PARTITION_SECTION("TFM_SP_CORE_TEST")
    = 0x02u;

/**
 * \brief Simulate user reaction time
 */
static void busy_wait_to_simulate_user(void)
{
    volatile uint32_t counter = BTN_WAIT_INIT_COUNTER_VALUE;

    while (counter) {
        --counter;
    }
}

void tfm_plat_test_wait_user_button_pressed(void)
{
    busy_wait_to_simulate_user();
}

void tfm_plat_test_wait_user_button_released(void)
{
    busy_wait_to_simulate_user();
}

uint32_t tfm_plat_test_get_led_status(void)
{
    return led_status;
}

void tfm_plat_test_set_led_status(uint32_t status)
{
    led_status = status & USERLED_MASK;
}

uint32_t tfm_plat_test_get_userled_mask(void)
{
    return USERLED_MASK;
}

void tfm_plat_test_secure_timer_start(void)
{
    syscounter_armv8_m_cntrl_init(&SYSCOUNTER_CNTRL_ARMV8_M_DEV_S);

    systimer_armv8_m_init(&SYSTIMER0_ARMV8_M_DEV_S);
    systimer_armv8_m_set_timer_value(&SYSTIMER0_ARMV8_M_DEV_S, TIMER_RELOAD_VALUE);
    systimer_armv8_m_enable_interrupt(&SYSTIMER0_ARMV8_M_DEV_S);
}

void tfm_plat_test_secure_timer_stop(void)
{
    systimer_armv8_m_uninit(&SYSTIMER0_ARMV8_M_DEV_S);
    systimer_armv8_m_clear_autoinc_interrupt(&SYSTIMER0_ARMV8_M_DEV_S);
}

void tfm_plat_test_non_secure_timer_start(void)
{
    systimer_armv8_m_init(&SYSTIMER1_ARMV8_M_DEV_NS);
    systimer_armv8_m_set_timer_value(&SYSTIMER1_ARMV8_M_DEV_NS, TIMER_RELOAD_VALUE);
    systimer_armv8_m_enable_interrupt(&SYSTIMER1_ARMV8_M_DEV_NS);
}

void tfm_plat_test_non_secure_timer_stop(void)
{
    systimer_armv8_m_uninit(&SYSTIMER1_ARMV8_M_DEV_NS);
    systimer_armv8_m_clear_autoinc_interrupt(&SYSTIMER1_ARMV8_M_DEV_NS);
}
