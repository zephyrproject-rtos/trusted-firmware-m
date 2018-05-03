/*
 * Copyright (c) 2018 Arm Limited. All rights reserved.
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

/* Flash layout on Musca with BL2:
 * 0x0020_0000 BL2 - MCUBoot(64 KB)
 * 0x0021_0000 Flash_area_image_0(192 KB)
 *    0x0021_0000 Secure     image primary
 *    0x0023_0000 Non-secure image primary
 * 0x0024_0000 Flash_area_image_1(192 KB)
 *    0x0024_0000 Secure     image secondary
 *    0x0026_0000 Non-secure image secondary
 * 0x0027_0000 Scratch area(192 KB)
 * 0x002A_0000 Secure Storage Area (0.02 MB)
 * 0x002A_5000 Unused
 *
 * Flash layout on Musca, if BL2 not defined:
 * 0x0020_0000 Secure     image
 * 0x0022_0000 Non-secure image
 */

/* This header file is included from linker scatter file as well, where only a
 * limited C constructs are allowed. Therefore it is not possible to include
 * here the platform_retarget.h to access flash related defines. To resolve this
 * some of the values are redefined here with different names, these are marked
 * with comment.
 */

/* The size of a partition. This should be large enough to contain a S or NS
 * sw binary. Each FLASH_AREA_IMAGE contains two partitions. See Flash layout
 * above.
 */
#define FLASH_PARTITION_SIZE            (0x20000) /* 128KB */

/* Sector size of the flash hardware; same as FLASH0_SECTOR_SIZE */
#define FLASH_AREA_IMAGE_SECTOR_SIZE    (0x1000)     /* 4 kB */
/* Same as FLASH0_SIZE */
#define FLASH_TOTAL_SIZE                (0x00800000) /* 8 MB */

/* Flash layout info for BL2 bootloader */
#define FLASH_BASE_ADDRESS              (0x10200000) /* same as FLASH0_BASE_S */

/* Offset and size definitions of the flash partitions that are handled by the
 * bootloader. The image swapping is done between IMAGE_0 and IMAGE_1, SCRATCH
 * is used as a temporary storage during image swapping.
 */
#define FLASH_AREA_BL2_OFFSET           (0x0)
#ifdef BL2
#define FLASH_AREA_BL2_SIZE             (0x10000) /* 64KB */
#else
#define FLASH_AREA_BL2_SIZE             (0x0)
#endif
#define FLASH_AREA_IMAGE_SIZE           ((2 * FLASH_PARTITION_SIZE) -\
                                          FLASH_AREA_BL2_SIZE)

#define FLASH_AREA_IMAGE_0_OFFSET       (FLASH_AREA_BL2_OFFSET + \
                                         FLASH_AREA_BL2_SIZE)
#define FLASH_AREA_IMAGE_0_SIZE         (FLASH_AREA_IMAGE_SIZE)

#define FLASH_AREA_IMAGE_1_OFFSET       (FLASH_AREA_IMAGE_0_OFFSET + \
                                         FLASH_AREA_IMAGE_0_SIZE)
#define FLASH_AREA_IMAGE_1_SIZE         (FLASH_AREA_IMAGE_SIZE)

#define FLASH_AREA_IMAGE_SCRATCH_OFFSET (FLASH_AREA_IMAGE_1_OFFSET + \
                                         FLASH_AREA_IMAGE_1_SIZE)
#define FLASH_AREA_IMAGE_SCRATCH_SIZE   (FLASH_AREA_IMAGE_SIZE)

/*
 * Note: Though the SST_FLASH_AREA_ADDR is pointing to offset in flash, but
 * the actual contents of SST is stored in Code SRAM. See Driver_Flash.c for
 * more details.
 */
#define FLASH_SST_AREA_OFFSET           (FLASH_AREA_IMAGE_SCRATCH_OFFSET + \
                                         FLASH_AREA_IMAGE_SCRATCH_SIZE)
#define FLASH_SST_AREA_SIZE             (0x5000)   /* 20 KB */

/* Offset and size definition in flash area, used by assemble.py */
#define SECURE_IMAGE_OFFSET             0x0
#define SECURE_IMAGE_MAX_SIZE           0x20000

#define NON_SECURE_IMAGE_OFFSET         0x20000
#define NON_SECURE_IMAGE_MAX_SIZE       0x20000

/* Flash device name used by BL2 and SST
 * Name is defined in flash driver file: Driver_Flash.c
 */
#define FLASH_DEV_NAME Driver_FLASH0

/* Secure Storage (SST) Service definitions */
/* In this target the CMSIS driver requires only the offset from the base
 * address instead of the full memory address.
 */
#define SST_FLASH_AREA_ADDR  FLASH_SST_AREA_OFFSET
#define SST_SECTOR_SIZE      FLASH_AREA_IMAGE_SECTOR_SIZE
/* The sectors must be in consecutive memory location */
#define SST_NBR_OF_SECTORS  (FLASH_SST_AREA_SIZE / SST_SECTOR_SIZE)
/* Specifies the smallest flash programmable unit in bytes */
#define SST_FLASH_PROGRAM_UNIT  0x4

#endif /* __FLASH_LAYOUT_H__ */
