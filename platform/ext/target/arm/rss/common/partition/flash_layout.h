/*
 * Copyright (c) 2019-2022 Arm Limited. All rights reserved.
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

#include "platform_base_address.h"

/* Flash layout on RSS with BL2 (multiple image boot):
 *
 * 0x3100_0000 BL2 - MCUBoot (128 KB)
 * 0x3102_0000 BL2 - MCUBoot (128 KB)
 * 0x3104_0000 Secure image     primary slot (512 KB)
 * 0x310C_0000 Non-secure image primary slot (512 KB)
 * 0x3114_0000 Secure image     secondary slot (512 KB)
 * 0x311C_0000 Non-secure image secondary slot (512 KB)
 * 0x3124_0000 AP BL1 primary slot (512 KB)
 * 0x312C_0000 SCP BL1 primary slot (512 KB)
 * 0x3134_0000 AP BL1 secondary slot (512 KB)
 * 0x313C_0000 SCP BL1 secondary slot (512 KB)
 * 0x3144_0000 Unused
 */

/* This header file is included from linker scatter file as well, where only a
 * limited C constructs are allowed. Therefore it is not possible to include
 * here the platform_retarget.h to access flash related defines. To resolve this
 * some of the values are redefined here with different names, these are marked
 * with comment.
 */

/* Size of a Secure and of a Non-secure image */
#define FLASH_BL2_PARTITION_SIZE        (0x20000) /* BL2 partition: 128 KB */
#define FLASH_S_PARTITION_SIZE          (0x80000) /* S   partition: 512 KB */
#define FLASH_NS_PARTITION_SIZE         (0x80000) /* NS  partition: 512 KB */
#define FLASH_AP_PARTITION_SIZE         (0x80000) /* AP  partition: 512 KB */
#define FLASH_SCP_PARTITION_SIZE        (0x80000) /* SCP partition: 512 KB */
#define FLASH_MAX_PARTITION_SIZE        ((FLASH_S_PARTITION_SIZE >   \
                                          FLASH_NS_PARTITION_SIZE) ? \
                                         FLASH_S_PARTITION_SIZE :    \
                                         FLASH_NS_PARTITION_SIZE)

/* Sector size of the flash hardware; same as FLASH0_SECTOR_SIZE */
#define FLASH_AREA_IMAGE_SECTOR_SIZE    (0x1000)    /* 4 KB */
/* Same as FLASH0_SIZE */
#define FLASH_TOTAL_SIZE                (VM0_SIZE)  /* 8 MB */

/* Flash layout info for BL2 bootloader */
/* Same as FLASH0_BASE_S */
#define FLASH_BASE_ADDRESS              (VM0_BASE_S)

/* Offset and size definitions of the flash partitions that are handled by the
 * bootloader. The image swapping is done between IMAGE_PRIMARY and
 * IMAGE_SECONDARY, SCRATCH is used as a temporary storage during image
 * swapping.
 */

/* BL2 primary slot */
#define FLASH_AREA_0_ID            (1)
#define FLASH_AREA_0_OFFSET        (0)
#define FLASH_AREA_0_SIZE          (FLASH_BL2_PARTITION_SIZE)
/* BL2 secondary slot */
#define FLASH_AREA_1_ID            (FLASH_AREA_0_ID + 1)
#define FLASH_AREA_1_OFFSET        (FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE)
#define FLASH_AREA_1_SIZE          (FLASH_BL2_PARTITION_SIZE)

/* Secure image primary slot */
#define FLASH_AREA_2_ID            (FLASH_AREA_1_ID + 1)
#define FLASH_AREA_2_OFFSET        (FLASH_AREA_1_OFFSET + FLASH_AREA_1_SIZE)
#define FLASH_AREA_2_SIZE          (FLASH_S_PARTITION_SIZE)
/* Non-secure image primary slot */
#define FLASH_AREA_3_ID            (FLASH_AREA_2_ID + 1)
#define FLASH_AREA_3_OFFSET        (FLASH_AREA_2_OFFSET + FLASH_AREA_2_SIZE)
#define FLASH_AREA_3_SIZE          (FLASH_NS_PARTITION_SIZE)
/* Secure image secondary slot */
#define FLASH_AREA_4_ID            (FLASH_AREA_3_ID + 1)
#define FLASH_AREA_4_OFFSET        (FLASH_AREA_3_OFFSET + FLASH_AREA_3_SIZE)
#define FLASH_AREA_4_SIZE          (FLASH_S_PARTITION_SIZE)
/* Non-secure image secondary slot */
#define FLASH_AREA_5_ID            (FLASH_AREA_4_ID + 1)
#define FLASH_AREA_5_OFFSET        (FLASH_AREA_4_OFFSET + FLASH_AREA_4_SIZE)
#define FLASH_AREA_5_SIZE          (FLASH_NS_PARTITION_SIZE)

