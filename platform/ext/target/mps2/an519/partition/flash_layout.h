/*
 * Copyright (c) 2017-2019 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __FLASH_LAYOUT_H__
#define __FLASH_LAYOUT_H__

/* Flash layout on MPS2 AN519 with BL2:
 *
 * 0x0000_0000 BL2 - MCUBoot(0.5 MB)
 * 0x0008_0000 Flash_area_image_primary(1 MB):
 *    0x0008_0000 Secure     image primary
 *    0x0010_0000 Non-secure image primary
 * 0x0018_0000 Flash_area_image_secondary(1 MB):
 *    0x0018_0000 Secure     image secondary
 *    0x0020_0000 Non-secure image secondary
 * 0x0028_0000 Scratch area(1 MB)
 * 0x0038_0000 Secure Storage Area(0.02 MB)
 * 0x0038_5000 NV counters area(20 Bytes)
 * 0x0038_5014 Unused(0.480 MB)
 *
 * Flash layout on MPS2 AN519, if BL2 not defined:
 * 0x0000_0000 Secure     image
 * 0x0010_0000 Non-secure image
 */

#define MAX(X,Y)                       ((X) > (Y) ? (X) : (Y))

/* This header file is included from linker scatter file as well, where only a
 * limited C constructs are allowed. Therefore it is not possible to include
 * here the platform_retarget.h to access flash related defines. To resolve this
 * some of the values are redefined here with different names, these are marked
 * with comment.
 */

/* Size of a Secure and of a Non-secure image */
#define FLASH_S_PARTITION_SIZE          (0x80000) /* S partition: 512 KB */
#define FLASH_NS_PARTITION_SIZE         (0x80000) /* NS partition: 512 KB */

/* Sector size of the flash hardware; same as FLASH0_SECTOR_SIZE */
#define FLASH_AREA_IMAGE_SECTOR_SIZE    (0x1000)   /* 4 KB */
/* Same as FLASH0_SIZE */
#define FLASH_TOTAL_SIZE                (0x00400000) /* 4 MB */

/* Flash layout info for BL2 bootloader */
/* Same as FLASH0_BASE_S */
#define FLASH_BASE_ADDRESS              (0x10000000)

/* Offset and size definitions of the flash partitions that are handled by the
 * bootloader. The image swapping is done between IMAGE_PRIMARY and
 * IMAGE_SECONDARY, SCRATCH is used as a temporary storage during image
 * swapping.
 */
#define FLASH_AREA_BL2_OFFSET             (0x0)
#define FLASH_AREA_BL2_SIZE               (0x80000) /* 512 KB */

#define FLASH_AREA_IMAGE_PRIMARY_OFFSET   (FLASH_AREA_BL2_OFFSET + \
                                           FLASH_AREA_BL2_SIZE)
#define FLASH_AREA_IMAGE_PRIMARY_SIZE     (FLASH_S_PARTITION_SIZE + \
                                           FLASH_NS_PARTITION_SIZE)

#define FLASH_AREA_IMAGE_SECONDARY_OFFSET (FLASH_AREA_IMAGE_PRIMARY_OFFSET + \
                                           FLASH_AREA_IMAGE_PRIMARY_SIZE)
#define FLASH_AREA_IMAGE_SECONDARY_SIZE   (FLASH_S_PARTITION_SIZE + \
                                           FLASH_NS_PARTITION_SIZE)

#define FLASH_AREA_IMAGE_SCRATCH_OFFSET   (FLASH_AREA_IMAGE_SECONDARY_OFFSET + \
                                           FLASH_AREA_IMAGE_SECONDARY_SIZE)
#define FLASH_AREA_IMAGE_SCRATCH_SIZE     (FLASH_S_PARTITION_SIZE + \
                                           FLASH_NS_PARTITION_SIZE)

/* The maximum number of status entries supported by the bootloader. */
/* The maximum number of status entries must be at least 2. For more
 * information see the MCUBoot issue:
 * https://github.com/JuulLabs-OSS/mcuboot/issues/427.
 */
#define BOOT_STATUS_MAX_ENTRIES         MAX(2, \
                                            (FLASH_S_PARTITION_SIZE + \
                                             FLASH_NS_PARTITION_SIZE) / \
                                            FLASH_AREA_IMAGE_SCRATCH_SIZE)

