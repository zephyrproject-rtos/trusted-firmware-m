/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NI_TOWER_DISCOVERY_DRV_H__
#define __NI_TOWER_DISCOVERY_DRV_H__

#include "ni_tower_drv.h"

#include <stdint.h>

/**
 * \brief NI-Tower Discovery node structure.
 */
struct ni_tower_discovery_node {
    enum ni_tower_node_type_value node_type;
    uint32_t node_id;
    uint32_t node_off_addr;
};

/**
 * \brief Does a discovery flow search and returns subfeature offset address
 *        based on the parent component ID and type.
 *
 * \param[in]   dev                  NI-Tower struct \ref ni_tower_dev.
 * \param[in]   cfg_node             Root discovery node to perform discovery
 *                                   struct \ref ni_tower_discovery_node.
 * \param[in]   component_node_type  Specify the parent component type of the
 *                                   subfeature enum \ref
 *                                   ni_tower_node_type_value.
 * \param[in]   component_node_id    Specify the parent component id of the
 *                                   subfeature.
 * \param[in]   subfeature_node_type Specify the type of the subfeature node
 *                                   enum \ref ni_tower_subfeature_type_value.
 * \param[out]  ret_address          Fetched offset address for the specified
 *                                   node.
 *
 * \return Returns error code as specified in enum \ref ni_tower_err.
 */
enum ni_tower_err ni_tower_discover_offset(
    const struct ni_tower_dev *dev,
    const struct ni_tower_discovery_node *cfg_node,
    const enum ni_tower_node_type_value component_node_type,
    const uint32_t component_node_id,
    const enum ni_tower_subfeature_type_value subfeature_node_type,
    uint32_t *ret_off_addr);

#endif /* __NI_TOWER_DISCOVERY_DRV_H__ */
