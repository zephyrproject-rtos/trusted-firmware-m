/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "rse_attack_tracking_counter.h"

#include "device_definition.h"
#include "tfm_hal_platform.h"

extern volatile uint32_t * const attack_tracking_bits_ptr;
extern uint32_t attack_tracking_bits_word_size;

#define MINOR_ATTACK_TRACKING_COUNTER_MAX 128

void increment_attack_tracking_counter_major(void)
{
    uint32_t tmp;

#ifndef NDEBUG
    /* In debug modes, catch this error to see if it is a programmer error */
    tfm_hal_system_halt();
#endif

    for (uint32_t idx = 0; idx < attack_tracking_bits_word_size; idx++) {
        tmp = attack_tracking_bits_ptr[idx] + 1;

        if (tmp) {
            attack_tracking_bits_ptr[idx] = tmp;
            break;
        }
    }
}

void increment_attack_tracking_counter_minor(void)
{
    /* Use the gretexreg so that the counter persists between boot stages */
    volatile uint32_t *tracking_counter = &((struct rse_sysctrl_t *)RSE_SYSCTRL_BASE_S)->gretexreg;

#ifndef NDEBUG
    /* In debug modes, catch this error to see if it is a programmer error */
    tfm_hal_system_halt();
#endif

    *tracking_counter += 1;

    if (*tracking_counter >= MINOR_ATTACK_TRACKING_COUNTER_MAX) {
        increment_attack_tracking_counter_major();
        *tracking_counter = 0;
    }
}

