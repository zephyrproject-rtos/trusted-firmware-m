/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * \file host_css_io_block_memory_map.h
 * \brief This file contains addresses that are defined in the compute
 *        subsystem (CSS) memory map for IO Block.
 */

#ifndef __HOST_CSS_IO_BLOCK_MEMORY_MAP_H__
#define __HOST_CSS_IO_BLOCK_MEMORY_MAP_H__

/* IO Block memory region offset value  */
#define HOST_IO_BLOCK_COMP_OFFSET           0x8000000ULL

/* IO Block memory region start */
#define HOST_IO_BLOCK_0_BASE                0x1C0000000ULL

/* IO Block memory region start for IO block n */
#define HOST_IO_BLOCK_N_BASE(n)             (HOST_IO_BLOCK_0_BASE + \
                                             (HOST_IO_BLOCK_COMP_OFFSET * (n)))

/* Base address for TCU */
#define HOST_IO_TCU_PHYS_BASE(n)                (HOST_IO_BLOCK_N_BASE(n))
/* End address for TCU */
#define HOST_IO_TCU_PHYS_LIMIT(n)               (0x3FFFFFFULL +     \
                                                 HOST_IO_BLOCK_N_BASE(n))
/* Base address for NCI GPV Space */
#define HOST_IO_NCI_GPV_PHYS_BASE(n)            (0x4000000ULL +     \
                                                 HOST_IO_BLOCK_N_BASE(n))
/* End address for NCI GPV Space */
#define HOST_IO_NCI_GPV_PHYS_LIMIT(n)           (0x4FFFFFFULL +     \
                                                 HOST_IO_BLOCK_N_BASE(n))
/* Base address for IO Integration Control registers */
#define HOST_IO_INTEGRATION_CTRL_PHYS_BASE(n)   (0x5B00000ULL +     \
                                                 HOST_IO_BLOCK_N_BASE(n))
/* End address for IO Integration Control registers */
#define HOST_IO_INTEGRATION_CTRL_PHYS_LIMIT(n)  (0x5B0FFFFULL +     \
                                                 HOST_IO_BLOCK_N_BASE(n))
/* Base address for IO Integration Control registers */
#define HOST_IO_RAS_AGENT_PHYS_BASE(n)          (0x5B10000ULL + \
                                                 HOST_IO_BLOCK_N_BASE(n))
/* End address for IO Integration Control registers */
#define HOST_IO_RAS_AGENT_PHYS_LIMIT(n)         (0x5B1FFFFULL + \
                                                 HOST_IO_BLOCK_N_BASE(n))
/* Base address for PHY Expansion interface */
#define HOST_IO_EXP_INTERFACE_PHYS_BASE(n)      (0x5C00000ULL +     \
                                                 HOST_IO_BLOCK_N_BASE(n))
/* End address for PHY Expansion interface */
#define HOST_IO_EXP_INTERFACE_PHYS_LIMIT(n)     (0x5FFFFFFULL +     \
                                                 HOST_IO_BLOCK_N_BASE(n))
/* Base address for PCIe Controller Expansion interface */
#define HOST_IO_PCIE_CTRL_EXP_PHYS_BASE(n)      (0x6000000ULL +     \
                                                 HOST_IO_BLOCK_N_BASE(n))
/* End address for PCIe Controller Expansion interface */
#define HOST_IO_PCIE_CTRL_EXP_PHYS_LIMIT(n)     (0x7FFFFFFULL +     \
                                                 HOST_IO_BLOCK_N_BASE(n))

#endif /* __HOST_CSS_IO_BLOCK_MEMORY_MAP_H__ */
