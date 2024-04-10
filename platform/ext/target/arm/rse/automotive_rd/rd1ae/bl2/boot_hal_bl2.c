/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "boot_hal.h"
#include "bootutil/bootutil_log.h"
#include "crypto_hw.h"
#include "device_definition.h"
#include "fih.h"
#include "flash_map/flash_map.h"
#include "host_base_address.h"
#include "platform_base_address.h"
#include "platform_regs.h"

#include <string.h>

/*
 * ============================ INIT FUNCTIONS =================================
 */

int32_t boot_platform_post_init(void)
{
    int32_t result;

    result = crypto_hw_accelerator_init();
    if (result) {
        return 1;
    }

    (void)fih_delay_init();

    return 0;
}

/*
 * ========================= SECURE LOAD FUNCTIONS =============================
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
 * ============================ SCP LOAD FUNCTIONS =============================
 */

/* Function called before SCP firmware is loaded. */
static int boot_platform_pre_load_scp(void)
{
    enum atu_error_t atu_err;

    BOOT_LOG_INF("BL2: SCP pre load start");

    /* Configure ATUs for loading to areas not directly addressable by RSE. */

    /*
     * Configure RSE ATU to access header region for SCP. The header part of
     * the image is loaded at the end of the ITCM to allow the code part of the
     * image to be placed at the start of the ITCM. For this, setup a separate
     * ATU region for the image header.
     */
    atu_err = atu_initialize_region(&ATU_DEV_S,
                                    RSE_ATU_IMG_HDR_LOAD_ID,
                                    HOST_SCP_HDR_ATU_WINDOW_BASE_S,
                                    HOST_SCP_HDR_PHYS_BASE,
                                    RSE_IMG_HDR_ATU_WINDOW_SIZE);
    if (atu_err != ATU_ERR_NONE) {
        return 1;
    }

    /* Configure RSE ATU to access SCP ITCM region */
    atu_err = atu_initialize_region(&ATU_DEV_S,
                                    RSE_ATU_IMG_CODE_LOAD_ID,
                                    HOST_SCP_IMG_CODE_BASE_S,
                                    HOST_SCP_PHYS_BASE,
                                    HOST_SCP_ATU_SIZE);
    if (atu_err != ATU_ERR_NONE) {
        return 1;
    }

    BOOT_LOG_INF("BL2: SCP pre load complete");

    return 0;
}

/* Function called after SCP firmware is loaded. */
static int boot_platform_post_load_scp(void)
{
    enum atu_error_t atu_err;

    BOOT_LOG_INF("BL2: SCP post load start");

    /*
     * Since the measurement are taken at this point, clear the image header
     * part in the ITCM before releasing SCP out of reset.
     */
    memset(HOST_SCP_IMG_HDR_BASE_S, 0, BL2_HEADER_SIZE);

    /* Close RSE ATU region configured to access RSE header region for SCP */
    atu_err = atu_uninitialize_region(&ATU_DEV_S, RSE_ATU_IMG_HDR_LOAD_ID);
    if (atu_err != ATU_ERR_NONE) {
        return 1;
    }

    /* Close RSE ATU region configured to access SCP ITCM region */
    atu_err = atu_uninitialize_region(&ATU_DEV_S, RSE_ATU_IMG_CODE_LOAD_ID);
    if (atu_err != ATU_ERR_NONE) {
        return 1;
    }

    BOOT_LOG_INF("BL2: SCP post load complete");

    return 0;
}


/*
 * ================================= VECTORS ==================================
 */

/*
 * Array of function pointers to call before each image is loaded indexed by
 * image id
 */
static int (*boot_platform_pre_load_vector[RSE_FIRMWARE_COUNT]) (void) = {
    [RSE_FIRMWARE_SECURE_ID]        = boot_platform_pre_load_secure,
    [RSE_FIRMWARE_SCP_ID]           = boot_platform_pre_load_scp,
};

/*
 * Array of function pointers to call after each image is loaded indexed by
 * image id
 */
static int (*boot_platform_post_load_vector[RSE_FIRMWARE_COUNT]) (void) = {
    [RSE_FIRMWARE_SECURE_ID]        = boot_platform_post_load_secure,
    [RSE_FIRMWARE_SCP_ID]           = boot_platform_post_load_scp,
};

/*
 * ============================== LOAD FUNCTIONS ==============================
 */

int boot_platform_pre_load(uint32_t image_id)
{
    if (image_id >= RSE_FIRMWARE_COUNT) {
        BOOT_LOG_WRN("BL2: no pre load for image %d", image_id);
        return 0;
    }

    return boot_platform_pre_load_vector[image_id]();
}

int boot_platform_post_load(uint32_t image_id)
{
    if (image_id >= RSE_FIRMWARE_COUNT) {
        BOOT_LOG_WRN("BL2: no post load for image %d", image_id);
        return 0;
    }

    return boot_platform_post_load_vector[image_id]();
}

bool boot_platform_should_load_image(uint32_t image_id)
{
    if (image_id == RSE_FIRMWARE_NON_SECURE_ID) {
        return false;
    }

    if (image_id >= RSE_FIRMWARE_COUNT) {
        BOOT_LOG_WRN("BL2: Image %d beyond expected Firmware count: %d",
                image_id, RSE_FIRMWARE_COUNT);
        return false;
    }

    return true;
}
