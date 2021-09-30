/*
 * Copyright (c) 2019-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "cmsis.h"
#include "region.h"
#include "boot_hal.h"
#include "Driver_Flash.h"
#include "flash_layout.h"
#include "bootutil/fault_injection_hardening.h"
#include "bootutil/bootutil_log.h"
#include "fip_parser.h"
#include "flash_map/flash_map.h"
#include <string.h>

#if defined(CRYPTO_HW_ACCELERATOR) || \
    defined(CRYPTO_HW_ACCELERATOR_OTP_PROVISIONING)
#include "crypto_hw.h"
#endif

/* Flash device name must be specified by target */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

REGION_DECLARE(Image$$, ER_DATA, $$Base)[];
REGION_DECLARE(Image$$, ARM_LIB_HEAP, $$ZI$$Limit)[];

__attribute__((naked)) void boot_clear_bl2_ram_area(void)
{
    __ASM volatile(
        ".syntax unified                             \n"
        "movs    r0, #0                              \n"
        "ldr     r1, =Image$$ER_DATA$$Base           \n"
        "ldr     r2, =Image$$ARM_LIB_HEAP$$ZI$$Limit \n"
        "subs    r2, r2, r1                          \n"
        "Loop:                                       \n"
        "subs    r2, #4                              \n"
        "blt     Clear_done                          \n"
        "str     r0, [r1, r2]                        \n"
        "b       Loop                                \n"
        "Clear_done:                                 \n"
        "bx      lr                                  \n"
         : : : "r0" , "r1" , "r2" , "memory"
    );
}

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
extern struct flash_area flash_map[];

int32_t fill_bl2_flash_map_by_parsing_fips(uint32_t bank_offset)
{
    int result;
    uint32_t tfa_offset = 0;
    uint32_t tfa_size = 0;

    /* parse directly from flash using XIP mode */
    /* FIP is large so its not a good idea to load it in memory */
    result = parse_fip_and_extract_tfa_info(bank_offset + FLASH_FIP_ADDRESS,
                  FLASH_FIP_SIZE,
                  &tfa_offset, &tfa_size);
    if (result != FIP_PARSER_SUCCESS) {
        BOOT_LOG_ERR("parse_fip_and_extract_tfa_info failed");
        return 1;
    }

    flash_map[2].fa_off = FLASH_FIP_OFFSET + tfa_offset;
    flash_map[2].fa_size = tfa_size;
    flash_map[3].fa_off = flash_map[2].fa_off + flash_map[2].fa_size;
    flash_map[3].fa_size = tfa_size;

    return 0;
}

extern void add_bank_offset_to_image_offset(uint32_t bank_offset);

int32_t boot_platform_init(void)
{
    int32_t result;

    uint32_t bank_offset = BANK_0_PARTITION_OFFSET;

    result = fill_bl2_flash_map_by_parsing_fips(BANK_0_PARTITION_OFFSET);
    if (result) {
        return 1;
    }

    add_bank_offset_to_image_offset(bank_offset);

    result = FLASH_DEV_NAME.Initialize(NULL);
    if (result != ARM_DRIVER_OK) {
        return 1;
    }

#ifdef CRYPTO_HW_ACCELERATOR
    result = crypto_hw_accelerator_init();
    if (result) {
        return 1;
    }
#endif /* CRYPTO_HW_ACCELERATOR */

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

    (void)fih_delay_init();
#endif /* CRYPTO_HW_ACCELERATOR */

    result = FLASH_DEV_NAME.Uninitialize();
    if (result != ARM_DRIVER_OK) {
        while (1);
    }

    vt_cpy = vt;

    __set_MSP(vt->msp);
    __DSB();
    __ISB();

    boot_jump_to_next_image(vt_cpy->reset);
}
