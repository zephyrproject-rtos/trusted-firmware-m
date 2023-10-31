/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "ni_tower_rss_drv.h"

#include <stddef.h>

static enum ni_tower_err ni_tower_fetch_offset_address(
                    struct ni_tower_dev *dev,
                    enum ni_tower_node_type_value component_node_type,
                    uint32_t component_node_id,
                    enum ni_tower_subfeature_type_value subfeature_node_type,
                    uint32_t *off_addr)
{
    struct ni_tower_discovery_node root = {
        .node_type = NI_TOWER_CFGNI,
        .node_id = 0,
        .node_off_addr = 0x0
    };

    return ni_tower_discover_offset(dev, &root, component_node_type,
                                    component_node_id, subfeature_node_type,
                                    off_addr);
}

enum ni_tower_err ni_tower_program_psam_table(struct ni_tower_dev *dev,
                            struct ni_tower_psam_cfgs psam_table[],
                            uint32_t psam_table_count)
{
    enum ni_tower_err err;
    struct ni_tower_psam_dev psam_dev;
    struct ni_tower_psam_reg_cfg_info* r_info;
    uint32_t p_idx, r_idx, off_addr;

    if (dev == NULL || dev->periphbase == (uintptr_t)NULL) {
        return NI_TOWER_ERR_INVALID_ARG;
    }

    if (psam_table_count != 0 && psam_table == NULL) {
        return NI_TOWER_ERR_INVALID_ARG;
    }

    for (p_idx = 0; p_idx < psam_table_count; ++p_idx) {
        /* Discover offset address for the PSAM */
        err = ni_tower_fetch_offset_address(
                                    dev, psam_table[p_idx].component_node_type,
                                    psam_table[p_idx].component_node_id,
                                    NI_TOWER_PSAM, &off_addr);
        if (err != NI_TOWER_SUCCESS) {
            return err;
        }

        psam_dev = (struct ni_tower_psam_dev) {
            .base = dev->periphbase + off_addr
        };

        /* Set region fields */
        for (r_idx = 0; r_idx < psam_table[p_idx].nh_region_count; ++r_idx) {
            r_info = &psam_table[p_idx].regions[r_idx];
            err = ni_tower_psam_configure_nhregion(&psam_dev, r_info, r_idx);
            if (err != NI_TOWER_SUCCESS) {
                return err;
            }
        }

        /* Enable the PSAM region */
        err = ni_tower_psam_enable(&psam_dev);
        if (err != NI_TOWER_SUCCESS) {
            return err;
        }
    }

    return NI_TOWER_SUCCESS;
}

enum ni_tower_err ni_tower_program_apu_table(struct ni_tower_dev *dev,
                           struct ni_tower_apu_cfgs apu_table[],
                           uint32_t apu_table_count)
{
    enum ni_tower_err err;
    struct ni_tower_apu_dev apu_dev;
    struct ni_tower_apu_reg_cfg_info* r_info;
    uint32_t a_idx, r_idx, off_addr;

    if (dev == NULL || dev->periphbase == (uintptr_t)NULL) {
        return NI_TOWER_ERR_INVALID_ARG;
    }

    if (apu_table_count != 0 && apu_table == NULL) {
        return NI_TOWER_ERR_INVALID_ARG;
    }

    for (a_idx = 0; a_idx < apu_table_count; ++a_idx) {
        /* Discover offset address for the PSAM*/
        err = ni_tower_fetch_offset_address(
                                    dev, apu_table[a_idx].component_node_type,
                                    apu_table[a_idx].component_node_id,
                                    NI_TOWER_APU, &off_addr);
        if (err != NI_TOWER_SUCCESS) {
            return err;
        }

        apu_dev = (struct ni_tower_apu_dev) {
            .base = dev->periphbase + off_addr
        };

        /* Set region fields */
        for (r_idx = 0; r_idx < apu_table[a_idx].region_count; ++r_idx) {
            r_info = &apu_table[a_idx].regions[r_idx];
            err = ni_tower_apu_configure_region(&apu_dev, r_info, r_idx);
            if (err != NI_TOWER_SUCCESS) {
                return err;
            }
        }

        err = ni_tower_apu_sync_err_enable(&apu_dev);
        if (err != NI_TOWER_SUCCESS) {
            return err;
        }

        err = ni_tower_apu_enable(&apu_dev);
        if (err != NI_TOWER_SUCCESS) {
            return err;
        }
    }

    return NI_TOWER_SUCCESS;
}
