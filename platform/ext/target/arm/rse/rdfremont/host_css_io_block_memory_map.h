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
 * \file host_css_io_block_memory_map.h
 * \brief This file contains addresses that are defined in the compute
 *        subsystem (CSS) memory map for IO Block.
 */

#ifndef __HOST_CSS_IO_BLOCK_MEMORY_MAP_H__
#define __HOST_CSS_IO_BLOCK_MEMORY_MAP_H__

/* IO Block memory region offset value  */
#define HOST_IO_BLOCK_COMP_OFFSET           0x8000000ULL
/* Base address for TCU */
#define HOST_IO_TCU_PHYS_BASE(n)            (0x280000000ULL +     \
                                             (HOST_IO_BLOCK_COMP_OFFSET * (n)))
/* End address for TCU */
#define HOST_IO_TCU_PHYS_LIMIT(n)           (0x283FFFFFFULL +     \
                                             (HOST_IO_BLOCK_COMP_OFFSET * (n)))
/* Base address for NCI GPV Space */
#define HOST_IO_NCI_GPV_PHYS_BASE(n)        (0x284000000ULL +     \
                                             (HOST_IO_BLOCK_COMP_OFFSET * (n)))
/* End address for NCI GPV Space */
#define HOST_IO_NCI_GPV_PHYS_LIMIT(n)       (0x284FFFFFFULL +     \
                                             (HOST_IO_BLOCK_COMP_OFFSET * (n)))
/* Base address for IO Integration Control registers */
#define HOST_IO_INTEGRATION_CTRL_PHYS_BASE(n)   (0x285B00000ULL + \
                                             (HOST_IO_BLOCK_COMP_OFFSET * (n)))
/* End address for IO Integration Control registers */
#define HOST_IO_INTEGRATION_CTRL_PHYS_LIMIT(n)  (0x285B0FFFFULL + \
                                             (HOST_IO_BLOCK_COMP_OFFSET * (n)))
/* Base address for PHY Expansion interface */
#define HOST_IO_EXP_INTERFACE_PHYS_BASE(n)  (0x285C00000ULL +     \
                                             (HOST_IO_BLOCK_COMP_OFFSET * (n)))
/* End address for PHY Expansion interface */
#define HOST_IO_EXP_INTERFACE_PHYS_LIMIT(n) (0x285FFFFFFULL +     \
                                             (HOST_IO_BLOCK_COMP_OFFSET * (n)))
/* Base address for PCIe Controller Expansion interface */
#define HOST_IO_PCIE_CTRL_EXP_PHYS_BASE(n)  (0x286000000ULL +     \
                                             (HOST_IO_BLOCK_COMP_OFFSET * (n)))
/* End address for PCIe Controller Expansion interface */
#define HOST_IO_PCIE_CTRL_EXP_PHYS_LIMIT(n) (0x287FFFFFFULL +     \
                                             (HOST_IO_BLOCK_COMP_OFFSET * (n)))

#endif /* __HOST_CSS_IO_BLOCK_MEMORY_MAP_H__ */
