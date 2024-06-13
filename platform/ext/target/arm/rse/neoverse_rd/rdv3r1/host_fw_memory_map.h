/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
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

/* SCP shared memory for SCMI transport */
#define SCP_SHARED_MEMORY_PHYS_BASE         HOST_RSM_SRAM_PHYS_BASE

#endif /* __HOST_FW_MEMORY_MAP_H__ */
