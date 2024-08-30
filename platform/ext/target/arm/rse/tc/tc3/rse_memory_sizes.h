/*
 * Copyright (c) 2022-2024, Arm Limited. All rights reserved.
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

#ifndef __RSE_MEMORY_SIZES_H__
#define __RSE_MEMORY_SIZES_H__

#ifdef RSE_XIP
#define VM0_SIZE                         0x00010000 /* 64 KiB */
#define VM1_SIZE                         0x00010000 /* 64 KiB */
#else
#define VM0_SIZE                         0x00080000 /* 512 KiB */
#define VM1_SIZE                         0x00080000 /* 512 KiB */
#endif /* RSE_XIP */

/*
 * The size of the SRAM which is allocated to NS. This may be increased
 * depending on the layout.
 */
#define NS_DATA_SIZE (0x5000)

/* The total size of the OTP for the RSE */
#define OTP_TOTAL_SIZE     (0x4000)
/* How much OTP is reserved for the portion of the DMA Initial Command Sequence
 * which is located in OTP. This is loaded by directly by the DMA hardware, so
 * this must match the size configured into the ROM part of the ICS.
 */
#define OTP_DMA_ICS_SIZE   (0x400)
/* How much space in ROM is used for the DMA Initial Command Sequence */
#define ROM_DMA_ICS_SIZE   (0x1000)

/* How much space in OTP can be used for the SAM configuration */
#define OTP_SAM_CONFIGURATION_SIZE 0x60

/* How much space in OTP can be used for the manufacturing data */
#define OTP_MANUFACTURING_DATA_MAX_SIZE 0x1000

/* The maximum size for code in the provisioning bundle */
#define PROVISIONING_BUNDLE_CODE_SIZE   (0xB000)
/* The maximum size for secret values in the provisioning bundle */
#define PROVISIONING_BUNDLE_VALUES_SIZE (0x3800)

#endif /* __RSE_MEMORY_SIZES_H__ */
