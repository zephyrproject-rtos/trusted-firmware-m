/*
 * Copyright (c) 2017-2020 Arm Limited. All rights reserved.
 * Copyright (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
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

/* Flash layout with BL2:
 *
 * Not supported
 *
 * Flash layout if BL2 not defined:
 *
 * 0x1000_0000 Secure     image primary (320 KB)
 * 0x1005_0000 Non-secure image primary (1152 KB)
 * 0x1017_0000 Secure     image secondary (320 KB)
 * 0x101c_0000 - 0x101f_ffff Reserved
 *  0x101c_0000 Internal Trusted Storage Area (16 KB)
 *  0x101c_4000 NV counters area (1 KB)
 *  0x101c_4400 Scratch area (27 KB)
 *  0x101c_b000 Protected Storage Area (20 KB)
 *  0x101d_0000 Reserved (192 KB)
 * 0x101f_ffff End of Flash
 *
 */

#define MAX(X, Y)                       (((X) > (Y)) ? (X) : (Y))

/* This header file is included from linker scatter file as well, where only a
 * limited C constructs are allowed. Therefore it is not possible to include
 * here the platform_base_address.h to access flash related defines. To resolve
 * this some of the values are redefined here with different names, these are
 * marked with comment.
 */

/* The size of S partition */
#define FLASH_S_PARTITION_SIZE          0x50000      /* 320 KB */
/* The size of NS partition */
#define FLASH_NS_PARTITION_SIZE         0x120000     /* 1152 KB */

/* Sector size of the flash hardware; same as FLASH0_SECTOR_SIZE */
#define FLASH_AREA_IMAGE_SECTOR_SIZE    (0x200)      /* 512 B */
/* Same as FLASH0_SIZE */
#define FLASH_TOTAL_SIZE                (0x00200000) /* 2 MB */

/* Flash layout info for BL2 bootloader */
#define FLASH_BASE_ADDRESS              (0x10000000U) /* same as FLASH0_BASE */

/* Reserved areas */
#define FLASH_RESERVED_AREA_OFFSET      (SECURE_IMAGE_OFFSET + \
                                         2*SECURE_IMAGE_MAX_SIZE + \
                                         NON_SECURE_IMAGE_MAX_SIZE)

/* FixMe: implement proper mcuboot partitioning for CYBL */

/* Internal Trusted Storage Area */
#define FLASH_ITS_AREA_OFFSET           (FLASH_RESERVED_AREA_OFFSET)
#define FLASH_ITS_AREA_SIZE             (0x4000)    /* 16 KB */

/* Non-volatile Counters Area */
#define FLASH_NV_COUNTERS_AREA_OFFSET   (FLASH_ITS_AREA_OFFSET + \
                                         FLASH_ITS_AREA_SIZE)
#define FLASH_NV_COUNTERS_AREA_SIZE     (2 * FLASH_AREA_IMAGE_SECTOR_SIZE)

#ifdef BL2
#error "BL2 configuration is not supported"
#endif /* BL2 */

/* Scratch Area - unused */
#define FLASH_AREA_SCRATCH_OFFSET       (FLASH_NV_COUNTERS_AREA_OFFSET + \
                                         FLASH_NV_COUNTERS_AREA_SIZE)
#define FLASH_AREA_SCRATCH_SIZE         (0x6c00)   /* 27 KB */

/* Protected Storage Area */
#define FLASH_PS_AREA_OFFSET            (FLASH_AREA_SCRATCH_OFFSET + \
                                         FLASH_AREA_SCRATCH_SIZE)
#define FLASH_PS_AREA_SIZE              (0x5000)   /* 20 KB */

#define FLASH_AREA_SYSTEM_RESERVED_SIZE (0x30000) /* 192 KB */


/* Secure and non-secure images definition in flash area */

#define SECURE_IMAGE_OFFSET             0x0

#define SECURE_IMAGE_MAX_SIZE           FLASH_S_PARTITION_SIZE

#define NON_SECURE_IMAGE_OFFSET         (SECURE_IMAGE_OFFSET + \
                                         SECURE_IMAGE_MAX_SIZE)

#define NON_SECURE_IMAGE_MAX_SIZE       FLASH_NS_PARTITION_SIZE

