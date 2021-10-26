/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "compiler_ext_defs.h"
#include "tfm_spm_hal.h"

/*
 * 'r0' impliedly holds the address of non-secure entry,
 * given during non-secure partition initialization.
 */
__section("SFN") __naked
void tfm_nspm_thread_entry(void)
{
    __ASM volatile(
#ifndef __ICCARM__
        ".syntax unified         \n"
#endif
        "mov      r4, r0         \n"
        "movs     r2, #1         \n" /* Clear Bit[0] for S to NS transition */
        "bics     r4, r2         \n"
        "mov      r0, r4         \n"
        "mov      r1, r4         \n"
        "mov      r2, r4         \n"
        "mov      r3, r4         \n"
        "mov      r5, r4         \n"
        "mov      r6, r4         \n"
        "mov      r7, r4         \n"
        "mov      r8, r4         \n"
        "mov      r9, r4         \n"
        "mov      r10, r4        \n"
        "mov      r11, r4        \n"
        "mov      r12, r4        \n"
        "bxns     r0             \n"
    );
}

void configure_ns_code(void)
{
    /* SCB_NS.VTOR points to the Non-secure vector table base address */
    SCB_NS->VTOR = tfm_spm_hal_get_ns_VTOR();

    /* Setups Main stack pointer of the non-secure code */
    uint32_t ns_msp = tfm_spm_hal_get_ns_MSP();

    __TZ_set_MSP_NS(ns_msp);
}
