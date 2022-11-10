/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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

/**
 * \file host_base_address.h
 * \brief This file defines the host memory map addresses accessed by RSS.
 */

#ifndef __HOST_BASE_ADDRESS_H__
#define __HOST_BASE_ADDRESS_H__

/* Host addresses */
#define AP_BL1_SRAM_BASE   0x0UL        /* AP initial boot SRAM base address */
#define AP_BL1_SIZE        0x20000U     /* 128KB */

#define SCP_BL1_SRAM_BASE  0x40000000UL /* SCP initial boot SRAM base address */
#define SCP_BL1_SIZE       0x10000U     /* 64KB */

#define HOST_UART_BASE     0x2A400000UL /* Host UART base address */
#define HOST_UART_SIZE     0x2000U      /* 8KB */

/* In future HOST_FLASH0_BASE should be set to the _actual_ base of the flash,
 * and we should implement some logic to locate the images (in a FIP, probably).
 * For now, define the base address at an offset large enough that it's not used
 * by the AP firmware images.
 */
/* #define HOST_FLASH_BASE   0x08000000UL /1* Host flash base address *1/ */
#define HOST_FLASH0_BASE   0x08400000UL /* Host flash base address */
#define HOST_FLASH0_SIZE   0xFC00000    /* 252 MiB */

#endif  /* __HOST_BASE_ADDRESS_H__ */
