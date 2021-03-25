/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2020 Nuvoton Technology Corp. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "timer_cmsdk_drv.h"
#include "tfm_plat_defs.h"
#include "tfm_plat_test.h"
#include "device_definition.h"

#define USERLED_MASK                (0x3)
#define BTN_WAIT_INIT_COUNTER_VALUE (10000u)
#define TIMER_RELOAD_VALUE          (12000000)

void tfm_plat_test_wait_user_button_pressed(void)
{
    /* Wait until user button 0 is pressed */
    while(PF11){
        ;
    }
}

void tfm_plat_test_wait_user_button_released(void)
{
    /* Wait until user button 0 is released */
    while(!PF11){
        ;
    }
}

uint32_t tfm_plat_test_get_led_status(void)
{
    return PD2;
}

void tfm_plat_test_set_led_status(uint32_t status)
{
    PD2 = status;
}

uint32_t tfm_plat_test_get_userled_mask(void)
{
    return USERLED_MASK;
}

void tfm_plat_test_non_secure_timer_start(void)
{
    if(!timer_cmsdk_is_initialized(&CMSDK_TIMER2_DEV_NS)) {
        timer_cmsdk_init(&CMSDK_TIMER2_DEV_NS);
    }

    timer_cmsdk_set_reload_value(&CMSDK_TIMER2_DEV_NS, TIMER_RELOAD_VALUE);
    timer_cmsdk_enable(&CMSDK_TIMER2_DEV_NS);
    timer_cmsdk_enable_interrupt(&CMSDK_TIMER2_DEV_NS);
}

void tfm_plat_test_non_secure_timer_stop(void)
{
    timer_cmsdk_disable(&CMSDK_TIMER2_DEV_NS);
    timer_cmsdk_disable_interrupt(&CMSDK_TIMER2_DEV_NS);
    timer_cmsdk_clear_interrupt(&CMSDK_TIMER2_DEV_NS);
}

void tfm_plat_test_secure_timer_start(void)
{
    if (!timer_cmsdk_is_initialized(&CMSDK_TIMER0_DEV_S)) {
        timer_cmsdk_init(&CMSDK_TIMER0_DEV_S);
    }
    timer_cmsdk_set_reload_value(&CMSDK_TIMER0_DEV_S, TIMER_RELOAD_VALUE);
    timer_cmsdk_enable(&CMSDK_TIMER0_DEV_S);
    timer_cmsdk_enable_interrupt(&CMSDK_TIMER0_DEV_S);
}

void tfm_plat_test_secure_timer_stop(void)
{
    timer_cmsdk_disable(&CMSDK_TIMER0_DEV_S);
    timer_cmsdk_disable_interrupt(&CMSDK_TIMER0_DEV_S);
    timer_cmsdk_clear_interrupt(&CMSDK_TIMER0_DEV_S);
}
