/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <flash.h>
#include <string.h>
#include "target.h"


int flash_erase(struct device *dev, off_t offset, size_t size)
{
    uint32_t address = FLASH_BASE_ADDRESS + offset;

    memset((void *)address, 0xff, size);
    return 0;
}

int flash_read(struct device *dev, off_t offset, void *data, size_t len)
{
    uint32_t address = FLASH_BASE_ADDRESS + offset;

    memcpy(data, (void *)address, len);
    return 0;
}

int flash_write(struct device *dev, off_t offset, const void *data, size_t len)
{
   uint32_t address = FLASH_BASE_ADDRESS + offset;

   memcpy((void *)address, data, len);
   return 0;
}

int flash_write_protection_set(struct device *dev, bool enable)
{
    /* Do nothing */
    return 0;
}
