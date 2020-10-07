/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "cmsis.h"
#include "region.h"
#include "target_cfg.h"
#include "boot_hal.h"
#include "Driver_Flash.h"
#include "flash_layout.h"
#include "bootutil/fault_injection_hardening.h"

#if defined(CRYPTO_HW_ACCELERATOR) || \
    defined(CRYPTO_HW_ACCELERATOR_OTP_PROVISIONING)
#include "crypto_hw.h"
#endif

/* Flash device name must be specified by target */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

REGION_DECLARE(Image$$, ER_DATA, $$Base)[];
REGION_DECLARE(Image$$, ARM_LIB_HEAP, $$ZI$$Limit)[];
REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Base);

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

    /* Initialize stack limit register */
    uint32_t msp_stack_bottom =
            (uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK, $$ZI$$Base);

    __set_MSPLIM(msp_stack_bottom);

    result = FLASH_DEV_NAME.Initialize(NULL);
    if (result != ARM_DRIVER_OK) {
        return 1;
    }

#ifdef CRYPTO_HW_ACCELERATOR
    result = crypto_hw_accelerator_init();
    if (result) {
        return 1;
    }

    (void)fih_delay_init();
#endif /* CRYPTO_HW_ACCELERATOR */

/* This is a workaround to program the TF-M related cryptographic keys
 * to CC312 OTP memory. This functionality is independent from secure boot,
 * this is usually done on the factory floor during chip manufacturing.
 */
#ifdef CRYPTO_HW_ACCELERATOR_OTP_PROVISIONING
    printf("OTP provisioning started.\r\n");
    result = crypto_hw_accelerator_otp_provisioning();
    if (result) {
        printf("OTP provisioning FAILED: 0x%X\r\n", result);
        return 1;
    } else {
        printf("OTP provisioning succeeded. TF-M won't be loaded.\r\n");

        /* We don't need to boot - the only aim is provisioning. */
        while (1);
    }
#endif /* CRYPTO_HW_ACCELERATOR_OTP_PROVISIONING */

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
    int32_t result;

#ifdef CRYPTO_HW_ACCELERATOR
    result = crypto_hw_accelerator_finish();
    if (result) {
        while (1);
    }
#endif /* CRYPTO_HW_ACCELERATOR */

    result = FLASH_DEV_NAME.Uninitialize();
    if (result != ARM_DRIVER_OK) {
        while (1);
    }

    vt_cpy = vt;

    __set_MSPLIM(0);
    __set_MSP(vt->msp);
    __DSB();
    __ISB();

    boot_jump_to_next_image(vt_cpy->reset);
}

