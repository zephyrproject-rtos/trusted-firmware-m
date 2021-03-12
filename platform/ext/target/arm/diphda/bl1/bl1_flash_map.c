/*
 * Copyright (c) 2019-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include "flash_map/flash_map.h"
#include "flash_map_backend/flash_map_backend.h"
#include "bootutil/bootutil_log.h"
#include "sysflash/sysflash.h"
#include "Driver_Flash.h"

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
};

const int flash_map_entry_num = ARRAY_SIZE(flash_map);