/* AP image primary slot */
#define FLASH_AREA_6_ID            (FLASH_AREA_5_ID + 1)
#define FLASH_AREA_6_OFFSET        (FLASH_AREA_5_OFFSET + FLASH_AREA_5_SIZE)
#define FLASH_AREA_6_SIZE          (FLASH_AP_PARTITION_SIZE)
/* SCP image primary slot */
#define FLASH_AREA_7_ID            (FLASH_AREA_6_ID + 1)
#define FLASH_AREA_7_OFFSET        (FLASH_AREA_6_OFFSET + FLASH_AREA_6_SIZE)
#define FLASH_AREA_7_SIZE          (FLASH_SCP_PARTITION_SIZE)
/* AP image secondary slot */
#define FLASH_AREA_8_ID            (FLASH_AREA_7_ID + 1)
#define FLASH_AREA_8_OFFSET        (FLASH_AREA_7_OFFSET + FLASH_AREA_7_SIZE)
#define FLASH_AREA_8_SIZE          (FLASH_AP_PARTITION_SIZE)
/* SCP image secondary slot */
#define FLASH_AREA_9_ID            (FLASH_AREA_8_ID + 1)
#define FLASH_AREA_9_OFFSET        (FLASH_AREA_8_OFFSET + FLASH_AREA_8_SIZE)
#define FLASH_AREA_9_SIZE          (FLASH_SCP_PARTITION_SIZE)

/* Maximum number of image sectors supported by the bootloader. */
#define MCUBOOT_MAX_IMG_SECTORS    (FLASH_MAX_PARTITION_SIZE / \
                                    FLASH_AREA_IMAGE_SECTOR_SIZE)

/* Check that all the images can fit in the Flash area. */
#if (FLASH_AREA_9_OFFSET + FLASH_AREA_9_SIZE > FLASH_TOTAL_SIZE)
#error "Out of Flash memory!"
#endif

#define FLASH_AREA_IMAGE_PRIMARY(x)     (((x) == 0) ? FLASH_AREA_2_ID : \
                                         ((x) == 1) ? FLASH_AREA_3_ID : \
                                         ((x) == 2) ? FLASH_AREA_6_ID : \
                                         ((x) == 3) ? FLASH_AREA_7_ID : \
                                                      255)
#define FLASH_AREA_IMAGE_SECONDARY(x)   (((x) == 0) ? FLASH_AREA_4_ID : \
                                         ((x) == 1) ? FLASH_AREA_5_ID : \
                                         ((x) == 2) ? FLASH_AREA_8_ID : \
                                         ((x) == 3) ? FLASH_AREA_9_ID : \
                                                      255)

/* Scratch area is not used with RAM loading firmware upgrade */
#define FLASH_AREA_IMAGE_SCRATCH        255

/* Offset and size definition in flash area used by assemble.py */
#define SECURE_IMAGE_OFFSET             (0x0)
#define SECURE_IMAGE_MAX_SIZE           FLASH_S_PARTITION_SIZE

#define NON_SECURE_IMAGE_OFFSET         (SECURE_IMAGE_OFFSET + \
                                         SECURE_IMAGE_MAX_SIZE)
#define NON_SECURE_IMAGE_MAX_SIZE       FLASH_NS_PARTITION_SIZE

/* Image load addresses used by imgtool.py */
#define S_IMAGE_LOAD_ADDRESS            (VM1_BASE_S)
#define NS_IMAGE_LOAD_ADDRESS           (S_IMAGE_LOAD_ADDRESS + \
                                         SECURE_IMAGE_MAX_SIZE)

/* Flash device name used by BL2
 * Name is defined in flash driver file: Driver_Flash.c
 */
#define FLASH_DEV_NAME Driver_FLASH0
/* Smallest flash programmable unit in bytes */
#define TFM_HAL_FLASH_PROGRAM_UNIT      (0x1)

#endif /* __FLASH_LAYOUT_H__ */
