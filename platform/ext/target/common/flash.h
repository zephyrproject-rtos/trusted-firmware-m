/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Public API for FLASH drivers
 */

#ifndef __FLASH_H__
#define __FLASH_H__

/**
 * @brief FLASH Interface
 * @defgroup flash_interface FLASH Interface
 * @ingroup io_interfaces
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "bl2_util.h"  /* struct device */

#define off_t int32_t

/**
 *  @brief  Read data from flash
 *  @param  dev             : flash device
 *  @param  offset          : Offset (byte aligned) to read
 *  @param  data            : Buffer to store read data
 *  @param  len             : Number of bytes to read.
 *
 *  @return  0 on success, negative errno code on fail.
 */
int
flash_read(struct device *dev, off_t offset, void *data, size_t len);

/**
 *  @brief  Write buffer into flash memory.
 *
 *  Prior to the invocation of this API, the flash_write_protection_set needs
 *  to be called first to disable the write protection.
 *
 *  @param  dev             : flash device
 *  @param  offset          : starting offset for the write
 *  @param  data            : data to write
 *  @param  len             : Number of bytes to write
 *
 *  @return  0 on success, negative errno code on fail.
 */
int
flash_write(struct device *dev, off_t offset, const void *data, size_t len);

/**
 *  @brief  Erase part or all of a flash memory
 *
 *  Acceptable values of erase size and offset are subject to
 *  hardware-specific multiples of sector size and offset. Please check the
 *  API implemented by the underlying sub driver.
 *
 *  Prior to the invocation of this API, the flash_write_protection_set needs
 *  to be called first to disable the write protection.
 *
 *  @param  dev             : flash device
 *  @param  offset          : erase area starting offset
 *  @param  size            : size of area to be erased
 *
 *  @return  0 on success, negative errno code on fail.
 */
int
flash_erase(struct device *dev, off_t offset, size_t size);

/**
 *  @brief  Enable or disable write protection for a flash memory
 *
 *  This API is required to be called before the invocation of write or erase
 *  API. Please note that on some flash components, the write protection is
 *  automatically turned on again by the device after the completion of each
 *  write or erase calls. Therefore, on those flash parts, write protection
 *  needs to be disabled before each invocation of the write or erase API.
 *  Please refer to the sub-driver API or the data sheet of the flash component
 *  to get details on the write protection behavior.
 *
 *  @param  dev             : flash device
 *  @param  enable          : enable or disable flash write protection
 *
 *  @return  0 on success, negative errno code on fail.
 */
int
flash_write_protection_set(struct device *dev, bool enable);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __FLASH_H__ */
