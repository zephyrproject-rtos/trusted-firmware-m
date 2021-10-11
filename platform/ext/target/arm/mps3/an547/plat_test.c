/*
 * Copyright (c) 2019-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "platform_description.h"
#include "systimer_armv8-m_drv.h"
#include "syscounter_armv8-m_cntrl_drv.h"
#include "tfm_plat_test.h"
#include "device_definition.h"

#define TIMER_RELOAD_VALUE          (SystemCoreClock)

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
