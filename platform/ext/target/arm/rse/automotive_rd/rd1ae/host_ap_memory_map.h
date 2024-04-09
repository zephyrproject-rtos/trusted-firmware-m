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
 * \file host_ap_memory_map.h
 * \brief This file contains addresses that are defined in the Application
 *        processor (AP) memory map.
 */

#ifndef __HOST_AP_MEMORY_MAP_H__
#define __HOST_AP_MEMORY_MAP_H__

/* AP Shared (ARSM) SRAM base address */
#define HOST_AP_SHARED_SRAM_PHYS_BASE               0x000000000ULL
/* AP Shared (ARSM) SRAM end address */
#define HOST_AP_SHARED_SRAM_PHYS_LIMIT              0x0000FFFFFULL

#endif /* __HOST_AP_MEMORY_MAP_H__ */
