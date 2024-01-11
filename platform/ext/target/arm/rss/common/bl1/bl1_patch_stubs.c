/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "region_defs.h"

#include <stdint.h>
#include <stdbool.h>

/* Since this gets run at the start of every relocatable function, use it to
 * make sure that the GOT register hasn't been clobbered
 */
void __cyg_profile_func_enter(void *func, void *callsite)
{
    __asm volatile(
        "mov r9, %0 \n"
        "mov r2, %1 \n"
        "lsl r9, #16 \n"
        "orr r9, r9, r2 \n"
        : : "I" (BL1_1_DATA_START >> 16), "I" (BL1_1_DATA_START & 0xFFFF) : "r2"
    );
}

void __cyg_profile_func_exit(void *func, void *callsite)
{
}
