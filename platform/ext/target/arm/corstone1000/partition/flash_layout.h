/*
 * Copyright (c) 2017-2021 Arm Limited. All rights reserved.
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

/* (Note: This is offsetted with 0x6000_0000 from the SE perspective as it is
 *  accessed via the Host)
 *
 * 0x0800_0000 Reserved                 (32 KB)
 * 0x0800_8000 GPT header               (256 KB)
 * 0x0804_8000 Reserved                 (32 KB)
 * 0x0805_0000 SE BL2   primary slot    (64 KiB)
 * 0x0806_0000 SE BL2   secondary slot  (64 KiB)
 * 0x0807_0000 TF-M     primary slot    (384 KiB)
 * 0x080D_0000 TF-M     secondary slot  (384 KiB)
 * 0x0813_0000 OPTEE    primary slot    (512 KiB)
 * 0x081B_0000 OPTEE    secondary slot  (512 KiB)
 * 0x0823_0000 U-boot   primary slot    (512 KiB)
 * 0x082B_0000 U-boot   secondary slot  (512 KiB)
 * 0x0833_0000 SPL      primary slot    (256 KiB)
 * 0x0837_0000 SPL      secondary slot  (256 KiB)
 */

/* This header file is included from linker scatter file as well, where only a
 * limited C constructs are allowed. Therefore it is not possible to include
 * here the platform_base_address.h to access flash related defines. To resolve
 * this some of the values are redefined here with different names, these are
 * marked with comment.
 */

#define MCUBOOT_MAX_IMG_SECTORS         (32)
#define FLASH_BASE_ADDRESS              (0x68000000)

#define SE_BL2_PARTITION_SIZE           (0x10000)     /* 64 KiB */
#define TFM_PARTITION_SIZE              (0x60000)     /* 384 KiB */
#define FIP_MAX_SIZE                    (0x80000)     /* 512 KiB */
#define FIP1_SIZE                       (0x151573)     /* 1350 KiB */
#define FIP2_SIZE                       (0x80000)     /* 512 KiB */
#define FIP_SIGNATURE_AREA_SIZE         (0x1000)      /* 4 KiB */


/* Secure Enclave internal SRAM */
#define SRAM_BASE                       (0x30000000)
#define SRAM_SIZE                       (0x78000)     /* 480 KiB */

#define BL2_DATA_GAP_SIZE               (0x08000)     /* 32 KiB */

#define BL1_DATA_START                  (SRAM_BASE)
#define BL1_DATA_SIZE                   (0x10000)     /* 64 KiB*/
#define BL1_DATA_LIMIT                  (BL1_DATA_START + BL1_DATA_SIZE - 1)

/* Smallest flash programmable unit in bytes */
#define TFM_HAL_FLASH_PROGRAM_UNIT      (1)

#if PLATFORM_IS_FVP
/* INTEL STRATA J3 NOR FLASH NVM */
#define STRATA_NVM_FLASH_TOTAL_SIZE     (0x04000000)  /* 32 MB Nor Flash (PMOD SF3) */
#define STRATA_NVM_FLASH_SECTOR_SIZE    (0x00001000)  /* 64 KB Sub sector size*/
#define STRATA_NVM_FLASH_PAGE_SIZE      (256U)        /* 64 KB */
#define STRATA_NVM_FLASH_PROGRAM_UNIT   (1U)          /* 4 B */

#define FLASH_DEV_NAME                  Driver_FLASH0
#define FLASH_TOTAL_SIZE                (STRATA_NVM_FLASH_TOTAL_SIZE)  /* 32 MB */
#define FLASH_AREA_IMAGE_SECTOR_SIZE    (STRATA_NVM_FLASH_SECTOR_SIZE)      /* 4 KiB */

#define STRATA_SE_FLASH_TOTAL_SIZE      (0x00800000)  /* 32 MB Nor Flash (PMOD SF3) */
#define STRATA_SE_FLASH_SECTOR_SIZE     (0x00001000)  /* 64 KB Sub sector size*/
#define STRATA_SE_FLASH_PAGE_SIZE       (256U)        /* 64 KB */
#define STRATA_SE_FLASH_PROGRAM_UNIT    (1U)          /* 4 B */

#define FLASH_DEV_NAME_SE_SECURE_FLASH  Driver_FLASH1


#else

