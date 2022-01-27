/*
 * Copyright (c) 2021-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "compiler_ext_defs.h"
#include "tfm_arch.h"
#include "tfm_hal_platform.h"

#if defined(__ICCARM__)
#pragma required = ns_agent_tz_init_c
#endif

/*
 * Initialization before launching NSPE in C.
 */
__used static uint32_t ns_agent_tz_init_c(void)
{
#ifndef TFM_MULTI_CORE_TOPOLOGY
    /* SCB_NS.VTOR points to the Non-secure vector table base address */
    SCB_NS->VTOR = tfm_hal_get_ns_VTOR();

    /* Setups Main stack pointer of the non-secure code */
    uint32_t ns_msp = tfm_hal_get_ns_MSP();

    __TZ_set_MSP_NS(ns_msp);
#endif

    return tfm_hal_get_ns_entry_point();
}

/*
 * 'r0' implicitly holds the address of non-secure entry,
 * given during non-secure partition initialization.
 */
__naked void tfm_nspm_thread_entry(void)
{
    __ASM volatile(
#ifndef __ICCARM__
        ".syntax unified                \n"
#endif
        "bl       ns_agent_tz_init_c    \n"
        "mov      r4, r0                \n"
        "movs     r2, #1                \n" /* address[0]:0 for NS execution */
        "bics     r4, r2                \n"
        "mov      r0, r4                \n"
        "mov      r1, r4                \n"
        "mov      r2, r4                \n"
        "mov      r3, r4                \n"
        "mov      r5, r4                \n"
        "mov      r6, r4                \n"
        "mov      r7, r4                \n"
        "mov      r8, r4                \n"
        "mov      r9, r4                \n"
        "mov      r10, r4               \n"
        "mov      r11, r4               \n"
        "mov      r12, r4               \n"
        "mov      r14, r4               \n"
        "bxns     r0                    \n"
    );
}
