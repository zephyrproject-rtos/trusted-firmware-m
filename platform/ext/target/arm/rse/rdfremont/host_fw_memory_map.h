/*
 * Copyright (c) 2024 Arm Limited. All rights reserved.
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
 * \file host_fw_memory_map.h
 * \brief This file contains firmware defined base addresses.
 */

#ifndef __HOST_FW_MEMORY_MAP_H__
#define __HOST_FW_MEMORY_MAP_H__

#include "host_css_memory_map.h"

/* AP SRAM Read-only base address */
#define HOST_AP_BL1_RO_SRAM_PHYS_BASE       HOST_AP_SHARED_SRAM_PHYS_BASE
/* AP SRAM Read-only size */
#define HOST_AP_BL1_RO_SRAM_PHYS_SIZE       (0x00019000ULL)
/* AP SRAM Read-only end address */
#define HOST_AP_BL1_RO_SRAM_PHYS_LIMIT      (HOST_AP_BL1_RO_SRAM_PHYS_BASE + \
                                             HOST_AP_BL1_RO_SRAM_PHYS_SIZE - 1)

#if (HOST_AP_BL1_RO_SRAM_PHYS_LIMIT >= HOST_AP_SHARED_SRAM_PHYS_LIMIT)
#error "AP_BL1_SRAM: Out of memory!"
#endif

/* AP SRAM Read-write base address */
#define HOST_AP_BL1_RW_SRAM_PHYS_BASE       (HOST_AP_BL1_RO_SRAM_PHYS_LIMIT + \
                                             1)
/* AP SRAM Read-write end address */
#define HOST_AP_BL1_RW_SRAM_PHYS_LIMIT      HOST_AP_SHARED_SRAM_PHYS_LIMIT

#endif /* __HOST_FW_MEMORY_MAP_H__ */
