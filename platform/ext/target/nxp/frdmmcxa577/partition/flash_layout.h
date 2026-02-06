/*
 * Copyright (c) 2018-2021 Arm Limited. All rights reserved.
 * Copyright 2026 NXP
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

 /* Flash layout on frdmmcxa577 without BL2:
 *
 * 0x0000_0000 Primary image area(384 KB):
 *    0x0000_0000 Secure     image primary (208 kB)
 *    0x0003_0000 Non-secure image primary (192 kB)
 * Reserved area:
 * 0xXXXX_XXXX Secure Binary tail Area (4 KB), if SB is used.
 * 0xXXXX_XXXX Protected Storage Area (16 KB)
 * 0xXXXX_XXXX Internal Trusted Storage Area (16 KB)
 * 0xXXXX_XXXX NV counters area (16 KB)
 * 0xXXXX_XXXX Unused
 */

/* This header file is included from linker scatter file as well, where only a
 * limited C constructs are allowed. Therefore it is not possible to include
 * here the platform_base_address.h to access flash related defines. To resolve
 * this some of the values are redefined here with different names, these are
 * marked with comment.
 */

/* Size of a Secure and of a Non-secure image */
#define FLASH_S_PARTITION_SIZE              (192 * 1024)       /* S partition: 208 KB : (0x34000)  */
#define FLASH_NS_PARTITION_SIZE             (192 * 1024)       /* NS partition: 192 KB : (0x30000)  */

/* Sector size of flash hardware (erase/program) */
#define FLASH_AREA_IMAGE_SECTOR_SIZE        (8*1024)           /* 8k. Flash memory erase operation granularity. FSL_FEATURE_SYSCON_FLASH_SECTOR_SIZE_BYTES */
#define FLASH_AREA_PROGRAM_SIZE             (128U)             /* 128 B as page size*/
#define FLASH_AREA_IMAGE_PHRASE_SIZE        (16U)              /* 16 B for write */

/* FLASH size */
#define FLASH_TOTAL_SIZE                    (2 * 1024 * 1024)    /* 2 MB flash. */

/* Flash layout info for BL2 bootloader */
#define FLASH_BASE_ADDRESS                  (0x00000000)
#define FLASH_BASE_S                        (0x10000000)

#ifdef SB_FILE /* Use signed Secure Binary (SB) image */
#define FLASH_SB_TAIL   0x2000 /* 8 KB */
#else
#define FLASH_SB_TAIL   0x0 /* 0 KB */
#endif

/* Secure + Non-secure image primary slot */
#define FLASH_AREA_0_ID            (1)
#define FLASH_AREA_0_OFFSET        (0x0)
#define FLASH_AREA_0_SIZE          (FLASH_S_PARTITION_SIZE + \
                                    FLASH_NS_PARTITION_SIZE + \
                                    FLASH_SB_TAIL)
                             
/* Not used*/
#define FLASH_AREA_SCRATCH_ID      (FLASH_AREA_0_ID + 1)
#define FLASH_AREA_SCRATCH_OFFSET  (FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE)
#define FLASH_AREA_SCRATCH_SIZE    (0)

/* Note: FLASH_PS_AREA_OFFSET, FLASH_ITS_AREA_OFFSET and
 * FLASH_OTP_NV_COUNTERS_AREA_OFFSET point to offsets in flash, but reads and
 * writes to these addresses are redirected to Code SRAM by Driver_Flash.c.
 */

/* Protected Storage (PS) Service definitions */
#define FLASH_PS_AREA_OFFSET            (FLASH_AREA_SCRATCH_OFFSET + \
                                         FLASH_AREA_SCRATCH_SIZE)
#define FLASH_PS_AREA_SIZE              (0x4000) /* 8 KB */

/* Internal Trusted Storage (ITS) Service definitions */
#define FLASH_ITS_AREA_OFFSET           (FLASH_PS_AREA_OFFSET + \
                                         FLASH_PS_AREA_SIZE)
#define FLASH_ITS_AREA_SIZE             (0x4000) /* 8 KB */

/* OTP_definitions */
#define FLASH_OTP_NV_COUNTERS_AREA_OFFSET (FLASH_ITS_AREA_OFFSET + \
                                           FLASH_ITS_AREA_SIZE)
#define FLASH_OTP_NV_COUNTERS_AREA_SIZE   (FLASH_AREA_IMAGE_SECTOR_SIZE * 2)
#define FLASH_OTP_NV_COUNTERS_SECTOR_SIZE FLASH_AREA_IMAGE_SECTOR_SIZE

/* Offset and size definition in flash area used by assemble.py */
#define SECURE_IMAGE_OFFSET             (0x0)
#define SECURE_IMAGE_MAX_SIZE           FLASH_S_PARTITION_SIZE

#define NON_SECURE_IMAGE_OFFSET         (SECURE_IMAGE_OFFSET + \
                                         SECURE_IMAGE_MAX_SIZE)
