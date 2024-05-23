/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
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
/* SCP INIT CTRL region physical address start */
#define HOST_SCP_INIT_CTRL_PHYS_BASE                0x1000050050000ULL

/* MCP region base address */
#define HOST_MCP_PHYS_BASE                          0x2000000000000ULL
/* MCP region end address */
#define HOST_MCP_PHYS_LIMIT                         0x20000FFFFFFFFULL
/* MCP INIT CTRL region physical address start */
#define HOST_MCP_INIT_CTRL_PHYS_BASE                0x2000050020000ULL

#endif /* __HOST_MSCP_MEMORY_MAP_H__ */
