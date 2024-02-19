/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "boot_hal.h"

#include <string.h>

#include "bootutil/bootutil_log.h"
#include "device_definition.h"
#include "flash_map/flash_map.h"
#include "host_base_address.h"
#include "platform_base_address.h"
#include "platform_regs.h"

#ifdef CRYPTO_HW_ACCELERATOR
#include "crypto_hw.h"
#include "fih.h"
#endif /* CRYPTO_HW_ACCELERATOR */

int32_t boot_platform_post_init(void)
{
#ifdef CRYPTO_HW_ACCELERATOR
    int32_t result;

    result = crypto_hw_accelerator_init();
    if (result) {
        return 1;
    }

    (void)fih_delay_init();
#endif /* CRYPTO_HW_ACCELERATOR */

    return 0;
}

/*
 * ================================ Secure ====================================
 */

static int boot_platform_pre_load_secure(void)
{
    return 0;
}

static int boot_platform_post_load_secure(void)
{
    return 0;
}

/*
 * ============================ Non-Secure ====================================
 */

static int boot_platform_pre_load_non_secure(void)
{
    return 0;
}

static int boot_platform_post_load_non_secure(void)
{
    return 0;
}

/*
 * ================================= VECTORS ==================================
 */

/*
 * Array of function pointers to call before each image is loaded indexed by
 * image id
 */
static int (*boot_platform_pre_load_vector[RSS_FIRMWARE_COUNT]) (void) = {
    [RSS_FIRMWARE_SECURE_ID]        = boot_platform_pre_load_secure,
#ifdef RSS_LOAD_NS_IMAGE
    [RSS_FIRMWARE_NON_SECURE_ID]    = boot_platform_pre_load_non_secure,
#endif /* RSS_LOAD_NS_IMAGE */
};

/*
 * Array of function pointers to call after each image is loaded indexed by
 * image id
 */
static int (*boot_platform_post_load_vector[RSS_FIRMWARE_COUNT]) (void) = {
    [RSS_FIRMWARE_SECURE_ID]        = boot_platform_post_load_secure,
#ifdef RSS_LOAD_NS_IMAGE
    [RSS_FIRMWARE_NON_SECURE_ID]    = boot_platform_post_load_non_secure,
#endif /* RSS_LOAD_NS_IMAGE */
};

/*
 * ============================== LOAD FUNCTIONS ==============================
 */

int boot_platform_pre_load(uint32_t image_id)
{
    if (image_id >= RSS_FIRMWARE_COUNT) {
        BOOT_LOG_WRN("BL2: no pre load for image %d", image_id);
        return 0;
    }

    return boot_platform_pre_load_vector[image_id]();
}

int boot_platform_post_load(uint32_t image_id)
{
    if (image_id >= RSS_FIRMWARE_COUNT) {
        BOOT_LOG_WRN("BL2: no post load for image %d", image_id);
        return 0;
    }

    return boot_platform_post_load_vector[image_id]();
}

bool boot_platform_should_load_image(uint32_t image_id)
{
#ifndef RSS_LOAD_NS_IMAGE
    if (image_id == RSS_FIRMWARE_NON_SECURE_ID) {
        return false;
    }
#endif /* RSS_LOAD_NS_IMAGE */

    if (image_id >= RSS_FIRMWARE_COUNT) {
        BOOT_LOG_WRN("BL2: Image %d beyond expected Firmware count: %d",
                image_id, RSS_FIRMWARE_COUNT);
        return false;
    }

    return true;
}