/* Maximum number of image sectors supported by the bootloader. */
#define BOOT_MAX_IMG_SECTORS            ((FLASH_S_PARTITION_SIZE + \
                                         FLASH_NS_PARTITION_SIZE) / \
                                         FLASH_AREA_IMAGE_SECTOR_SIZE)

/* Secure Storage (SST) Service definitions */
#define FLASH_SST_AREA_OFFSET           (FLASH_AREA_IMAGE_SCRATCH_OFFSET + \
                                         FLASH_AREA_IMAGE_SCRATCH_SIZE)
#define FLASH_SST_AREA_SIZE             (0x5000)   /* 20 KB */

/* NV Counters definitions */
#define FLASH_NV_COUNTERS_AREA_OFFSET   (FLASH_SST_AREA_OFFSET + \
                                         FLASH_SST_AREA_SIZE)
#define FLASH_NV_COUNTERS_AREA_SIZE     (0x14)     /* 20 Bytes */

/* Offset and size definition in flash area used by assemble.py */
#define SECURE_IMAGE_OFFSET             (0x0)
#define SECURE_IMAGE_MAX_SIZE           FLASH_S_PARTITION_SIZE

#define NON_SECURE_IMAGE_OFFSET         (SECURE_IMAGE_OFFSET + \
                                         SECURE_IMAGE_MAX_SIZE)
#define NON_SECURE_IMAGE_MAX_SIZE       FLASH_NS_PARTITION_SIZE

/* Concatenated binary size used by imgtool.py */
#define SIGN_BIN_SIZE                   (FLASH_S_PARTITION_SIZE + \
                                         FLASH_NS_PARTITION_SIZE)

/* Flash device name used by BL2
 * Name is defined in flash driver file: Driver_Flash.c
 */
#define FLASH_DEV_NAME Driver_FLASH0

/* Secure Storage (SST) Service definitions
 * Note: Further documentation of these definitions can be found in the
 * TF-M SST Integration Guide.
 */
#define SST_FLASH_DEV_NAME Driver_FLASH0

/* In this target the CMSIS driver requires only the offset from the base
 * address instead of the full memory address.
 */
#define SST_FLASH_AREA_ADDR     FLASH_SST_AREA_OFFSET
#define SST_SECTOR_SIZE         FLASH_AREA_IMAGE_SECTOR_SIZE
/* The sectors must be in consecutive memory location */
#define SST_NBR_OF_SECTORS      (FLASH_SST_AREA_SIZE / SST_SECTOR_SIZE)
/* Specifies the smallest flash programmable unit in bytes */
#define SST_FLASH_PROGRAM_UNIT  (0x1)
/* The maximum asset size to be stored in the SST area */
#define SST_MAX_ASSET_SIZE      (2048)
/* The maximum number of assets to be stored in the SST area */
#define SST_NUM_ASSETS          (10)

/* NV Counters definitions */
#define TFM_NV_COUNTERS_AREA_ADDR    FLASH_NV_COUNTERS_AREA_OFFSET
#define TFM_NV_COUNTERS_AREA_SIZE    FLASH_NV_COUNTERS_AREA_SIZE
#define TFM_NV_COUNTERS_SECTOR_ADDR  FLASH_NV_COUNTERS_AREA_OFFSET
#define TFM_NV_COUNTERS_SECTOR_SIZE  FLASH_AREA_IMAGE_SECTOR_SIZE

/* Use SRAM1 memory to store Code data */
#define S_ROM_ALIAS_BASE  (0x10000000)
#define NS_ROM_ALIAS_BASE (0x00000000)

/* FIXME: Use SRAM2 memory to store RW data */
#define S_RAM_ALIAS_BASE  (0x38000000)
#define NS_RAM_ALIAS_BASE (0x28000000)

#define TOTAL_ROM_SIZE FLASH_TOTAL_SIZE
#define TOTAL_RAM_SIZE (0x200000)     /* 2 MB */

/* Shared data area between bootloader and runtime firmware.
 * Shared data area is allocated at the beginning of the RAM, it is overlapping
 * with TF-M Secure code's MSP stack
 */
#define BOOT_TFM_SHARED_DATA_BASE S_RAM_ALIAS_BASE
#define BOOT_TFM_SHARED_DATA_SIZE (0x400)

#endif /* __FLASH_LAYOUT_H__ */
