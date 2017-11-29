/*
 * Copyright (c) 2017 ARM Limited
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

#define TOTAL_ROM_SIZE (0x00400000) /* 4MB */
#define TOTAL_RAM_SIZE (0x00200000) /* 2MB */

/*
 * MPC granularity is 128 KB on AN505 IoT Kit MPS2 FPGA image. Alignment
 * of partitions is defined in accordance with this constraint.
 */

#define BL_PARTITION_SIZE (0x80000)

/*Flash partitions on MPS2 AN505 with MCUboot:
 *
 * 0x0000_0000 MCUBoot
 * 0x0008_0000 Secure image primary
 * 0x0010_0000 Non-secure image primary
 * 0x0018_0000 Secure image secondary
 * 0x0020_0000 Non-secure image secondary
 * 0x0028_0000 Scratch area
 *
 * Flash partitions on bare metal
 * 0x0000_0000 Secure image
 * 0x0010_0000 Non-secure image
 */

#ifdef MCUBOOT
#define  S_IMAGE_PRIMARY_PARTITION_OFFSET (0x80000)
#else
#define  S_IMAGE_PRIMARY_PARTITION_OFFSET (0x0)
#endif

#define NS_IMAGE_PRIMARY_PARTITION_OFFSET (0x100000)

/*
 * Boot partition structure if MCUboot is used:
 * 0x0_0000 Bootloader header
 * 0x0_0200 Image area
 * 0x7_0000 Trailer
 */
/* IMAGE_AREA_SIZE is the space available for the software binary image.
 * It is less than the PARTITION_SIZE because we reserve space
 * for the image header and trailer introduced by the bootloader. */
#ifdef MCUBOOT
#define BL_HEADER_SIZE      (0x200)
#define BL_TRAILER_SIZE     (0x10000)
#else
/* No header if no bootloader, but keep IMAGE_AREA_SIZE the same */
#define BL_HEADER_SIZE      (0x0)
#define BL_TRAILER_SIZE     (0x10200)
#endif

#define IMAGE_AREA_SIZE \
            (BL_PARTITION_SIZE - BL_HEADER_SIZE - BL_TRAILER_SIZE)

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
            (S_IMAGE_PRIMARY_PARTITION_OFFSET + BL_HEADER_SIZE)
#define S_CODE_START    (S_ROM_ALIAS(S_IMAGE_PRIMARY_AREA_OFFSET))
#define S_CODE_SIZE     (IMAGE_AREA_SIZE - CMSE_VENEER_REGION_SIZE)
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
                        (NS_IMAGE_PRIMARY_PARTITION_OFFSET + BL_HEADER_SIZE)
#define NS_CODE_START   (NS_ROM_ALIAS(NS_IMAGE_PRIMARY_AREA_OFFSET))
#define NS_CODE_SIZE    (IMAGE_AREA_SIZE)
#define NS_CODE_LIMIT   (NS_CODE_START + NS_CODE_SIZE - 1)

/* NS partition information is used for MPC configuration */
#define NS_PARTITION_START \
            (NS_ROM_ALIAS(NS_IMAGE_PRIMARY_PARTITION_OFFSET))

#ifdef MCUBOOT
/* Cover: non-secure primary + secure secondary + non-secure secondary area */
#define NS_PARTITION_LIMIT \
            (NS_PARTITION_START + 3 * BL_PARTITION_SIZE - 1)
#else
#define NS_PARTITION_LIMIT \
            (NS_PARTITION_START + BL_PARTITION_SIZE - 1)
#endif /* MCUBOOT */

#define NS_DATA_START   (NS_RAM_ALIAS(TOTAL_RAM_SIZE/2))
#define NS_DATA_SIZE    (TOTAL_RAM_SIZE/2)
#define NS_DATA_LIMIT   (NS_DATA_START + NS_DATA_SIZE -1)

#endif /* __REGION_DEFS_H__ */

