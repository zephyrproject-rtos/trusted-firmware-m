/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "host_flash_atu.h"

#include "bl2_image_id.h"
#include "flash_layout.h"
#include "device_definition.h"
#include "flash_map/flash_map.h"

#define RSS_ATU_REGION_INPUT_IMAGE_SLOT_0 0
#define RSS_ATU_REGION_INPUT_IMAGE_SLOT_1 1
#define RSS_ATU_REGION_OUTPUT_IMAGE_SLOT  2

#define HOST_BOOT_AP_LOAD_BASE_S  HOST_BOOT_IMAGE0_LOAD_BASE_S
#define HOST_BOOT_SCP_LOAD_BASE_S HOST_BOOT_IMAGE1_LOAD_BASE_S

extern struct flash_area flash_map[];
extern int flash_map_entry_num;

static struct flash_area *flash_map_slot_from_flash_area_id(uint32_t area_id)
{
    uint32_t idx;

    for (idx = 0; idx < flash_map_entry_num; idx++) {
        if (area_id == flash_map[idx].fa_id) {
            return &flash_map[idx];
        }
    }

    return NULL;
}

static int setup_image_input_slots(uint32_t image_id)
{
    enum atu_error_t err;
    struct flash_area *flash_area_primary =
        flash_map_slot_from_flash_area_id(FLASH_AREA_IMAGE_PRIMARY(image_id));
    struct flash_area *flash_area_secondary =
        flash_map_slot_from_flash_area_id(FLASH_AREA_IMAGE_SECONDARY(image_id));

    if (flash_area_primary == NULL || flash_area_secondary == NULL) {
        return 1;
    }

    /* Initialize primary input region */
    err = atu_initialize_region(&ATU_DEV_S,
                                RSS_ATU_REGION_INPUT_IMAGE_SLOT_0,
                                HOST_FLASH0_BASE_S + flash_area_primary->fa_off,
                                HOST_FLASH0_BASE + flash_area_primary->fa_off,
                                flash_area_primary->fa_size);
    if (err != ATU_ERR_NONE) {
        return 1;
    }

    /* Initialize secondary input region */
    err = atu_initialize_region(&ATU_DEV_S,
                                RSS_ATU_REGION_INPUT_IMAGE_SLOT_1,
                                HOST_FLASH0_BASE_S + flash_area_secondary->fa_off,
                                HOST_FLASH0_BASE + flash_area_secondary->fa_off,
                                flash_area_secondary->fa_size);
    if (err != ATU_ERR_NONE) {
        return 1;
    }

    return 0;
}

int host_flash_atu_pre_load(uint32_t image_id)
{
    int err;
    enum atu_error_t atu_err;

    err = setup_image_input_slots(image_id);
    if (err != 0) {
        return err;
    }

    switch (image_id) {
    case RSS_BL2_IMAGE_SCP:
        /* Initialize SCP ATU output region */
        atu_err = atu_initialize_region(&ATU_DEV_S,
                                        RSS_ATU_REGION_OUTPUT_IMAGE_SLOT,
                                        HOST_BOOT_SCP_LOAD_BASE_S,
                                        SCP_BL1_SRAM_BASE, SCP_BL1_SIZE);
        if (atu_err != ATU_ERR_NONE) {
            return 1;
        }
        break;
    case RSS_BL2_IMAGE_AP:
        /* Initialize AP ATU region */
        atu_err = atu_initialize_region(&ATU_DEV_S,
                                        RSS_ATU_REGION_OUTPUT_IMAGE_SLOT,
                                        HOST_BOOT_AP_LOAD_BASE_S,
                                        AP_BL1_SRAM_BASE, AP_BL1_SIZE);
        if (atu_err != ATU_ERR_NONE) {
            return 1;
        }
        break;
    case RSS_BL2_IMAGE_NS:
#ifndef RSS_XIP
        /* Initialize AP ATU region */
        atu_err = atu_initialize_region(&ATU_DEV_S,
                                        RSS_ATU_REGION_OUTPUT_IMAGE_SLOT,
                                        HOST_BOOT_AP_LOAD_BASE_S,
                                        AP_BL1_SRAM_BASE, AP_BL1_SIZE);
        if (atu_err != ATU_ERR_NONE) {
            return 1;
        }
#endif /* !RSS_XIP */
        break;
    case RSS_BL2_IMAGE_S:
#ifndef RSS_XIP
        /* Initialize AP ATU region */
        atu_err = atu_initialize_region(&ATU_DEV_S,
                                        RSS_ATU_REGION_OUTPUT_IMAGE_SLOT,
                                        HOST_BOOT_AP_LOAD_BASE_S,
                                        AP_BL1_SRAM_BASE, AP_BL1_SIZE);
        if (atu_err != ATU_ERR_NONE) {
            return 1;
        }
#endif /* !RSS_XIP */
        break;
    default:
        return 1;
    }

    return 0;
}

int host_flash_atu_post_load(uint32_t image_id)
{
    enum atu_error_t atu_err;
    (void)image_id;

    atu_err = atu_uninitialize_region(&ATU_DEV_S,
                                      RSS_ATU_REGION_INPUT_IMAGE_SLOT_0);
    if (atu_err != ATU_ERR_NONE) {
        return 1;
    }

    atu_err = atu_uninitialize_region(&ATU_DEV_S,
                                      RSS_ATU_REGION_INPUT_IMAGE_SLOT_1);
    if (atu_err != ATU_ERR_NONE) {
        return 1;
    }

    atu_err = atu_uninitialize_region(&ATU_DEV_S,
                                      RSS_ATU_REGION_OUTPUT_IMAGE_SLOT);
    if (atu_err != ATU_ERR_NONE) {
        return 1;
    }

    return 0;
}
