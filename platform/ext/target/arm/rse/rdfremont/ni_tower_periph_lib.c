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
 * Software implementation defined region for SRAM and is accessible by AP,
 * RSE, SCP and MCP. First region (AP BL1 code region) will be changed to Read
 * only access after RSE loads AP BL1 image.
 */
static const struct ni_tower_apu_reg_cfg_info ram_axim_apu[] = {
    INIT_APU_REGION(HOST_AP_BL1_RO_SRAM_PHYS_BASE,
                    HOST_AP_BL1_RO_SRAM_PHYS_LIMIT,
                    NI_T_ROOT_RW),
    INIT_APU_REGION(HOST_AP_BL1_RW_SRAM_PHYS_BASE,
                    HOST_AP_BL1_RW_SRAM_PHYS_LIMIT,
                    NI_T_ROOT_RW),
};

/* AP Non-secure UART */
static const struct ni_tower_apu_reg_cfg_info nsuart0_apbm_apu[] = {
    INIT_APU_REGION(HOST_NS_UART_PHYS_BASE,
                    HOST_NS_UART_PHYS_LIMIT,
                    NI_T_ALL_PERM),
};

/* AP Secure UART */
static const struct ni_tower_apu_reg_cfg_info secuart_apbm_apu[] = {
    INIT_APU_REGION(HOST_S_UART_PHYS_BASE,
                    HOST_S_UART_PHYS_LIMIT,
                    NI_T_ROOT_RW | NI_T_SEC_RW),
};

/* AP Non-secure UART for RMM debug */
static const struct ni_tower_apu_reg_cfg_info nsuart1_apbm_apu[] = {
    INIT_APU_REGION(HOST_RMM_NS_UART_PHYS_BASE,
                    HOST_RMM_NS_UART_PHYS_LIMIT,
                    NI_T_REALM_RW),
};

/* AP Non-secure WatchDog */
static const struct ni_tower_apu_reg_cfg_info nsgenwdog_apbm_apu[] = {
    INIT_APU_REGION(HOST_AP_NS_WDOG_PHYS_BASE,
                    HOST_AP_NS_WDOG_PHYS_LIMIT,
                    NI_T_ALL_PERM),
};

/* AP root WatchDog */
static const struct ni_tower_apu_reg_cfg_info rootgenwdog_apbm_apu[] = {
    INIT_APU_REGION(HOST_AP_RT_WDOG_PHYS_BASE,
                    HOST_AP_RT_WDOG_PHYS_LIMIT,
                    NI_T_ROOT_RW),
};

/* AP Secure WatchDog */
static const struct ni_tower_apu_reg_cfg_info secgenwdog_apbm_apu[] = {
    INIT_APU_REGION(HOST_AP_S_WDOG_PHYS_BASE,
                    HOST_AP_S_WDOG_PHYS_LIMIT,
                    NI_T_ROOT_RW | NI_T_SEC_RW),
};

/*
 * Configure Access Protection Unit (APU) to setup access permission for each
 * memory region based on its target in Peripheral NI-Tower.
 */
static int32_t program_periph_apu(void)
{
    enum ni_tower_err err;

    /*
     * Populates all APU entry into a table array to confgiure and enable
     * desired APUs
     */
    const struct ni_tower_apu_cfgs apu_table[] = {
        {
            .dev_cfg = &PERIPH_RAM_AMNI_APU_DEV_CFG,
            .region_count = ARRAY_SIZE(ram_axim_apu),
            .regions = ram_axim_apu,
        },
        {
            .dev_cfg = &PERIPH_NSUART0_PMNI_APU_DEV_CFG,
            .region_count = ARRAY_SIZE(nsuart0_apbm_apu),
            .regions = nsuart0_apbm_apu,
        },
        {
            .dev_cfg = &PERIPH_SECUART_PMNI_APU_DEV_CFG,
            .region_count = ARRAY_SIZE(secuart_apbm_apu),
            .regions = secuart_apbm_apu,
        },
        {
            .dev_cfg = &PERIPH_NSUART1_PMNI_APU_DEV_CFG,
            .region_count = ARRAY_SIZE(nsuart1_apbm_apu),
            .regions = nsuart1_apbm_apu,
        },
        {
            .dev_cfg = &PERIPH_NSGENWDOG_PMNI_APU_DEV_CFG,
            .region_count = ARRAY_SIZE(nsgenwdog_apbm_apu),
            .regions = nsgenwdog_apbm_apu,
        },
        {
            .dev_cfg = &PERIPH_ROOTGENWDOG_PMNI_APU_DEV_CFG,
            .region_count = ARRAY_SIZE(rootgenwdog_apbm_apu),
            .regions = rootgenwdog_apbm_apu,
        },
        {
            .dev_cfg = &PERIPH_SECGENWDOG_PMNI_APU_DEV_CFG,
            .region_count = ARRAY_SIZE(secgenwdog_apbm_apu),
            .regions = secgenwdog_apbm_apu,
        },
    };

    err = ni_tower_program_apu_table(&PERIPH_NI_TOWER_DEV, apu_table,
                                                        ARRAY_SIZE(apu_table));
    if (err != NI_TOWER_SUCCESS) {
        return -1;
    }

    return 0;
}