/* Check if it fits into available Flash*/

#define FLASH_RESERVED_AREA_SIZE        (FLASH_ITS_AREA_SIZE + \
                                         FLASH_NV_COUNTERS_AREA_SIZE + \
                                         FLASH_AREA_SCRATCH_SIZE + \
                                         FLASH_PS_AREA_SIZE + \
                                         FLASH_AREA_SYSTEM_RESERVED_SIZE)

#if (FLASH_RESERVED_AREA_OFFSET + FLASH_RESERVED_AREA_SIZE) > (FLASH_TOTAL_SIZE)
#error "Out of Flash memory"
#endif

/* Flash device name used by BL2 and PS
 * Name is defined in flash driver file: Driver_Flash.c
 */
#define FLASH_DEV_NAME Driver_FLASH0

/* Protected Storage (PS) Service definitions
 * Note: Further documentation of these definitions can be found in the
 * TF-M PS Integration Guide.
 */
#define PS_FLASH_DEV_NAME Driver_FLASH0

/* In this target the CMSIS driver requires only the offset from the base
 * address instead of the full memory address.
 */
#define PS_FLASH_AREA_ADDR     FLASH_PS_AREA_OFFSET
/* Dedicated flash area for PS */
#define PS_FLASH_AREA_SIZE     FLASH_PS_AREA_SIZE
#define PS_SECTOR_SIZE         FLASH_AREA_IMAGE_SECTOR_SIZE
/* Number of PS_SECTOR_SIZE per block */
#define PS_SECTORS_PER_BLOCK   0x8
/* Specifies the smallest flash programmable unit in bytes */
#define PS_FLASH_PROGRAM_UNIT  0x1
/* The maximum asset size to be stored in the PS area */
#define PS_MAX_ASSET_SIZE      2048
/* The maximum number of assets to be stored in the PS area */
#define PS_NUM_ASSETS          10

/* Internal Trusted Storage (ITS) Service definitions
 * Note: Further documentation of these definitions can be found in the
 * TF-M ITS Integration Guide. The ITS should be in the internal flash, but is
 * allocated in the external flash just for development platforms that don't
 * have internal flash available.
 */
#define ITS_FLASH_DEV_NAME Driver_FLASH0

/* In this target the CMSIS driver requires only the offset from the base
 * address instead of the full memory address.
 */
#define ITS_FLASH_AREA_ADDR     FLASH_ITS_AREA_OFFSET
/* Dedicated flash area for ITS */
#define ITS_FLASH_AREA_SIZE     FLASH_ITS_AREA_SIZE
#define ITS_SECTOR_SIZE         FLASH_AREA_IMAGE_SECTOR_SIZE
/* Number of ITS_SECTOR_SIZE per block */
#define ITS_SECTORS_PER_BLOCK   (0x8)
/* Specifies the smallest flash programmable unit in bytes */
#define ITS_FLASH_PROGRAM_UNIT  (0x1)
/* The maximum asset size to be stored in the ITS area */
#define ITS_MAX_ASSET_SIZE      (512)
/* The maximum number of assets to be stored in the ITS area */
#define ITS_NUM_ASSETS          (10)
/* Decrease flash wear slightly, at the cost of increased ITS service memory */
#define ITS_MAX_BLOCK_DATA_COPY 512

/* NV Counters definitions */
#define TFM_NV_COUNTERS_FLASH_DEV    Driver_FLASH0
#define TFM_NV_COUNTERS_AREA_SIZE    FLASH_NV_COUNTERS_AREA_SIZE
#define TFM_NV_COUNTERS_SECTOR_ADDR  FLASH_NV_COUNTERS_AREA_OFFSET
#define TFM_NV_COUNTERS_SECTOR_SIZE  FLASH_AREA_IMAGE_SECTOR_SIZE

/* Use Flash to store Code data */
#define S_ROM_ALIAS_BASE  (0x10000000)
#define NS_ROM_ALIAS_BASE (0x10000000)

/* Use SRAM to store RW data */
#define S_RAM_ALIAS_BASE  (0x08000000)
#define NS_RAM_ALIAS_BASE (0x08000000)

#endif /* __FLASH_LAYOUT_H__ */
