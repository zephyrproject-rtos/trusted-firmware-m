/*
 * Copyright (c) 2023 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Driver_Flash_SST26VF064B.h"
#include "RTE_Device.h"
#include "flash_device_definition.h"

#include "cmsis.h"

#if (RTE_FLASH0)

static struct arm_flash_sst26vf064b_flash_dev_t ARM_FLASH0_DEV = {
    .dev    = &SPI_SST26VF064B_DEV,
    .data   = &SST26VF064B_DEV_DATA,
    .setup_qspi = NULL,
    .release_qspi = NULL,
    .memory_base_s = QSPI_SRAM_BASE_S,
    .memory_base_ns = QSPI_SRAM_BASE_NS,
};

ARM_FLASH_SST26VF064B(ARM_FLASH0_DEV, Driver_FLASH0);

#endif /* RTE_FLASH0 */
