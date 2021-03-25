/*
 * Copyright (c) 2018-2021 Arm Limited. All rights reserved.
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

#ifndef __CMSIS_DRIVER_CONFIG_H__
#define __CMSIS_DRIVER_CONFIG_H__

#include "device_cfg.h"
#include "device_definition.h"
#include "platform_description.h"
#include "RTE_Device.h"
#include "target_cfg.h"

#define UART0_DEV           UART0_PL011_DEV_NS
#define UART1_DEV           UART1_PL011_DEV_NS

#define MPC_ISRAM0_DEV      MPC_ISRAM0_DEV_S
#define MPC_ISRAM1_DEV      MPC_ISRAM1_DEV_S
#define MPC_ISRAM2_DEV      MPC_ISRAM2_DEV_S
#define MPC_ISRAM3_DEV      MPC_ISRAM3_DEV_S
#define MPC_CODE_SRAM_DEV   MPC_CODE_SRAM_DEV_S
#define MPC_MRAM_DEV        MPC_MRAM_DEV_S

#define AHB_PPC0_DEV        AHB_PPC0_DEV_S
#define AHB_PPCEXP0_DEV     AHB_PPCEXP0_DEV_S
#define APB_PPC0_DEV        APB_PPC0_DEV_S
#define APB_PPC1_DEV        APB_PPC1_DEV_S
#define APB_PPCEXP0_DEV     APB_PPCEXP0_DEV_S
#define APB_PPCEXP1_DEV     APB_PPCEXP1_DEV_S

#define MUSCA_S1_SCC_DEV    MUSCA_S1_SCC_DEV_S
#define SSE_200_CACHE_DEV   SSE_200_CACHE_DEV_S

#define QSPI_FLASH0_DEV     MT25QL_DEV_S

#endif  /* __CMSIS_DRIVER_CONFIG_H__ */
