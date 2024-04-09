/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "bootutil/bootutil_log.h"
#include "host_device_definition.h"
#include "host_base_address.h"
#include "ni_tower_lib.h"

#include <stddef.h>

/* System Control NI-Tower component nodes */
const struct ni_tower_component_node sysctrl_rse_main_asni  = {
    .type = NI_TOWER_ASNI,
    .id = SYSCTRL_RSE_MAIN_ASNI_ID,
};

/*
 * System Control NI-Tower is the interconnect between the AXI interfaces of
 * RSE, SCP, Safety Island and the CMN interconnect. Following block diagram
 * depicts an abstract overview of the routing map for these interfaces.
 *
 *                             +-----+
 *                             |     |
 *                             |     |---------------------------> tcu_apbm
 *                             |     |
 *                             |     |
 *                             |     |
 *                             |     |---------------------------> rse_scp_axim
 *                             |     |
 *                             |     |
 * rse_main_axis ------------->|     |
 *                             |     |---------------------------> rse_si_axim
 *                             |     |
 *                             |     |
 *                             |     |       +-----+
 *                             |     |------>|     |
 *                             |     |       |     |-------------> cmn_apbm
 *                             +-----+       |     |
 *                                           |     |
 *                                           |     |-------------> rsm_axim
 *                                           |     |
 *                                           |     |
 *                                           |     |
 *                                           |     |-------------> rsm_apbm
 *                                           |     |
 *                                           |     |
 *                                           |     |
 *                                           |     |-------------> app_axim
 *                                           |     |
 *                                           +-----+
 *
 *
 * The following matrix shows the connections within System Control NI-Tower.
 *
 * +------------+---------------+
 * |            | rse_main_axis |
 * +============+===============+
 * |rse_scp_axim|       X       |
 * +------------+---------------+
 * |rse_si_axim |       X       |
 * +------------+---------------+
 * |  rsm_axim  |       X       |
 * +------------+---------------+
 * |  rsm_apbm  |       X       |
 * +------------+---------------+
 * |  cmn_apbm  |       X       |
 * +------------+---------------+
 * |  tcu_apbm  |       X       |
 * +------------+---------------+
 * |  app_axim  |       X       |
 * +------------+---------------+
 * |app_scp_axim|               |
 * +------------+---------------+
 * |app_si_axim |               |
 * +------------+---------------+
 *  NOTE: 'X' means there is a connection.
 */

/*
 * Request originating from RSE ATU is mapped to targets based on following
 * address map.
 */
static const struct ni_tower_psam_reg_cfg_info rse_main_axis_psam[] = {
    /* Shared SRAM + AP Memory Expansion 1 */
    {
        HOST_AP_SHARED_SRAM_PHYS_BASE,
        HOST_AP_MEM_EXP_1_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* System Control Block NI-Tower */
    {
        HOST_SYSCTRL_NI_TOWER_PHYS_BASE,
        HOST_SYSCTRL_NI_TOWER_PHYS_LIMIT,
        SYSCTRL_CONFIG_SPACE_ID
    },
    /*
     * IO Block NI-Tower + Peripheral Block NI-Tower + System Fabric
     * NI-Tower + Debug Block NCI GPV + UART Peripherals + Generic refclk +
     * AP Watchdog peripherals + SID + ECC error record registers
     */
    {
        HOST_IO_MACRO_NI_TOWER_PHYS_BASE,
        HOST_RSE_RL_ARSM_RAM_ECC_REC_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* ECC error record for RSM block */
    {
        HOST_RSE_S_RSM_RAM_ECC_REC_PHYS_BASE,
        HOST_RSE_NS_RSM_RAM_ECC_REC_PHYS_LIMIT,
        SYSCTRL_RSM_PMNI_ID
    },
    /* Generic refclk registers + AP<->SCP MHUv3 registers */
    {
        HOST_GENERIC_REFCLK_CNTREAD_PHYS_BASE,
        HOST_AP_RT_SCP_MHUV3_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* AP<->RSE MHUv3 registers */
    {
        HOST_AP_NS_RSE_MHUV3_PHYS_BASE,
        HOST_AP_RL_RSE_MHUV3_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* Timer synchronization registers */
    {
        HOST_SYNCNT_MSTUPDTVAL_ADDR_PHYS_BASE,
        HOST_SYNCNT_MSTUPDTVAL_ADDR_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* Shared RSM SRAM */
    {
        HOST_RSM_SRAM_PHYS_BASE,
        HOST_RSM_SRAM_PHYS_LIMIT,
        SYSCTRL_RSM_AMNI_ID
    },
    /* GIC-720AE */
    {
        HOST_GIC_720AE_PHYS_BASE,
        HOST_GIC_720AE_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* PCIe NCI Memory Space 1 + DRAM */
    {
        HOST_PCIE_NCI_1_PHYS_BASE,
        HOST_DRAM_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* CMN GPV */
    {
        HOST_CMN_GPV_PHYS_BASE,
        HOST_CMN_GPV_PHYS_LIMIT,
        SYSCTRL_CMN_PMNI_ID
    },
    /* Memory Controller + MPE register space */
    {
        HOST_MPE_PHYS_BASE,
        HOST_MPE_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* Cluster Utility memory region */
    {
        HOST_CLUST_UTIL_PHYS_BASE,
        HOST_CLUST_UTIL_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* IO Block SMMU + NCI GPV + PCIe CTRL + PHY */
    {
        HOST_IO_BLOCK_PHYS_BASE,
        HOST_IO_BLOCK_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* SYSCTRL SMMU */
    {
        HOST_SYSCTRL_SMMU_PHYS_BASE,
        HOST_SYSCTRL_SMMU_PHYS_LIMIT,
        SYSCTRL_TCU_PMNI_ID
    },
    /* Debug Memory Map + AP Memory Expansion 2 */
    {
        HOST_DEBUG_MMAP_PHYS_BASE,
        HOST_AP_MEM_EXP_2_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /*
     * PCIe NCI Memory space 2 + PCIe NCI Memory space 3 + DRAM +
     * AP Memory Expansion 3
     */
    {
        HOST_PCIE_NCI_2_PHYS_BASE,
        HOST_AP_MEM_EXP_3_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* SCP Access region */
    {
        HOST_SCP_PHYS_BASE,
        HOST_SCP_PHYS_LIMIT,
        SYSCTRL_RSE_SCP_AMNI_ID
    },
    /* SI Access region */
    {
        HOST_SI_PHYS_BASE,
        HOST_SI_PHYS_LIMIT,
        SYSCTRL_RSE_SI_AMNI_ID
    }
};

/*
 * Configure Programmable System Address Map (PSAM) to setup the memory map and
 * its target ID for each requester in the System Control NI-Tower for nodes
 * under AON domain.
 */
static int32_t program_sysctrl_psam_aon(void)
{
    enum ni_tower_err err;

    /* Populates all address maps into a table array to enable desired PSAMs */
    struct ni_tower_psam_cfgs psam_table[] = {
        {
            .component = &sysctrl_rse_main_asni,
            .nh_region_count = ARRAY_SIZE(rse_main_axis_psam),
            .regions = rse_main_axis_psam,
            .add_chip_addr_offset = false,
        },
    };

    err = ni_tower_program_psam_table(&SYSCTRL_NI_TOWER_DEV, psam_table,
            ARRAY_SIZE(psam_table));
    if (err != NI_TOWER_SUCCESS) {
        return -1;
    }

    return 0;
}
