/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2021, Laird Connectivity. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cmsis.h"
#include "region.h"
#include "target_cfg.h"
#include "cmsis.h"
#include "boot_hal.h"
#include "Driver_Flash.h"
#include "flash_layout.h"
#include "region_defs.h"
#include "RTE_Device.h"

REGION_DECLARE(Image$$, ER_DATA, $$Base)[];
REGION_DECLARE(Image$$, ARM_LIB_HEAP, $$ZI$$Limit)[];

#ifdef RTE_FLASH0
extern ARM_DRIVER_FLASH Driver_FLASH0;
#endif
#ifdef RTE_QSPI0
extern ARM_DRIVER_FLASH Driver_FLASH1;
#endif

__attribute__((naked)) void boot_clear_bl2_ram_area(void)
{
    __ASM volatile(
        "mov     r0, #0                              \n"
        "subs    %1, %1, %0                          \n"
        "Loop:                                       \n"
        "subs    %1, #4                              \n"
        "itt     ge                                  \n"
        "strge   r0, [%0, %1]                        \n"
        "bge     Loop                                \n"
        "bx      lr                                  \n"
        :
        : "r" (REGION_NAME(Image$$, ER_DATA, $$Base)),
          "r" (REGION_NAME(Image$$, ARM_LIB_HEAP, $$ZI$$Limit))
        : "r0", "memory"
    );
}

int32_t boot_platform_init(void)
{
    int32_t result;

#ifdef RTE_FLASH0
    result = Driver_FLASH0.Initialize(NULL);
    if (result != ARM_DRIVER_OK) {
        return 1;
    }
#endif

#ifdef RTE_QSPI0
    result = Driver_FLASH1.Initialize(NULL);
    if (result != ARM_DRIVER_OK) {
        return 1;
    }
#endif

    return 0;
}

void boot_platform_quit(struct boot_arm_vector_table *vt)
{
    /* Clang at O0, stores variables on the stack with SP relative addressing.
     * When manually set the SP then the place of reset vector is lost.
     * Static variables are stored in 'data' or 'bss' section, change of SP has
     * no effect on them.
     */
    static struct boot_arm_vector_table *vt_cpy;

    vt_cpy = vt;

#if RTE_FLASH0
    Driver_FLASH0.Uninitialize();
#endif

#if RTE_QSPI0
    Driver_FLASH1.Uninitialize();
#endif

#if defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__)
    /* Restore the Main Stack Pointer Limit register's reset value
     * before passing execution to runtime firmware to make the
     * bootloader transparent to it.
     */
    __set_MSPLIM(0);
#endif
    __set_MSP(vt_cpy->msp);
    __DSB();
    __ISB();

    boot_jump_to_next_image(vt_cpy->reset);
}
