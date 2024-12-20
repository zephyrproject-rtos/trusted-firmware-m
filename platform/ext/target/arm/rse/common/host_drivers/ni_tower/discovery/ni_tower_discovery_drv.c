/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "ni_tower_discovery_drv.h"
#include "ni_tower_discovery_reg.h"

#include <stdbool.h>
#include <stdlib.h>

#define FMU_CHECK(node_base) \
                (*(uint32_t *)((node_base) + FMU_CHECK_OFFSET_ADDRESS))

static inline bool noc_s3_type_is_domain(
    const enum noc_s3_node_type_value type)
{
    return ((type) == NOC_S3_CFGNI) || ((type) == NOC_S3_VD) ||
           ((type) == NOC_S3_PD) || ((type) == NOC_S3_CD);
}

static inline bool noc_s3_type_is_component(
    const enum noc_s3_node_type_value type)
{
    return ((type) == NOC_S3_ASNI) || ((type) == NOC_S3_AMNI) ||
           ((type) == NOC_S3_PMU) || ((type) == NOC_S3_HSNI) ||
           ((type) == NOC_S3_HMNI) || ((type) == NOC_S3_PMNI);
}

static inline bool noc_s3_type_is_subfeature(
    const enum noc_s3_subfeature_type_value type)
{
    return ((type) == NOC_S3_APU) || ((type) == NOC_S3_PSAM) ||
           ((type) == NOC_S3_FCU) || ((type) == NOC_S3_IDM);
}

static enum noc_s3_err noc_s3_check_interconnect_part_number(
    const struct noc_s3_dev *dev)
{
    struct noc_s3_global_reg_map* reg;
    if (dev != NULL && dev->periphbase == (uintptr_t)NULL) {
        return NOC_S3_ERR_INVALID_ARG;
    }

    reg = (struct noc_s3_global_reg_map*)dev->periphbase;

    if ((reg->peripheral_id0 & NOC_S3_GLOBAL_PERIPHERAL_ID0 &
                                            ~NOC_S3_PID0_PART_NUM) != 0 &&
        (reg->peripheral_id1 & NOC_S3_GLOBAL_PERIPHERAL_ID1 &
                                            ~NOC_S3_PID1_PART_NUM) != 0) {
        return NOC_S3_DEVICE_INVALID;
    }

    return NOC_S3_SUCCESS;
}

static enum noc_s3_err noc_s3_discover_subfeature_offset(
    const struct noc_s3_dev *dev,
    const uint32_t component_offset_address,
    const enum noc_s3_subfeature_type_value subfeature_node_type,
    uint32_t *ret_off_addr)
{
    uint32_t s_idx;
    struct noc_s3_component_cfg_hdr *component_hdr;

    if (dev == NULL || dev->periphbase == (uintptr_t)NULL) {
        return NOC_S3_ERR_INVALID_ARG;
    }

    if (!noc_s3_type_is_subfeature(subfeature_node_type)) {
        return NOC_S3_ERR_INVALID_ARG;
    }

    component_hdr = (struct noc_s3_component_cfg_hdr *)(dev->periphbase +
                                                    component_offset_address);

    for(s_idx = 0; s_idx < component_hdr->num_subfeatures; ++s_idx) {
        if (component_hdr->subfeature[s_idx].type == subfeature_node_type) {
            *ret_off_addr = component_hdr->subfeature[s_idx].pointer;
            return NOC_S3_SUCCESS;
        }
    }

    return NOC_S3_ERR_NOT_FOUND;
}

