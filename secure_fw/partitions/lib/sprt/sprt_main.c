/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "compiler_ext_defs.h"
#include "runtime_defs.h"
#include "sprt_partition_metadata_indicator.h"
#include "sprt_main.h"

#if defined(__ICCARM__)
#pragma required = meta_init_c
#endif

__used static uintptr_t runtime_init_c(void)
{
    return PART_METADATA()->entry;
}

__naked void sprt_main(void)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified    \n"
#endif
        "bl runtime_init_c  \n"
        "bx r0              \n"
    );
}