/* PMOD SF3 NOR FLASH */
#define PMOD_SF3_FLASH_TOTAL_SIZE       (0x02000000)  /* 32 MB Nor Flash (PMOD SF3) */
#define PMOD_SF3_FLASH_SECTOR_SIZE      (0x00001000)  /* 4 KB Sub sector size*/
#define PMOD_SF3_FLASH_PAGE_SIZE        (256U)        /* 256 B */
#define PMOD_SF3_FLASH_PROGRAM_UNIT     (1U)          /* 1 B */

#define FLASH_DEV_NAME Driver_FLASH0

#define FLASH_TOTAL_SIZE                (PMOD_SF3_FLASH_TOTAL_SIZE)  /* 32 MB */
#define FLASH_AREA_IMAGE_SECTOR_SIZE    (PMOD_SF3_FLASH_SECTOR_SIZE)      /* 4 KiB */

#endif

#ifdef BL1

#define IMAGE_EXECUTABLE_RAM_START      (SRAM_BASE + BL1_DATA_SIZE)
#define IMAGE_EXECUTABLE_RAM_SIZE       (SRAM_SIZE - BL1_DATA_SIZE)

/* BL2 primary and secondary images */
#define FLASH_AREA_8_ID                 (1)
#define FLASH_AREA_8_OFFSET             (0x50000)
#define FLASH_AREA_8_SIZE               (SE_BL2_PARTITION_SIZE)

#define FLASH_AREA_9_ID                 (FLASH_AREA_8_ID + 1)
#define FLASH_AREA_9_OFFSET             (FLASH_AREA_8_OFFSET + FLASH_AREA_8_SIZE)
#define FLASH_AREA_9_SIZE               (SE_BL2_PARTITION_SIZE)

/* Macros needed to imgtool.py, used when creating BL2 signed image */
#define BL2_IMAGE_LOAD_ADDRESS          (SRAM_BASE + TFM_PARTITION_SIZE + BL2_DATA_GAP_SIZE)
#define BL2_IMAGE_OFFSET                (0x0)
#define BL2_IMAGE_MAX_SIZE              (SE_BL2_PARTITION_SIZE)

#define BL1_FLASH_AREA_IMAGE_PRIMARY(x)     (((x) == 0) ? FLASH_AREA_8_ID : \
                                                          255 )
#define BL1_FLASH_AREA_IMAGE_SECONDARY(x)   (((x) == 0) ? FLASH_AREA_9_ID : \
                                                          255 )

#define BL1_FLASH_AREA_IMAGE_SCRATCH        255

#endif /* BL1 */

/* TF-M primary and secondary images */
#define FLASH_AREA_0_ID                 (1)
#define FLASH_AREA_0_OFFSET             (0x50000 + \
                                         SE_BL2_PARTITION_SIZE + SE_BL2_PARTITION_SIZE)
#define FLASH_AREA_0_SIZE               (TFM_PARTITION_SIZE)

#define FLASH_AREA_1_ID                 (FLASH_AREA_0_ID + 1)
#define FLASH_AREA_1_OFFSET             (FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE)
#define FLASH_AREA_1_SIZE               (TFM_PARTITION_SIZE)

/* Host FIPs */
#define FLASH_FIP1_OFFSET               (FLASH_AREA_1_OFFSET + FLASH_AREA_1_SIZE + FIP_SIGNATURE_AREA_SIZE)
#define FLASH_FIP1_ADDRESS              (FLASH_BASE_ADDRESS + FLASH_FIP1_OFFSET)
#define FLASH_FIP1_SIZE                 (FIP1_SIZE)
#define FLASH_FIP2_OFFSET               (FLASH_FIP1_OFFSET + FIP_MAX_SIZE)
#define FLASH_FIP2_ADDRESS              (FLASH_BASE_ADDRESS + FLASH_FIP2_OFFSET)
#define FLASH_FIP2_SIZE                 (FIP2_SIZE)

/* Host BL2 (TF-A) primary and secondary image. */
#define FLASH_AREA_2_ID                 (FLASH_AREA_1_ID + 1)
#define FLASH_AREA_3_ID                 (FLASH_AREA_2_ID + 1)
#define FLASH_INVALID_OFFSET            (0xFFFFFFFF)
#define FLASH_INVALID_SIZE              (0xFFFFFFFF)

/* Macros needed to imgtool.py, used when creating TF-M signed image */
#define IMAGE_LOAD_ADDRESS              (SRAM_BASE)
#define SECURE_IMAGE_OFFSET             (0x0)
#define SECURE_IMAGE_MAX_SIZE           (TFM_PARTITION_SIZE)
#define NON_SECURE_IMAGE_OFFSET         (TFM_PARTITION_SIZE)
#define NON_SECURE_IMAGE_MAX_SIZE       (0x0)

