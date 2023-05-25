/*
 * Copyright (c) 2019-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "boot_hal.h"
#include "region.h"
#include "device_definition.h"
#include "Driver_Flash.h"
#include "flash_layout.h"
#include "host_base_address.h"
#include "region_defs.h"
#include "platform_base_address.h"
#include "uart_stdout.h"
#include "tfm_plat_otp.h"
#include "trng.h"
#include "kmu_drv.h"
#include "platform_regs.h"
#ifdef CRYPTO_HW_ACCELERATOR
#include "fih.h"
#include "cc3xx_drv.h"
#endif /* CRYPTO_HW_ACCELERATOR */
#include <string.h>
#include "cmsis_compiler.h"
#include "fip_parser.h"
#include "host_flash_atu.h"
#include "plat_def_fip_uuid.h"
#include "tfm_plat_nv_counters.h"
#include "rss_key_derivation.h"

uint32_t image_offsets[2];

/* Flash device name must be specified by target */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Base);

uint32_t bl1_image_get_flash_offset(uint32_t image_id)
{
    switch (image_id) {
    case 0:
        return HOST_FLASH0_IMAGE0_BASE_S - FLASH_BASE_ADDRESS + image_offsets[0];
    case 1:
        return HOST_FLASH0_IMAGE1_BASE_S - FLASH_BASE_ADDRESS + image_offsets[1];
    default:
        FIH_PANIC;
    }
}

static int32_t init_atu_regions(void)
{
    enum atu_error_t err;

#ifdef RSS_USE_HOST_UART
    /* Initialize UART region */
    err = atu_initialize_region(&ATU_DEV_S,
                                get_supported_region_count(&ATU_DEV_S) - 1,
                                HOST_UART0_BASE_NS, HOST_UART_BASE,
                                HOST_UART_SIZE);
    if (err != ATU_ERR_NONE) {
        return 1;
    }
#endif /* RSS_USE_HOST_UART */

    return 0;
}

/* bootloader platform-specific hw initialization */
int32_t boot_platform_init(void)
{
    int32_t result;
    uint8_t prbg_seed[KMU_PRBG_SEED_LEN];
    enum tfm_plat_err_t plat_err;

    /* Initialize stack limit register */
    uint32_t msp_stack_bottom =
            (uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK, $$ZI$$Base);

    __set_MSPLIM(msp_stack_bottom);

    plat_err = tfm_plat_init_nv_counter();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return 1;
    }

    result = init_atu_regions();
    if (result) {
        return result;
    }

#if defined(TFM_BL1_LOGGING) || defined(TEST_BL1_1) || defined(TEST_BL1_2)
    stdio_init();
#endif /* defined(TFM_BL1_LOGGING) || defined(TEST_BL1_1) || defined(TEST_BL1_2) */

    result = FLASH_DEV_NAME.Initialize(NULL);
    if (result != ARM_DRIVER_OK) {
        return 1;
    }

    result = bl1_trng_generate_random(prbg_seed, sizeof(prbg_seed));
    if (result != 0) {
        return result;
    }

    result = kmu_init(&KMU_DEV_S, prbg_seed);
    if (result != KMU_ERROR_NONE) {
        return result;
    }

    result = host_flash_atu_init_regions_for_image(UUID_RSS_FIRMWARE_BL2, image_offsets);
    if (result) {
        return result;
    }

    return 0;
}

int32_t boot_platform_post_init(void)
{
    return 0;
}


static int invalidate_hardware_keys(void)
{
    enum kmu_error_t kmu_err;
    uint32_t slot;

    for (slot = 0; slot < KMU_USER_SLOT_MIN; slot++) {
        kmu_err = kmu_set_slot_invalid(&KMU_DEV_S, slot);
        if (kmu_err != KMU_ERROR_NONE) {
            return 1;
        }
    }

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

    result = host_flash_atu_uninit_regions();
    if (result) {
        while(1){}
    }


    result = invalidate_hardware_keys();
    if (result) {
        while(1){}
    }

#ifdef CRYPTO_HW_ACCELERATOR
    result = cc3xx_finish();
    if (result) {
        while (1);
    }
#endif /* CRYPTO_HW_ACCELERATOR */

    result = FLASH_DEV_NAME.Uninitialize();
    if (result != ARM_DRIVER_OK) {
        while (1){}
    }

#if defined(TFM_BL1_LOGGING) || defined(TEST_BL1_1) || defined(TEST_BL1_2)
    stdio_uninit();
#endif /* defined(TFM_BL1_LOGGING) || defined(TEST_BL1_1) || defined(TEST_BL1_2) */

    vt_cpy = vt;
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

    boot_jump_to_next_image(vt_cpy->reset);
}

int boot_platform_post_load(uint32_t image_id)
{
    int rc = 0;
    uint8_t key_buf[32];
    size_t key_len;

    rc = rss_derive_vhuk_seed(key_buf, sizeof(key_buf), &key_len);
    if (rc) {
        goto exit;
    }

    rc = rss_derive_vhuk(key_buf, sizeof(key_buf), KMU_USER_SLOT_MIN);
    if (rc) {
        goto exit;
    }

    rc = rss_derive_cpak_seed(KMU_USER_SLOT_MIN + 1);
    if (rc) {
        goto exit;
    }

    rc = rss_derive_dak_seed(KMU_USER_SLOT_MIN + 2);

exit:
    memset(key_buf, 0, sizeof(key_buf));

    return rc;
}
