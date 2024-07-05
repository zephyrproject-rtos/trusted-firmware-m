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
 * \file host_mscp_memory_map.h
 * \brief This file contains addresses that are defined in the compute
 *        subsystem (CSS) MSCP address space.
 */

#ifndef __HOST_MSCP_MEMORY_MAP_H__
#define __HOST_MSCP_MEMORY_MAP_H__

/*
 * CSS system MSCP memory spaces
 * -----------------------------
 *
 * RSE ATU is used for accessing MSCP physical address which is mapped at the
 * following system physical address spaces.
 *
 *  - SCP address space: 0x1_0000_0000_0000 to 0x1_0000_FFFF_FFFF
 *  - MCP address space: 0x2_0000_0000_0000 to 0x2_0000_FFFF_FFFF
 */

/* SCP region base address */
#define HOST_SCP_PHYS_BASE                          0x1000000000000ULL
/* SCP region end address */
#define HOST_SCP_PHYS_LIMIT                         0x10000FFFFFFFFULL

/* MCP region base address */
#define HOST_MCP_PHYS_BASE                          0x2000000000000ULL
/* MCP region end address */
#define HOST_MCP_PHYS_LIMIT                         0x20000FFFFFFFFULL

#endif /* __HOST_MSCP_MEMORY_MAP_H__ */
