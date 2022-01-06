/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "compiler_ext_defs.h"
#include "tfm_arch.h"

/*
 * 'r0' implicitly holds the address of non-secure entry,
 * given during non-secure partition initialization.
 */
__naked void tfm_nspm_thread_entry(void)
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
