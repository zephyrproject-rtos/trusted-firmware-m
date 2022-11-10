/*
 * Copyright (c) 2019-2023 Arm Limited. All rights reserved.
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

/* RSS memory layout is as follows during BL1
 *     |---------------------------------------------------------
 * VM0 | BOOT_SHARED  | BL1_2_CODE | BL1_1_DATA  | BL1_2_DATA   |
 *     |---------------------------------------------------------
 *     |---------------------------------------------------------
 * VM1 | BL2_CODE                                               |
 *     |---------------------------------------------------------
 *
 * If the size of VM0 and VM1 are larger than 64KiB, the size of BL1 code/data
 * and BL2 code can be increased to fill the extra space.
 *
 * RSS memory layout is as follows during BL2
 *     |---------------------------------------------------------
 * VM0 | BOOT_SHARED  | ?XIP tables | BL2_DATA                  |
 *     |---------------------------------------------------------
 *     |---------------------------------------------------------
 * VM1 | BL2_CODE                                               |
 *     |---------------------------------------------------------
 *
 * If the size of VM0 and VM1 are larger than 64KiB, the size of BL2 code and
 * data can be increased to fill the extra space.
 *
 * RSS memory layout is as follows during Runtime with XIP mode enabled
 *     |---------------------------------------------------------
 * VM0 | BOOT_SHARED+S_DATA                                     |
 *     |---------------------------------------------------------
 *     |---------------------------------------------------------
 * VM1 | S_DATA                     | NS_DATA                   |
 *     |---------------------------------------------------------
 *
 * RSS memory layout is as follows during Runtime with XIP mode disabled. Note
 * that each SRAM must be at least 512KiB in this mode (64KiB data and 384KiB
 * code, for each of secure and non-secure).
 *     |-----------------------------------------------------------------------|
 * VM0 | BOOT_SHARED+S_DATA | S_CODE                                           |
 *     |-----------------------------------------------------------------------|
 *     |-----------------------------------------------------------------------|
 * VM1 | NS_DATA            | NS_CODE                                          |
 *     |-----------------------------------------------------------------------|
 */

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
/* Secure Code executes from VM0, or XIP from flash via the SIC */
#ifdef RSS_XIP
#define S_CODE_START    (RSS_RUNTIME_S_XIP_BASE_S)
#define S_CODE_SIZE     (FLASH_S_PARTITION_SIZE)
#else
#define S_CODE_START    (S_IMAGE_LOAD_ADDRESS + BL2_HEADER_SIZE)
#define S_CODE_SIZE     (IMAGE_S_CODE_SIZE)
#endif /* RSS_XIP */
#define S_CODE_LIMIT    (S_CODE_START + S_CODE_SIZE - 1)

/* Secure Data stored in VM0. Size defined in flash layout */
#define S_DATA_START    (VM0_BASE_S)
#define S_DATA_LIMIT    (S_DATA_START + S_DATA_SIZE - 1)

/* Size of vector table: 111 interrupt handlers + 4 bytes MSP initial value */
#define S_CODE_VECTOR_TABLE_SIZE    (0x1C0)

/* Non-secure regions */
/* Non-Secure Code executes from VM1, or XIP from flash via the SIC */
#ifdef RSS_XIP
#define NS_CODE_START   (RSS_RUNTIME_NS_XIP_BASE_NS)
#define NS_CODE_SIZE    (FLASH_NS_PARTITION_SIZE)
#else
#define NS_CODE_START   (VM1_BASE_NS + NS_DATA_SIZE + BL2_HEADER_SIZE)
#define NS_CODE_SIZE    (IMAGE_NS_CODE_SIZE)
#endif /* RSS_XIP */
#define NS_CODE_LIMIT   (NS_CODE_START + NS_CODE_SIZE - 1)

/* Non-Secure Data stored after secure data, or in VM1. */
#ifdef RSS_XIP
#define NS_DATA_START   (VM0_BASE_NS + S_DATA_SIZE)
#else
#define NS_DATA_START   (VM1_BASE_NS)
#endif
#define NS_DATA_LIMIT   (NS_DATA_START + NS_DATA_SIZE - 1)