enum noc_s3_err noc_s3_discover_offset(
    const struct noc_s3_dev *dev,
    const struct noc_s3_discovery_node *cfg_node,
    const enum noc_s3_node_type_value component_node_type,
    const uint32_t component_node_id,
    const enum noc_s3_subfeature_type_value subfeature_node_type,
    uint32_t *ret_off_addr)
{
    enum noc_s3_err err;
    struct noc_s3_discovery_node node;
    struct noc_s3_domain_cfg_hdr *domain_cfg_ptr, *c_hdr;
    uint32_t c_off_addr, c_idx, s_idx;
    uint32_t hdr_base;
    bool should_skip_discovery;

    if (dev == NULL || dev->periphbase == (uintptr_t)NULL) {
        return NOC_S3_ERR_INVALID_ARG;
    }

    if (cfg_node == NULL) {
        return NOC_S3_ERR_INVALID_ARG;
    }

    hdr_base = (uint32_t)dev->periphbase + cfg_node->node_off_addr;

    if (!noc_s3_type_is_component(component_node_type) ||
        !noc_s3_type_is_subfeature(subfeature_node_type)) {
        return NOC_S3_ERR_INVALID_ARG;
    }

    /* Check interconnect part number */
    if (cfg_node->node_type == NOC_S3_CFGNI) {
        err = noc_s3_check_interconnect_part_number(dev);
        if (err != NOC_S3_SUCCESS) {
            return err;
        }
    }

    /*
     * FMU check requires the read at 0xFFE0 offset which can only be done
     * for 64KB config node granularity mode.
     */
    if (dev->config_node_granularity == NOC_S3_64KB_CONFIG_NODES) {
        if ((cfg_node->node_type != NOC_S3_CFGNI) &&
            FMU_CHECK(hdr_base) != 0) {
            return NOC_S3_ERR_NOT_FOUND;
        }
    }

    if (noc_s3_type_is_domain(cfg_node->node_type)) {
        domain_cfg_ptr = (struct noc_s3_domain_cfg_hdr *)hdr_base;
        for(c_idx = 0; c_idx < domain_cfg_ptr->child_node_info; ++c_idx) {
            /* Skip discovering node based on the skip node list */
            should_skip_discovery = false;
            if (dev->skip_discovery_list != NULL) {
                for (s_idx = 0;
                     s_idx < dev->skip_discovery_list->skip_node_count;
                     ++s_idx) {
                    if (dev->skip_discovery_list->skip_node_data[s_idx]
                         .parent_node->type == cfg_node->node_type &&
                        dev->skip_discovery_list->skip_node_data[s_idx]
                         .parent_node->id == cfg_node->node_id)
                    {
                        if (dev->skip_discovery_list->skip_node_data[s_idx]
                             .node_idx == c_idx) {
                            should_skip_discovery = true;
                            break;
                        }
                    }
                }
            }

            if (should_skip_discovery) {
                continue;
            }

            c_off_addr = domain_cfg_ptr->x_pointers[c_idx];
            c_hdr = (struct noc_s3_domain_cfg_hdr *)(dev->periphbase +
                                                                c_off_addr);
            /* Recursively discover child node */
            node = (struct noc_s3_discovery_node) {
               .node_type = (c_hdr->node_type &
                                            NOC_S3_DOMAIN_NODE_TYPE_MSK) >>
                            NOC_S3_DOMAIN_NODE_TYPE_POS,
               .node_id = (c_hdr->node_type & NOC_S3_DOMAIN_NODE_ID_MSK) >>
                          NOC_S3_DOMAIN_NODE_ID_POS,
               .node_off_addr = c_off_addr
            };

            err = noc_s3_discover_offset(dev, &node, component_node_type,
                                           component_node_id,
                                           subfeature_node_type,
                                           ret_off_addr);
            if (err != NOC_S3_ERR_NOT_FOUND) {
                return err;
            }
        }
    } else if (noc_s3_type_is_component(cfg_node->node_type)) {
        /* Skip traversing of components which is not requested */
        if (cfg_node->node_id != component_node_id ||
            cfg_node->node_type != component_node_type) {
            return NOC_S3_ERR_NOT_FOUND;
        }
        /* Skip since PMU doesn't have children */
        if (cfg_node->node_type == NOC_S3_PMU) {
            return NOC_S3_ERR_NOT_FOUND;
        }
        return noc_s3_discover_subfeature_offset(dev,
                                                   cfg_node->node_off_addr,
                                                   subfeature_node_type,
                                                   ret_off_addr);
    } else {
        return NOC_S3_ERR_INVALID_ARG;
    }

    return NOC_S3_ERR_NOT_FOUND;
}
