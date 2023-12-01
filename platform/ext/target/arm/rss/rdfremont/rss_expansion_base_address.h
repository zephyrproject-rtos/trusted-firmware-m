/*
 * Copyright (c) 2023 Arm Limited
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file  platform_base_address.h
 * \brief This file defines all the peripheral base addresses for RSS platform.
 */

#ifndef __RSS_EXPANSION_BASE_ADDRESS_H__
#define __RSS_EXPANSION_BASE_ADDRESS_H__

/* RSS Integration Layer register block */
#define RSS_INTEG_LAYER_BASE_S  0x58100000
/* UART 0 Secure base address */
#define UART0_BASE_S            0x58101000
/* Boot flash */
#define BOOT_FLASH_BASE_S       0xB0000000

#endif  /* __RSS_EXPANSION_BASE_ADDRESS_H__ */
