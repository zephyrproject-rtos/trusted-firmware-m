/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NOC_S3_RSE_DRV_H__
#define __NOC_S3_RSE_DRV_H__

#include <stdbool.h>
#include <stdint.h>

#include "ni_tower_drv.h"
#include "apu/ni_tower_apu_drv.h"
#include "discovery/ni_tower_discovery_drv.h"
#include "psam/ni_tower_psam_drv.h"

/**
 * \brief NoC S3 PSAM config structure
 */
struct noc_s3_psam_cfgs {
    /* Pointer to the PSAM device configuration */
    const struct noc_s3_component_node* component;
    /* Number of non-hashed regions that needs to be configured */
    const uint32_t nh_region_count;
    /* List of all region configuration information */
    const struct noc_s3_psam_reg_cfg_info* regions;
    /* Whether to add chip address offset to the memory map regions */
    const bool add_chip_addr_offset;
};

/**
 * \brief NoC S3 APU config structure
 */
struct noc_s3_apu_cfgs {
    /* Pointer to the APU device configuration */
    const struct noc_s3_component_node* component;
    /* Number of address regions that needs to be configured */
    const uint32_t region_count;
    /* List of all region configuration information */
    const struct noc_s3_apu_reg_cfg_info* regions;
    /* Whether to add chip address offset to the memory map regions */
    const bool add_chip_addr_offset;
};

/**
 * \brief Program NoC S3 PSAM from PSAM configs
 *
 * \param[in]   dev               NoC S3 struct \ref noc_s3_dev.
 * \param[in]   psam_table        Array of all PSAM configs for a particular
 *                                NoC S3 \ref noc_s3_psam_cfgs.
 * \param[in]   psam_table_count  Number of PSAM that needs to be configured.
 *
 * \return Returns error code as specified in \ref noc_s3_err
 */
enum noc_s3_err noc_s3_program_psam_table(
    const struct noc_s3_dev *dev,
    const struct noc_s3_psam_cfgs psam_table[],
    const uint32_t psam_table_count);

/**
 * \brief Program NoC S3 APU from APU configs
 *
 * \param[in]   dev               NoC S3 struct \ref noc_s3_dev.
 * \param[in]   apu_table         Array of all APU configs for a particular
 *                                NoC S3 \ref noc_s3_apu_cfgs.
 * \param[in]   apu_table_count   Number of APUs that needs to be configured.
 *
 * \return Returns error code as specified in \ref noc_s3_err
 */
enum noc_s3_err noc_s3_program_apu_table(
    const struct noc_s3_dev *dev,
    const struct noc_s3_apu_cfgs apu_table[],
    const uint32_t apu_table_count);

#endif /* __NOC_S3_RSE_DRV_H__ */
