/*
 * Copyright (c) 2019-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "flash_map/flash_map.h"
#include "bootutil/ramload.h"
#include "platform_base_address.h"
#include "region_defs.h"

/* Flash device name must be specified by target */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

const struct flash_area flash_map[] = {
    {
        .fa_id = FLASH_AREA_0_ID,
        .fa_device_id = FLASH_DEVICE_ID,
        .fa_driver = &FLASH_DEV_NAME,
        .fa_off = FLASH_AREA_0_OFFSET,
        .fa_size = FLASH_AREA_0_SIZE,
    },
    {
        .fa_id = FLASH_AREA_1_ID,
        .fa_device_id = FLASH_DEVICE_ID,
        .fa_driver = &FLASH_DEV_NAME,
        .fa_off = FLASH_AREA_1_OFFSET,
        .fa_size = FLASH_AREA_1_SIZE,
    },
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
};

const int flash_map_entry_num = ARRAY_SIZE(flash_map);


int boot_get_image_exec_ram_info(uint32_t image_id,
                                 uint32_t *exec_ram_start,
                                 uint32_t *exec_ram_size)
{
    int32_t rc =  -1;

    if (image_id == 0) {
        (*exec_ram_start) = SRAM_BASE;
        (*exec_ram_size) = TFM_PARTITION_SIZE;
        rc = 0;
    }
    else if (image_id == 1 || image_id == 2) {
        (*exec_ram_start) = DIPHDA_HOST_SHARED_RAM_BASE;
        (*exec_ram_size) = 0x20000000U;
        rc = 0;
    }

    return rc;
}
