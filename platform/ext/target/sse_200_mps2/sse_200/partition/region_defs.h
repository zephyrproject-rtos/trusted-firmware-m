/*
 * Copyright (c) 2017-2018 ARM Limited
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

#ifndef __REGION_DEFS_H__
#define __REGION_DEFS_H__

#include "flash_layout.h"

#define TOTAL_ROM_SIZE (0x00400000) /* 4MB */
#define TOTAL_RAM_SIZE (0x00200000) /* 2MB */

/*
 * MPC granularity is 128 KB on AN521 Castor MPS2 FPGA image. Alignment
 * of partitions is defined in accordance with this constraint.
 */

/*Flash partitions on MPS2 AN521 with BL2:
 *
 * 0x0000_0000 BL2 - MCUBoot
 * 0x0008_0000 Flash_area_image_0:
 *    0x0008_0000 Secure     image primary
 *    0x0010_0000 Non-secure image primary
 * 0x0018_0000 Flash_area_image_1:
 *    0x0018_0000 Secure     image secondary
 *    0x0020_0000 Non-secure image secondary
 * 0x0028_0000 Scratch area
 *
 * Flash partitions on bare metal, if BL2 not defined:
 * 0x0000_0000 Secure     image
 * 0x0010_0000 Non-secure image
 */

#ifdef BL2
#define  S_IMAGE_PRIMARY_PARTITION_OFFSET (FLASH_AREA_IMAGE_0_OFFSET)
#else
#define  S_IMAGE_PRIMARY_PARTITION_OFFSET (0x0)
#endif /* BL2 */

#define NS_IMAGE_PRIMARY_PARTITION_OFFSET (0x100000)

/*
 * Boot partition structure if MCUBoot is used:
 * 0x0_0000 Bootloader header
 * 0x0_0200 Image area
 * 0x7_0000 Trailer
 */
/* IMAGE_CODE_SIZE is the space available for the software binary image.
 * It is less than the FLASH_PARTITION_SIZE because we reserve space
 * for the image header and trailer introduced by the bootloader. */
#ifdef BL2
#define BL2_HEADER_SIZE      (0x400)
#define BL2_TRAILER_SIZE     (0x10000)
#else
/* No header if no bootloader, but keep IMAGE_CODE_SIZE the same */
#define BL2_HEADER_SIZE      (0x0)
#define BL2_TRAILER_SIZE     (0x10400)
#endif /* BL2 */

#define IMAGE_CODE_SIZE \
            (FLASH_PARTITION_SIZE - BL2_HEADER_SIZE - BL2_TRAILER_SIZE)

#define CMSE_VENEER_REGION_SIZE     (0x00000080)

/* Use SRAM1 memory to store Code data */
#define S_ROM_ALIAS_BASE  (0x10000000)
#define NS_ROM_ALIAS_BASE (0x00000000)

/* FIXME: Use SRAM2 memory to store RW data */
#define S_RAM_ALIAS_BASE  (0x38000000)
#define NS_RAM_ALIAS_BASE (0x28000000)

/* Alias definitions for secure and non-secure areas*/
#define S_ROM_ALIAS(x)  (S_ROM_ALIAS_BASE + x)
#define NS_ROM_ALIAS(x) (NS_ROM_ALIAS_BASE + x)

#define S_RAM_ALIAS(x)  (S_RAM_ALIAS_BASE + x)
#define NS_RAM_ALIAS(x) (NS_RAM_ALIAS_BASE + x)

/* Secure regions */
#define  S_IMAGE_PRIMARY_AREA_OFFSET \
            (S_IMAGE_PRIMARY_PARTITION_OFFSET + BL2_HEADER_SIZE)
#define S_CODE_START    (S_ROM_ALIAS(S_IMAGE_PRIMARY_AREA_OFFSET))
#define S_CODE_SIZE     (IMAGE_CODE_SIZE - CMSE_VENEER_REGION_SIZE)
#define S_CODE_LIMIT    (S_CODE_START + S_CODE_SIZE - 1)

#define S_DATA_START    (S_RAM_ALIAS(0x0))
#define S_DATA_SIZE     (TOTAL_RAM_SIZE/2)
#define S_DATA_LIMIT    (S_DATA_START + S_DATA_SIZE - 1)

/* CMSE Veneers region */
#define CMSE_VENEER_REGION_START  (S_CODE_LIMIT + 1)
#define CMSE_VENEER_REGION_LIMIT  (CMSE_VENEER_REGION_START + \
                                   CMSE_VENEER_REGION_SIZE - 1)

/* Non-secure regions */
#define NS_IMAGE_PRIMARY_AREA_OFFSET \
                        (NS_IMAGE_PRIMARY_PARTITION_OFFSET + BL2_HEADER_SIZE)
#define NS_CODE_START   (NS_ROM_ALIAS(NS_IMAGE_PRIMARY_AREA_OFFSET))
#define NS_CODE_SIZE    (IMAGE_CODE_SIZE)
#define NS_CODE_LIMIT   (NS_CODE_START + NS_CODE_SIZE - 1)

/* NS partition information is used for MPC configuration */
#define NS_PARTITION_START \
            (NS_ROM_ALIAS(NS_IMAGE_PRIMARY_PARTITION_OFFSET))

#ifdef BL2
/* Cover: non-secure primary + secure secondary + non-secure secondary area */
#define NS_PARTITION_LIMIT \
            (NS_PARTITION_START + 3 * FLASH_PARTITION_SIZE - 1)
#else
#define NS_PARTITION_LIMIT \
            (NS_PARTITION_START + FLASH_PARTITION_SIZE - 1)
#endif /* BL2 */

#define NS_DATA_START   (NS_RAM_ALIAS(TOTAL_RAM_SIZE/2))
#define NS_DATA_SIZE    (TOTAL_RAM_SIZE/2)
#define NS_DATA_LIMIT   (NS_DATA_START + NS_DATA_SIZE -1)

#ifdef BL2
/* Bootloader regions */
#define BL2_CODE_START    (S_ROM_ALIAS(FLASH_AREA_BL2_OFFSET))
#define BL2_CODE_SIZE     (FLASH_AREA_BL2_SIZE)
#define BL2_CODE_LIMIT    (BL2_CODE_START + BL2_CODE_SIZE - 1)

#define BL2_DATA_START    (S_RAM_ALIAS(0x0))
#define BL2_DATA_SIZE     (TOTAL_RAM_SIZE)
#define BL2_DATA_LIMIT    (BL2_DATA_START + BL2_DATA_SIZE - 1)
#endif /* BL2 */

#endif /* __REGION_DEFS_H__ */

