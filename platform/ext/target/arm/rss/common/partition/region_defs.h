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

#ifndef __REGION_DEFS_H__
#define __REGION_DEFS_H__

#include "flash_layout.h"
#include "platform_base_address.h"

#define BL1_1_HEAP_SIZE         (0x0001000)
#define BL1_1_MSP_STACK_SIZE    (0x0001800)

#define BL1_2_HEAP_SIZE         (0x0001000)
#define BL1_2_MSP_STACK_SIZE    (0x0001800)

#define BL2_HEAP_SIZE           (0x0001000)
#define BL2_MSP_STACK_SIZE      (0x0001800)

#define S_HEAP_SIZE             (0x0001000)
#define S_MSP_STACK_SIZE        (0x0000800)

#define NS_HEAP_SIZE            (0x0001000)
#define NS_STACK_SIZE           (0x0001000)

/* This size of buffer is big enough to store an attestation
 * token produced by initial attestation service
 */
#define PSA_INITIAL_ATTEST_TOKEN_MAX_SIZE   (0x800)
/* This size of buffer is big enough to store an array of all the
 * boot records/measurements which is encoded in CBOR format.
 */
#define TFM_ATTEST_BOOT_RECORDS_MAX_SIZE    (0x400)

#define S_IMAGE_SECONDARY_PARTITION_OFFSET (FLASH_AREA_4_OFFSET)

/* Boot partition structure if MCUBoot is used:
 * 0x0_0000 Bootloader header
 * 0x0_0400 Image area
 * 0x5_0000 Trailer
 */
/* IMAGE_CODE_SIZE is the space available for the software binary image.
 * It is less than the FLASH_S_PARTITION_SIZE + FLASH_NS_PARTITION_SIZE
 * because we reserve space for the image header and trailer introduced
 * by the bootloader.
 */
#define IMAGE_BL2_CODE_SIZE \
            (FLASH_BL2_PARTITION_SIZE - BL1_HEADER_SIZE - BL1_TRAILER_SIZE)

#define IMAGE_S_CODE_SIZE \
            (FLASH_S_PARTITION_SIZE - BL2_HEADER_SIZE - BL2_TRAILER_SIZE)

#define IMAGE_NS_CODE_SIZE \
            (FLASH_NS_PARTITION_SIZE - BL2_HEADER_SIZE - BL2_TRAILER_SIZE)

/* Secure regions */
/* Secure Code executes from VM1 */
#define S_CODE_START    (S_IMAGE_LOAD_ADDRESS + BL2_HEADER_SIZE)
#define S_CODE_SIZE     (IMAGE_S_CODE_SIZE)
#define S_CODE_LIMIT    (S_CODE_START + S_CODE_SIZE - 1)

/* Secure Data stored in VM0 */
#define S_DATA_START    (VM0_BASE_S + 0x500000)
#define S_DATA_SIZE     (0x100000)
#define S_DATA_LIMIT    (S_DATA_START + S_DATA_SIZE - 1)

/* Size of vector table: 111 interrupt handlers + 4 bytes MSP initial value */
#define S_CODE_VECTOR_TABLE_SIZE    (0x1C0)

/* Non-secure regions */
/* Non-Secure Code executes from VM1 */
#define NS_CODE_START   (VM1_BASE_NS + SECURE_IMAGE_MAX_SIZE + BL2_HEADER_SIZE)
#define NS_CODE_SIZE    (IMAGE_NS_CODE_SIZE)
#define NS_CODE_LIMIT   (NS_CODE_START + NS_CODE_SIZE - 1)

/* Non-Secure Data stored in VM0 */
#define NS_DATA_START   (VM0_BASE_NS + 0x600000)
#define NS_DATA_SIZE    (0x100000)
#define NS_DATA_LIMIT   (NS_DATA_START + NS_DATA_SIZE - 1)

/* NS partition information is used for MPC and SAU configuration */
#define NS_PARTITION_START \
            (VM1_BASE_NS + SECURE_IMAGE_MAX_SIZE)
#define NS_PARTITION_SIZE (FLASH_NS_PARTITION_SIZE)

/* Secondary partition for new images in case of firmware upgrade */
#define SECONDARY_PARTITION_START \
            (VM0_BASE_NS + S_IMAGE_SECONDARY_PARTITION_OFFSET)
#define SECONDARY_PARTITION_SIZE (FLASH_S_PARTITION_SIZE + \
                                  FLASH_NS_PARTITION_SIZE)

/* Bootloader regions */
/* BL1_1 is XIP from ROM */
#define BL1_1_CODE_START  (ROM_BASE_S)
#define BL1_1_CODE_SIZE   (0xE000) /* 56 KB */
#define BL1_1_CODE_LIMIT  (BL1_1_CODE_START + BL1_1_CODE_SIZE - 1)

#define PROVISIONING_DATA_START (BL1_1_CODE_START + BL1_1_CODE_SIZE)
#define PROVISIONING_DATA_SIZE  (0x2400) /* 9 KB */
#define PROVISIONING_DATA_LIMIT (PROVISIONING_DATA_START + PROVISIONING_DATA_SIZE - 1)

/* BL1_2 executes from VM1 */
#define BL1_2_CODE_START  (NS_IMAGE_LOAD_ADDRESS + NON_SECURE_IMAGE_MAX_SIZE)
#define BL1_2_CODE_SIZE   (0x2000) /* 8 KB */
#define BL1_2_CODE_LIMIT  (BL1_2_CODE_START + BL1_2_CODE_SIZE - 1)

/* BL2 executes from VM1, and is positioned directly after BL1_2 */
#define BL2_IMAGE_START   (BL1_2_CODE_START + BL1_2_CODE_SIZE)
#define BL2_CODE_START    (BL2_IMAGE_START + BL1_HEADER_SIZE)
#define BL2_CODE_SIZE     (IMAGE_BL2_CODE_SIZE)
#define BL2_CODE_LIMIT    (BL2_CODE_START + BL2_CODE_SIZE - 1)

/* Bootloader uses same memory as for secure image */
#define BL1_1_DATA_START  (S_DATA_START)
#define BL1_1_DATA_SIZE   (0x4000) /* 16 KB */
#define BL1_1_DATA_LIMIT  (BL1_1_DATA_START + BL1_1_DATA_SIZE - 1)

#define BL1_2_DATA_START  (BL1_1_DATA_START + BL1_1_DATA_SIZE)
#define BL1_2_DATA_SIZE   (0x4000) /* 16 KB */
#define BL1_2_DATA_LIMIT  (BL1_2_DATA_START + BL1_2_DATA_SIZE - 1)

#define BL2_DATA_START    (S_DATA_START)
#define BL2_DATA_SIZE     (0x8000) /* 32 KB */
#define BL2_DATA_LIMIT    (BL2_DATA_START + BL2_DATA_SIZE - 1)

/* Shared data area between bootloader and runtime firmware.
 * Shared data area is allocated at the beginning of the RAM, it is overlapping
 * with TF-M Secure code's MSP stack
 */
#define BOOT_TFM_SHARED_DATA_BASE S_DATA_START
#define BOOT_TFM_SHARED_DATA_SIZE (0x400)
#define BOOT_TFM_SHARED_DATA_LIMIT (BOOT_TFM_SHARED_DATA_BASE + \
                                    BOOT_TFM_SHARED_DATA_SIZE - 1)

/* AP to RSS MHU receiver interrupt */
#define MAILBOX_IRQ CMU_MHU0_Receiver_IRQn

#endif /* __REGION_DEFS_H__ */