#define NON_SECURE_IMAGE_MAX_SIZE       FLASH_NS_PARTITION_SIZE

/* Flash device name used by BL2
 * Name is defined in flash driver file: Driver_Flash.c
 */
#define FLASH_DEV_NAME                  Driver_FLASH0
/* Smallest flash programmable unit in bytes (not used at the moment) */
#define TFM_HAL_FLASH_PROGRAM_UNIT      FLASH_AREA_IMAGE_PHRASE_SIZE

/* Protected Storage (PS) Service definitions
 * Note: Further documentation of these definitions can be found in the
 * TF-M PS Integration Guide.
 */
#define TFM_HAL_PS_FLASH_DRIVER Driver_FLASH0

/* In this target the CMSIS driver requires only the offset from the base
 * address instead of the full memory address.
 */
/* Base address of dedicated flash area for PS */
#define TFM_HAL_PS_FLASH_AREA_ADDR      (FLASH_PS_AREA_OFFSET)
/* Size of dedicated flash area for PS */
#define TFM_HAL_PS_FLASH_AREA_SIZE      FLASH_PS_AREA_SIZE
#define PS_RAM_FS_SIZE                  TFM_HAL_PS_FLASH_AREA_SIZE
/* Number of physical erase sectors per logical FS block */
#define TFM_HAL_PS_SECTORS_PER_BLOCK    1

/* Smallest flash programmable unit in bytes */
#define TFM_HAL_PS_PROGRAM_UNIT         FLASH_AREA_IMAGE_PHRASE_SIZE
/* TBD -- Not sure what is this ? */
#define PS_FLASH_NAND_BUF_SIZE          (FLASH_AREA_IMAGE_SECTOR_SIZE * \
                                        TFM_HAL_PS_SECTORS_PER_BLOCK)

/* Internal Trusted Storage (ITS) Service definitions
 * Note: Further documentation of these definitions can be found in the
 * TF-M ITS Integration Guide. The ITS should be in the internal flash, but is
 * allocated in the external flash just for development platforms that don't
 * have internal flash available.
 */
#define TFM_HAL_ITS_FLASH_DRIVER Driver_FLASH0

/* In this target the CMSIS driver requires the full memory address
 */
/* Base address of dedicated flash area for ITS */
#define TFM_HAL_ITS_FLASH_AREA_ADDR    (FLASH_ITS_AREA_OFFSET)
/* Size of dedicated flash area for ITS */
#define TFM_HAL_ITS_FLASH_AREA_SIZE    FLASH_ITS_AREA_SIZE
#define ITS_RAM_FS_SIZE                TFM_HAL_ITS_FLASH_AREA_SIZE
/* Number of physical erase sectors per logical FS block */
#define TFM_HAL_ITS_SECTORS_PER_BLOCK   1

/* Smallest flash programmable unit in bytes */
#define TFM_HAL_ITS_PROGRAM_UNIT        FLASH_AREA_IMAGE_PHRASE_SIZE

#define ITS_FLASH_NAND_BUF_SIZE        (FLASH_AREA_IMAGE_SECTOR_SIZE * \
                                        TFM_HAL_ITS_SECTORS_PER_BLOCK)

/* OTP / NV counter definitions */
#define TFM_OTP_NV_COUNTERS_AREA_SIZE   (FLASH_OTP_NV_COUNTERS_AREA_SIZE / 2)
#define TFM_OTP_NV_COUNTERS_AREA_ADDR   (FLASH_OTP_NV_COUNTERS_AREA_OFFSET)
#define TFM_OTP_NV_COUNTERS_SECTOR_SIZE FLASH_OTP_NV_COUNTERS_SECTOR_SIZE
#define TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR (TFM_OTP_NV_COUNTERS_AREA_ADDR + \
                                              TFM_OTP_NV_COUNTERS_AREA_SIZE)

/* Use Flash memory to store Code data */
#define S_ROM_ALIAS_BASE        (0x10000000)
#define NS_ROM_ALIAS_BASE       (0x00000000)

/* Use SRAM[0-4] memory to store RW data */
#define S_RAM_ALIAS_BASE        (0x30000000)
#define NS_RAM_ALIAS_BASE       (0x20000000)

#define UNUSED_RAM_SIZE         (0x00002000)  /* Reserved/Unused SRAM A0: 8 KB 32-bit RAM with ECC (ERM used for capturing memory ECC error information) */
#define RESERVED_RAM_PKC_SIZE   (0x00002000)  /* Reserved SRAM A1 and SARAM A2: (4 + 4) = total 8 KB for PKC */
#define RESERVED_RAM_SIZE       (UNUSED_RAM_SIZE + RESERVED_RAM_PKC_SIZE)

#define TOTAL_ROM_SIZE      FLASH_TOTAL_SIZE
#define TOTAL_RAM_SIZE      (0x000A0000 - RESERVED_RAM_SIZE)     /* RAM (640 - 16) = 632 KB RAM for data */

#endif /* __FLASH_LAYOUT_H__ */
