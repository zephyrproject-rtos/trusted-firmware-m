/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NI_TOWER_RSE_DRV_H__
#define __NI_TOWER_RSE_DRV_H__

#include <stdbool.h>
#include <stdint.h>

#include "ni_tower_drv.h"
#include "apu/ni_tower_apu_drv.h"
#include "discovery/ni_tower_discovery_drv.h"
#include "psam/ni_tower_psam_drv.h"

/**
 * \brief NI-Tower PSAM config structure
 */
struct ni_tower_psam_cfgs {
    /* Pointer to the PSAM device configuration */
    const struct ni_tower_component_node* component;
    /* Number of non-hashed regions that needs to be configured */
    const uint32_t nh_region_count;
    /* List of all region configuration information */
    const struct ni_tower_psam_reg_cfg_info* regions;
    /* Whether to add chip address offset to the memory map regions */
    const bool add_chip_addr_offset;
};

/**
 * \brief NI-Tower APU config structure
 */
struct ni_tower_apu_cfgs {
    /* Pointer to the APU device configuration */
    const struct ni_tower_component_node* component;
    /* Number of address regions that needs to be configured */
    const uint32_t region_count;
    /* List of all region configuration information */
    const struct ni_tower_apu_reg_cfg_info* regions;
    /* Whether to add chip address offset to the memory map regions */
    const bool add_chip_addr_offset;
};

/**
 * \brief Program NI-Tower PSAM from PSAM configs
 *
 * \param[in]   dev               NI-Tower struct \ref ni_tower_dev.
 * \param[in]   psam_table        Array of all PSAM configs for a particular
 *                                NI-Tower \ref ni_tower_psam_cfgs.
 * \param[in]   psam_table_count  Number of PSAM that needs to be configured.
 *
 * \return Returns error code as specified in \ref ni_tower_err
 */
enum ni_tower_err ni_tower_program_psam_table(
    const struct ni_tower_dev *dev,
    const struct ni_tower_psam_cfgs psam_table[],
    const uint32_t psam_table_count);

/**
 * \brief Program NI-Tower APU from APU configs
 *
 * \param[in]   dev               NI-Tower struct \ref ni_tower_dev.
 * \param[in]   apu_table         Array of all APU configs for a particular
 *                                NI-Tower \ref ni_tower_apu_cfgs.
 * \param[in]   apu_table_count   Number of APUs that needs to be configured.
 *
 * \return Returns error code as specified in \ref ni_tower_err
 */
enum ni_tower_err ni_tower_program_apu_table(
    const struct ni_tower_dev *dev,
    const struct ni_tower_apu_cfgs apu_table[],
    const uint32_t apu_table_count);

#endif /* __NI_TOWER_RSE_DRV_H__ */
