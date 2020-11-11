/*
 * Copyright (c) 2020, Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cmsis_compiler.h"
#include "flash_layout.h"
#include "tfm_hal_its.h"

__WEAK void tfm_hal_its_fs_info(uint32_t *flash_area_addr, size_t *flash_area_size)
{
    if (!flash_area_addr || !flash_area_size) {
        return;
    }

    *flash_area_addr = ITS_FLASH_AREA_ADDR;
    *flash_area_size = ITS_FLASH_AREA_SIZE;
}
