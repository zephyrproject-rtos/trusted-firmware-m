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

/* Peripheral NI-Tower component nodes */
const struct ni_tower_component_node periph_ram_amni  = {
    .type = NI_TOWER_AMNI,
    .id = PERIPH_RAM_AMNI_ID,
};

/*
 * Software implementation defined region for SRAM and is accessible by AP,
 * RSE and SCP. First region (AP BL2 code region) will be changed to Read
 * only access after RSE loads AP BL2 image.
 */
static const struct ni_tower_apu_reg_cfg_info ram_axim_apu[] = {
    INIT_APU_REGION(HOST_AP_BL2_RO_SRAM_PHYS_BASE,
                    HOST_AP_BL2_RO_SRAM_PHYS_LIMIT,
                    NI_T_ROOT_RW | NI_T_SEC_RW),
    INIT_APU_REGION(HOST_AP_BL2_RW_SRAM_PHYS_BASE,
                    HOST_AP_BL2_RW_SRAM_PHYS_LIMIT,
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
            .component = &periph_ram_amni,
            .region_count = ARRAY_SIZE(ram_axim_apu),
            .regions = ram_axim_apu,
            .add_chip_addr_offset = false,
        },
    };

    err = ni_tower_program_apu_table(&PERIPH_NI_TOWER_DEV, apu_table,
                                                        ARRAY_SIZE(apu_table));
    if (err != NI_TOWER_SUCCESS) {
        return -1;
    }

    return 0;
}