#define FLASH_AREA_IMAGE_PRIMARY(x)     (((x) == 0) ? FLASH_AREA_0_ID : \
                                         ((x) == 1) ? FLASH_AREA_2_ID : \
                                                      255 )
#define FLASH_AREA_IMAGE_SECONDARY(x)   (((x) == 0) ? FLASH_AREA_1_ID : \
                                         ((x) == 1) ? FLASH_AREA_3_ID : \
                                                      255 )

#define FLASH_AREA_IMAGE_SCRATCH        255

#if PLATFORM_IS_FVP
#define FLASH_SECTOR_SIZE              (STRATA_NVM_FLASH_SECTOR_SIZE) /* 1 kB */
#else
#define FLASH_SECTOR_SIZE              (PMOD_SF3_FLASH_SECTOR_SIZE) /* 1 kB */
#endif

#define FLASH_ITS_AREA_OFFSET           (0)
#define FLASH_ITS_AREA_SIZE             (4 * FLASH_SECTOR_SIZE)  /* 4 KiB */

#define FLASH_PS_AREA_OFFSET            (FLASH_ITS_AREA_OFFSET + \
                                         FLASH_ITS_AREA_SIZE)
#define FLASH_PS_AREA_SIZE              (16 * FLASH_SECTOR_SIZE)  /* 16 KB */

/* OTP_definitions */
#define FLASH_OTP_NV_COUNTERS_AREA_OFFSET (FLASH_PS_AREA_OFFSET + \
                                           FLASH_PS_AREA_SIZE)
#define FLASH_OTP_NV_COUNTERS_AREA_SIZE   (FLASH_AREA_IMAGE_SECTOR_SIZE * 2)
#define FLASH_OTP_NV_COUNTERS_SECTOR_SIZE FLASH_AREA_IMAGE_SECTOR_SIZE

/* Internal Trusted Storage (ITS) Service definitions
 * Note: Further documentation of these definitions can be found in the
 * TF-M ITS Integration Guide.
 */
#if PLATFORM_IS_FVP
#define TFM_HAL_ITS_FLASH_DRIVER Driver_FLASH1
#else
#define TFM_HAL_ITS_FLASH_DRIVER Driver_FLASH0
#endif

/* Protected Storage (PS) Service definitions
 * Note: Further documentation of these definitions can be found in the
 * TF-M PS Integration Guide.
 */
#define TFM_HAL_PS_FLASH_DRIVER Driver_FLASH0

/* In this target the CMSIS driver requires only the offset from the base
 * address instead of the full memory address.
 */
/* Base address of dedicated flash area for ITS */
#define TFM_HAL_ITS_FLASH_AREA_ADDR     FLASH_ITS_AREA_OFFSET
/* Size of dedicated flash area for ITS */
#define TFM_HAL_ITS_FLASH_AREA_SIZE     FLASH_ITS_AREA_SIZE
/* Number of physical erase sectors per logical FS block */
#define TFM_HAL_ITS_SECTORS_PER_BLOCK   (1)
/* Smallest flash programmable unit in bytes */
#define TFM_HAL_ITS_PROGRAM_UNIT        (1)

/* In this target the CMSIS driver requires only the offset from the base
 * address instead of the full memory address.
 */
/* Base address of dedicated flash area for PS */
#define TFM_HAL_PS_FLASH_AREA_ADDR      FLASH_PS_AREA_OFFSET
/* Size of dedicated flash area for PS */
#define TFM_HAL_PS_FLASH_AREA_SIZE      FLASH_PS_AREA_SIZE
/* Number of physical erase sectors per logical FS block */
#define TFM_HAL_PS_SECTORS_PER_BLOCK    (4)
/* Smallest flash programmable unit in bytes */
#define TFM_HAL_PS_PROGRAM_UNIT         (1)

#define OTP_NV_COUNTERS_FLASH_DEV Driver_FLASH0

/* OTP / NV counter definitions */
#define TFM_OTP_NV_COUNTERS_AREA_SIZE   (FLASH_OTP_NV_COUNTERS_AREA_SIZE / 2)
#define TFM_OTP_NV_COUNTERS_AREA_ADDR   FLASH_OTP_NV_COUNTERS_AREA_OFFSET
#define TFM_OTP_NV_COUNTERS_SECTOR_SIZE FLASH_OTP_NV_COUNTERS_SECTOR_SIZE
#define TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR (TFM_OTP_NV_COUNTERS_AREA_ADDR + \
                                              TFM_OTP_NV_COUNTERS_AREA_SIZE)
#endif /* __FLASH_LAYOUT_H__ */
