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

/*
 * System Control NI-Tower is the interconnect between the AXI interfaces of
 * RSE, SCP/MCP and the CMN interconnect. Following block diagram depicts an
 * abstract overview of the routing map for these interfaces.
 *
 *                             +-----+
 *                             |     |
 *                             |     |---------------------------> tcu_apbm
 *                             |     |
 *                             |     |
 *                             |     |                 +-----+
 *                             |     |-----------------| APU |---> rse_scp_axim
 *                             |     |                 +-----+
 *                             |     |
 * rse_main_axis ------------->|     |                 +-----+
 *                             |     |-----------------| APU |---> rse_mcp_axim
 *                             |     |                 +-----+
 *                             |     |
 *                             |     |       +-----+
 *                             |     |------>|     |
 *                             |     |       |     |-------------> cmn_apbm
 *                             +-----+       |     |
 *                                           |     |   +-----+
 *                                           |     |---| APU |---> rsm_axim
 *      scp_axis --------------------------->|     |   +-----+
 *                                           |     |
 *                                           |     |   +-----+
 *                                           |     |---| APU |---> rsm_apbm
 *                             +-----+       |     |   +-----+
 *                             |     |       |     |
 *                             |     |------>|     |
 *                             |     |       |     |-------------> app_axim
 *                             |     |       |     |
 *                  +-----+    |     |       +-----+
 *      mcp_axis ---| APU |--->|     |
 *                  +-----+    |     |
 *                             |     |       +-----+
 *                             |     |       |     |
 *                             |     |------>|     |
 *                             |     |       |     |
 *                             +-----+       |     |
 *                                           |     |
 *                                           |     |-------------> app_scp_axim
 *                                           |     |
 *                                           |     |
 *                                           |     |
 *                                           |     |
 *                                           |     |
 *                                           +-----+
 *
 *                             +-----+
 *                             |     |
 *                             |     |
 *  rse_scp_axis ------------->|     |--------------------------> lcp_axim
 *                             |     |
 *                             |     |
 *                             +-----+
 *
 * The following matrix shows the connections within System Control NI-Tower.
 *
 * +------------+---------------+----------+----------+--------------+
 * |            | rse_main_axis | scp_axis | mcp_axis | rse_scp_axis |
 * +============+===============+==========+==========+==============+
 * |rse_scp_axim|       X       |          |          |              |
 * +------------+---------------+----------+----------+--------------+
 * |rse_mcp_axim|       X       |          |          |              |
 * +------------+---------------+----------+----------+--------------+
 * |  rsm_axim  |       X       |    X     |    X     |              |
 * +------------+---------------+----------+----------+--------------+
 * |  rsm_apbm  |       X       |    X     |    X     |              |
 * +------------+---------------+----------+----------+--------------+
 * |  cmn_apbm  |       X       |    X     |    X     |              |
 * +------------+---------------+----------+----------+--------------+
 * |  tcu_apbm  |       X       |          |          |              |
 * +------------+---------------+----------+----------+--------------+
 * |  lcp_axim  |               |          |          |      X       |
 * +------------+---------------+----------+----------+--------------+
 * |  app_axim  |       X       |    X     |    X     |              |
 * +------------+---------------+----------+----------+--------------+
 * |app_scp_axim|               |          |    X     |              |
 * +------------+---------------+----------+----------+--------------+
 * |app_mcp_axim|               |          |          |              |
 * +------------+---------------+----------+----------+--------------+
 * |lcp_scp_axim|               |          |          |              |
 * +------------+---------------+----------+----------+--------------+
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
    /*
     * Generic refclk registers + AP<->SCP MHUv3 registers +
     * AP<->MCP MHUv3 registers + AP<->RSE MHUv3 registers +
     * SCP<->SCP MHU registers + MCP<->MCP MHU registers
     */
    {
        HOST_GENERIC_REFCLK_CNTREAD_PHYS_BASE,
        HOST_MCP_TO_MCP_MHU_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* RSE<->RSE Cross-chip MHU registers */
    {
        HOST_RSE_TO_RSE_MHU_PHYS_BASE,
        HOST_RSE_TO_RSE_MHU_PHYS_LIMIT,
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
    /* GIC-700 */
    {
        HOST_GIC_700_PHYS_BASE,
        HOST_GIC_700_PHYS_LIMIT,
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
    /* LCP peripherals + Cluster Utility memory region */
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
    /* MCP Access region */
    {
        HOST_MCP_PHYS_BASE,
        HOST_MCP_PHYS_LIMIT,
        SYSCTRL_RSE_MCP_AMNI_ID
    }
};

/*
 * Request originating from SCP ATU is mapped to targets based on following
 * address map.
 */
static const struct ni_tower_psam_reg_cfg_info scp_axis_psam[] = {
    /* Shared SRAM + AP Memory Expansion 1 */
    {
        HOST_AP_SHARED_SRAM_PHYS_BASE,
        HOST_AP_MEM_EXP_1_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* Generic refclk registers */
    {
        HOST_GENERIC_REFCLK_CNTCONTROL_PHYS_BASE,
        HOST_GENERIC_REFCLK_CNTCONTROL_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* SCP ECC error record */
    {
        HOST_SCP_S_ARSM_RAM_ECC_REC_PHYS_BASE,
        HOST_SCP_RL_ARSM_RAM_ECC_REC_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* SCP/MCP RSM ECC error record */
    {
        HOST_SCP_S_RSM_RAM_ECC_REC_PHYS_BASE,
        HOST_MCP_NS_RSM_RAM_ECC_REC_PHYS_LIMIT,
        SYSCTRL_RSM_PMNI_ID
    },
    /* Refclk registers */
    {
        HOST_GENERIC_REFCLK_CNTREAD_PHYS_BASE,
        HOST_AP_NS_REFCLK_CNTBASE1_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* AP<->SCP MHUv3 registers */
    {
        HOST_AP_NS_SCP_MHUV3_PHYS_BASE,
        HOST_AP_RT_SCP_MHUV3_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* Shared RSM SRAM */
    {
        HOST_RSM_SRAM_PHYS_BASE,
        HOST_RSM_SRAM_PHYS_LIMIT,
        SYSCTRL_RSM_AMNI_ID
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
    /* LCP peripherals + Cluster Utility memory region */
    {
        HOST_CLUST_UTIL_PHYS_BASE,
        HOST_CLUST_UTIL_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* SMMU + NCI GPV + PCIe CTRL + PHY */
    {
        HOST_IO_BLOCK_PHYS_BASE,
        HOST_IO_BLOCK_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* AP Memory Expansion 2 */
    {
        HOST_AP_MEM_EXP_2_PHYS_BASE,
        HOST_AP_MEM_EXP_2_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /*
     * PCIe NCI Memory space 2 + PCIe NCI Memory space 3 + DRAM +
     * AP Memory Expansion 3
     */
    {
        HOST_PCIE_NCI_2_PHYS_BASE,
        HOST_PCIE_NCI_3_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
};

/*
 * Request originating from MCP ATU is mapped to targets based on following
 * address map.
 */
static const struct ni_tower_psam_reg_cfg_info mcp_axis_psam[] = {
    /* Shared SRAM */
    {
        HOST_AP_SHARED_SRAM_PHYS_BASE,
        HOST_AP_SHARED_SRAM_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* Generic refclk registers */
    {
        HOST_GENERIC_REFCLK_CNTCONTROL_PHYS_BASE,
        HOST_GENERIC_REFCLK_CNTCONTROL_PHYS_LIMIT,
        SYSCTRL_APP_SCP_AMNI_ID
    },
    /* AP ECC error record */
    {
        HOST_AP_S_ARSM_RAM_ECC_REC_PHYS_BASE,
        HOST_RSE_RL_ARSM_RAM_ECC_REC_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* SCP/MCP RSM ECC error record */
    {
        HOST_SCP_S_RSM_RAM_ECC_REC_PHYS_BASE,
        HOST_MCP_NS_RSM_RAM_ECC_REC_PHYS_LIMIT,
        SYSCTRL_RSM_PMNI_ID
    },
    /* AP<->MCP MHUv3 registers + AP<->RSE MHUv3 registers */
    {
        HOST_AP_NS_MCP_MHUV3_SEND_BASE,
        HOST_AP_RL_RSE_MHUV3_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* MCP<->MCP MHU registers */
    {
        HOST_MCP_TO_MCP_MHU_PHYS_BASE,
        HOST_MCP_TO_MCP_MHU_PHYS_LIMIT,
        SYSCTRL_APP_AMNI_ID
    },
    /* Shared RSM SRAM */
    {
        HOST_RSM_SRAM_PHYS_BASE,
        HOST_RSM_SRAM_PHYS_LIMIT,
        SYSCTRL_RSM_AMNI_ID
    },
};

/*
 * Accesses from RSE and SCP targeting LCP address space are handled by a
 * NIC-400 which then forwards only the bottom [20:0] address bits of the
 * request to rse_scp_axis interface. The bottom [20:0] address bits
 * (range: 0x0 - 0x1FFFFF) are sent from NIC-400 to System Control NI-Tower
 * for APU filtering of request from RSE/SCP to LCPs.
 */
static const struct ni_tower_psam_reg_cfg_info rse_scp_axis_psam[] = {
    {
        HOST_CLUS_UTIL_LCP_SMCF_OFF_ADDR_PHYS_BASE,
        HOST_CLUS_UTIL_MPMM_OFF_ADDR_PHYS_LIMIT,
        SYSCTRL_LCP_AMNI_ID
    },
};

/*
 * Requester side MCP AXIS APU to check access permission targeting Generic
 * refclk in SCP and shared RSM SRAM
 */
static const struct ni_tower_apu_reg_cfg_info mcp_axis_apu[] = {
    /* Root read-write permission : Generic refclk registers */
    INIT_APU_REGION(HOST_GENERIC_REFCLK_CNTCONTROL_PHYS_BASE,
                    HOST_GENERIC_REFCLK_CNTCONTROL_PHYS_LIMIT,
                    NI_T_ROOT_RW),
    /* Full permission : Shared RSM SRAM */
    INIT_APU_REGION(HOST_RSM_SRAM_PHYS_BASE,
                    HOST_RSM_SRAM_PHYS_LIMIT,
                    NI_T_ALL_PERM),
};

/*
 * Completer side RSM AXIM APU to check access permission targeting Shared RAM
 * between RSE, SCP and MCP
 */
static const struct ni_tower_apu_reg_cfg_info rsm_axim_apu[] = {
    INIT_APU_REGION(HOST_RSM_SRAM_PHYS_BASE,
                    HOST_RSM_SRAM_PHYS_LIMIT,
                    NI_T_ALL_PERM),
};

/*
 * RSM APBM APU to check the ECC Error record register block for Shared RAM
 * between RSE, SCP and MCP
 */
static const struct ni_tower_apu_reg_cfg_info rsm_apbm_apu[] = {
    INIT_APU_REGION(HOST_RSE_S_RSM_RAM_ECC_REC_PHYS_BASE,
                    HOST_RSE_S_RSM_RAM_ECC_REC_PHYS_LIMIT,
                    NI_T_SEC_RW | NI_T_ROOT_RW),
    INIT_APU_REGION(HOST_RSE_NS_RSM_RAM_ECC_REC_PHYS_BASE,
                    HOST_RSE_NS_RSM_RAM_ECC_REC_PHYS_LIMIT,
                    NI_T_ALL_PERM),
    INIT_APU_REGION(HOST_SCP_S_RSM_RAM_ECC_REC_PHYS_BASE,
                    HOST_SCP_S_RSM_RAM_ECC_REC_PHYS_LIMIT,
                    NI_T_SEC_RW | NI_T_ROOT_RW),
    INIT_APU_REGION(HOST_SCP_NS_RSM_RAM_ECC_REC_PHYS_BASE,
                    HOST_SCP_NS_RSM_RAM_ECC_REC_PHYS_LIMIT,
                    NI_T_ALL_PERM),
    INIT_APU_REGION(HOST_MCP_S_RSM_RAM_ECC_REC_PHYS_BASE,
                    HOST_MCP_S_RSM_RAM_ECC_REC_PHYS_LIMIT,
                    NI_T_SEC_RW | NI_T_ROOT_RW),
    INIT_APU_REGION(HOST_MCP_NS_RSM_RAM_ECC_REC_PHYS_BASE,
                    HOST_MCP_NS_RSM_RAM_ECC_REC_PHYS_LIMIT,
                    NI_T_ALL_PERM),
};

/*
 * Completer side RSE-SCP AXIM APU to check access permission targeting the SCP
 */
static const struct ni_tower_apu_reg_cfg_info rse_scp_axim_apu[] = {
    INIT_APU_REGION(HOST_SCP_PHYS_BASE,
                    HOST_SCP_PHYS_LIMIT,
                    NI_T_ALL_PERM),
};

/*
 * Completer side RSE-MCP AXIM APU to check access permission targeting the MCP
 */
static const struct ni_tower_apu_reg_cfg_info rse_mcp_axim_apu[] = {
    INIT_APU_REGION(HOST_MCP_PHYS_BASE,
                    HOST_MCP_PHYS_LIMIT,
                    NI_T_ALL_PERM),
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
            .dev_cfg = &SYSCTRL_RSE_MAIN_ASNI_PSAM_DEV_CFG,
            .nh_region_count = ARRAY_SIZE(rse_main_axis_psam),
            .regions = rse_main_axis_psam,
        },
        {
            .dev_cfg = &SYSCTRL_SCP_ASNI_PSAM_DEV_CFG,
            .nh_region_count = ARRAY_SIZE(scp_axis_psam),
            .regions = scp_axis_psam,
        },
        {
            .dev_cfg = &SYSCTRL_MCP_ASNI_PSAM_DEV_CFG,
            .nh_region_count = ARRAY_SIZE(mcp_axis_psam),
            .regions = mcp_axis_psam,
        },
        {
            .dev_cfg = &SYSCTRL_RSE_SCP_ASNI_PSAM_DEV_CFG,
            .nh_region_count = ARRAY_SIZE(rse_scp_axis_psam),
            .regions = rse_scp_axis_psam,
        },
    };

    err = ni_tower_program_psam_table(&SYSCTRL_NI_TOWER_DEV, psam_table,
            ARRAY_SIZE(psam_table));
    if (err != NI_TOWER_SUCCESS) {
        return -1;
    }

    return 0;
}

/*
 * Configure Access Protection Unit (APU) to setup access permission for each
 * memory region based on its target in System Control NI-Tower for nodes
 * under AON domain.
 */
static int32_t program_sysctrl_apu_aon(void)
{
    enum ni_tower_err err;

    /*
     * Populates all APU entry into a table array to confgiure and enable
     * desired APUs
     */
    struct ni_tower_apu_cfgs apu_table[] = {
        {
            .dev_cfg = &SYSCTRL_MCP_ASNI_APU_DEV_CFG,
            .region_count = ARRAY_SIZE(mcp_axis_apu),
            .regions = mcp_axis_apu,
        },
        {
            .dev_cfg = &SYSCTRL_RSM_AMNI_APU_DEV_CFG,
            .region_count = ARRAY_SIZE(rsm_axim_apu),
            .regions = rsm_axim_apu,
        },
        {
            .dev_cfg = &SYSCTRL_RSM_PMNI_APU_DEV_CFG,
            .region_count = ARRAY_SIZE(rsm_apbm_apu),
            .regions = rsm_apbm_apu,
        },
        {
            .dev_cfg = &SYSCTRL_RSE_SCP_AMNI_APU_DEV_CFG,
            .region_count = ARRAY_SIZE(rse_scp_axim_apu),
            .regions = rse_scp_axim_apu,
        },
        {
            .dev_cfg = &SYSCTRL_RSE_MCP_AMNI_APU_DEV_CFG,
            .region_count = ARRAY_SIZE(rse_mcp_axim_apu),
            .regions = rse_mcp_axim_apu,
        },
    };

    err = ni_tower_program_apu_table(&SYSCTRL_NI_TOWER_DEV, apu_table,
            ARRAY_SIZE(apu_table));
    if (err != NI_TOWER_SUCCESS) {
        return -1;
    }

    return 0;
}

int32_t program_sysctrl_ni_tower_aon(void)
{
    if (program_sysctrl_psam_aon() != 0) {
        return -1;
    }

    if (program_sysctrl_apu_aon() != 0) {
        return -1;
    }

    return 0;
}
