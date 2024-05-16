/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "region_defs.h"
#include "boot_hal.h"

static void run_test_executable(void)
{
    static struct boot_arm_vector_table *vt_cpy;

    vt_cpy = (struct boot_arm_vector_table *)(RSE_TESTS_CODE_START);
#if defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__) \
 || defined(__ARM_ARCH_8_1M_MAIN__)
    /* Restore the Main Stack Pointer Limit register's reset value
     * before passing execution to runtime firmware to make the
     * bootloader transparent to it.
     */
    __set_MSPLIM(0);
#endif /* defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__) \
       || defined(__ARM_ARCH_8_1M_MAIN__) */

    __set_MSP(vt_cpy->msp);
    __DSB();
    __ISB();

    ((void (*)(void))vt_cpy->reset)();
}

void run_bl1_2_testsuite(void)
{
    run_test_executable();
}

void run_bl1_1_testsuite(void)
{
#ifndef TEST_BL1_2
    run_test_executable();
#endif
}
