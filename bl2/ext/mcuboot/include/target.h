/*
 *  Copyright (C) 2017, Linaro Ltd
 *  Copyright (c) 2018-2019, Arm Limited.
 *
 *  SPDX-License-Identifier: Apache-2.0
 */

#ifndef H_TARGETS_TARGET_
#define H_TARGETS_TARGET_

/* Target specific defines: flash partitions; flash driver name, etc.
 * Comes from: platform/ext/target/<BOARD>/<SUBSYSTEM>/partition
 */
#include "flash_layout.h"

#ifndef FLASH_BASE_ADDRESS
#error "FLASH_BASE_ADDRESS must be defined by the target"
#endif

#ifndef FLASH_AREA_IMAGE_SECTOR_SIZE
#error "FLASH_AREA_IMAGE_SECTOR_SIZE must be defined by the target"
#endif

#ifndef FLASH_AREA_IMAGE_PRIMARY_OFFSET
#error "FLASH_AREA_IMAGE_PRIMARY_OFFSET must be defined by the target"
#endif

#ifndef FLASH_AREA_IMAGE_PRIMARY_SIZE
#error "FLASH_AREA_IMAGE_PRIMARY_SIZE must be defined by the target"
#endif

#ifndef FLASH_AREA_IMAGE_SECONDARY_OFFSET
#error "FLASH_AREA_IMAGE_SECONDARY_OFFSET must be defined by the target"
#endif

#ifndef FLASH_AREA_IMAGE_SECONDARY_SIZE
#error "FLASH_AREA_IMAGE_SECONDARY_SIZE must be defined by the target"
#endif

#ifndef FLASH_AREA_IMAGE_SCRATCH_OFFSET
#error "FLASH_AREA_IMAGE_SCRATCH_OFFSET must be defined by the target"
#endif

#ifndef FLASH_AREA_IMAGE_SCRATCH_SIZE
#error "FLASH_AREA_IMAGE_SCRATCH_SIZE must be defined by the target"
#endif

#ifndef FLASH_DEV_NAME
#error "BL2 supports CMSIS flash interface and device name must be specified"
#endif

#ifndef BOOT_STATUS_MAX_ENTRIES
#error "BOOT_STATUS_MAX_ENTRIES must be defined by the target"
#endif

#ifndef BOOT_MAX_IMG_SECTORS
#error "BOOT_MAX_IMG_SECTORS must be defined by the target"
#endif

#endif
