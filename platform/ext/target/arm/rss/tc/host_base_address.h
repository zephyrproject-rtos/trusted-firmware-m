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

#endif  /* __HOST_BASE_ADDRESS_H__ */