/* NS partition information is used for MPC and SAU configuration */
#ifdef RSS_XIP
#define NS_PARTITION_START RSS_RUNTIME_NS_XIP_BASE_NS
#else
#define NS_PARTITION_START (NS_CODE_START)
#endif /* RSS_XIP */
#define NS_PARTITION_SIZE (NS_CODE_SIZE)

#define SECONDARY_PARTITION_START (FWU_HOST_IMAGE_BASE_S)
#define SECONDARY_PARTITION_SIZE (0x100000)

/* Bootloader regions */
/* BL1_1 is XIP from ROM */
#define BL1_1_CODE_START  (ROM_BASE_S)
#define BL1_1_CODE_SIZE   (0xE000) /* 56 KB */
#define BL1_1_CODE_LIMIT  (BL1_1_CODE_START + BL1_1_CODE_SIZE - 1)

#define PROVISIONING_DATA_START (BL1_1_CODE_START + BL1_1_CODE_SIZE)
#define PROVISIONING_DATA_SIZE  (0x2400) /* 9 KB */
#define PROVISIONING_DATA_LIMIT (PROVISIONING_DATA_START + PROVISIONING_DATA_SIZE - 1)

/* BL1_2 executes from VM0, leaving space for the shared data */
#define BL1_2_CODE_START  (VM0_BASE_S + BOOT_TFM_SHARED_DATA_SIZE)
#define BL1_2_CODE_SIZE   (0x2000) /* 8 KiB */
#define BL1_2_CODE_LIMIT  (BL1_2_CODE_START + BL1_2_CODE_SIZE - 1)

/* BL2 executes from VM1 */
#define BL2_IMAGE_START   (VM1_BASE_S)
#define BL2_CODE_START    (BL2_IMAGE_START + BL1_HEADER_SIZE)
#define BL2_CODE_SIZE     (IMAGE_BL2_CODE_SIZE)
#define BL2_CODE_LIMIT    (BL2_CODE_START + BL2_CODE_SIZE - 1)

/* BL1 data is in VM0 */
#define BL1_1_DATA_START  (BL1_2_CODE_START + BL1_2_CODE_SIZE)
#define BL1_1_DATA_SIZE   (0x4000) /* 16 KB */
#define BL1_1_DATA_LIMIT  (BL1_1_DATA_START + BL1_1_DATA_SIZE - 1)

#define BL1_2_DATA_START  (BL1_1_DATA_START + BL1_1_DATA_SIZE)
#define BL1_2_DATA_SIZE   (0x4000) /* 16 KB */
#define BL1_2_DATA_LIMIT  (BL1_2_DATA_START + BL1_2_DATA_SIZE - 1)

/* XIP data go after the reserved space for boot_data */
#define BL2_XIP_TABLES_START (VM0_BASE_S + BOOT_TFM_SHARED_DATA_SIZE)
#define BL2_XIP_TABLES_SIZE  (0x5000) /* 20 KiB */
#define BL2_XIP_TABLES_LIMIT (BL2_XIP_TABLES_START + BL2_XIP_TABLES_SIZE - 1)

/* BL2 data is in VM0, in the same space as BL1 data */
#define BL2_DATA_START    (BL2_XIP_TABLES_START + BL2_XIP_TABLES_SIZE)
#define BL2_DATA_SIZE     (0x8000) /* 32 KB */
#define BL2_DATA_LIMIT    (BL2_DATA_START + BL2_DATA_SIZE - 1)

/* Store boot data is in the start of VM0. It overlaps with the secure data
 * region
 */
#define BOOT_TFM_SHARED_DATA_BASE VM0_BASE_S
#define BOOT_TFM_SHARED_DATA_SIZE (0x400)
#define BOOT_TFM_SHARED_DATA_LIMIT (BOOT_TFM_SHARED_DATA_BASE + \
                                    BOOT_TFM_SHARED_DATA_SIZE - 1)

/* AP to RSS MHU receiver interrupt */
#define MAILBOX_IRQ CMU_MHU0_Receiver_IRQn

#endif /* __REGION_DEFS_H__ */
