/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "flash_map/flash_map.h"
#include "target.h"
#include "Driver_Flash.h"
#include "host_base_address.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

/* Flash device names must be specified by target */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

/* Default Drivers list */
const ARM_DRIVER_FLASH *flash_driver[] = {
    &FLASH_DEV_NAME,
};
const int flash_driver_entry_num = ARRAY_SIZE(flash_driver);

const struct flash_area flash_map[] = {
    {
        .fa_id = FLASH_AREA_2_ID,
        .fa_device_id = FLASH_DEVICE_ID,
        .fa_driver = &FLASH_DEV_NAME,
        .fa_off = FLASH_AREA_2_OFFSET,
        .fa_size = FLASH_AREA_2_SIZE,
    },
    {
        .fa_id = FLASH_AREA_3_ID,
        .fa_device_id = FLASH_DEVICE_ID,
        .fa_driver = &FLASH_DEV_NAME,
        .fa_off = FLASH_AREA_3_OFFSET,
        .fa_size = FLASH_AREA_3_SIZE,
    },
    {
        .fa_id = FLASH_AREA_4_ID,
        .fa_device_id = FLASH_DEVICE_ID,
        .fa_driver = &FLASH_DEV_NAME,
        .fa_off = FLASH_AREA_4_OFFSET,
        .fa_size = FLASH_AREA_4_SIZE,
    },
    {
        .fa_id = FLASH_AREA_5_ID,
        .fa_device_id = FLASH_DEVICE_ID,
        .fa_driver = &FLASH_DEV_NAME,
        .fa_off = FLASH_AREA_5_OFFSET,
        .fa_size = FLASH_AREA_5_SIZE,
    },
    {
        .fa_id = FLASH_AREA_6_ID,
        .fa_device_id = FLASH_DEVICE_ID,
        .fa_driver = &FLASH_DEV_NAME,
        .fa_off = FLASH_AREA_6_OFFSET,
        .fa_size = FLASH_AREA_6_SIZE,
    },
    {
        .fa_id = FLASH_AREA_7_ID,
        .fa_device_id = FLASH_DEVICE_ID,
        .fa_driver = &FLASH_DEV_NAME,
        .fa_off = FLASH_AREA_7_OFFSET,
        .fa_size = FLASH_AREA_7_SIZE,
    },
    {
        .fa_id = FLASH_AREA_8_ID,
        .fa_device_id = FLASH_DEVICE_ID,
        .fa_driver = &FLASH_DEV_NAME,
        .fa_off = FLASH_AREA_8_OFFSET,
        .fa_size = FLASH_AREA_8_SIZE,
    },
    {
        .fa_id = FLASH_AREA_9_ID,
        .fa_device_id = FLASH_DEVICE_ID,
        .fa_driver = &FLASH_DEV_NAME,
        .fa_off = FLASH_AREA_9_OFFSET,
        .fa_size = FLASH_AREA_9_SIZE,
    },
};

const int flash_map_entry_num = ARRAY_SIZE(flash_map);

int boot_get_image_exec_ram_info(uint32_t image_id,
                                 uint32_t *exec_ram_start,
                                 uint32_t *exec_ram_size)
{
    int rc = -1;

    if (image_id == 0) {
        *exec_ram_start = S_IMAGE_LOAD_ADDRESS;
        *exec_ram_size  = SECURE_IMAGE_MAX_SIZE;
        rc = 0;
    } else if (image_id == 1) {
        *exec_ram_start = NS_IMAGE_LOAD_ADDRESS;
        *exec_ram_size  = NON_SECURE_IMAGE_MAX_SIZE;
        rc = 0;
    } else if (image_id == 2) {
        *exec_ram_start = HOST_BOOT0_LOAD_BASE_S;
        *exec_ram_size  = AP_BL1_SIZE;
        rc = 0;
    } else if (image_id == 3) {
        *exec_ram_start = HOST_BOOT1_LOAD_BASE_S;
        *exec_ram_size  = SCP_BL1_SIZE;
        rc = 0;
    }

    return rc;
}
