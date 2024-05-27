/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NI_TOWER_DRV_H__
#define __NI_TOWER_DRV_H__

#include <stdint.h>

/**
 * \brief NI-Tower error enumeration types
 */
enum ni_tower_err {
    NI_TOWER_SUCCESS,
    NI_TOWER_ERR_INVALID_ARG,
    NI_TOWER_ERR_NOT_FOUND,
    NI_TOWER_DEVICE_INVALID,
    NI_TOWER_ERR_NOT_PERMITTED,
    NI_TOWER_ERR_REGION_OVERLAPS,
    NI_TOWER_ERR
};

/**
 * \brief NI-Tower node type value enumerations
 */
enum ni_tower_node_type_value {
    /* Domains */
    NI_TOWER_CFGNI = 0x0,
    NI_TOWER_VD,
    NI_TOWER_PD,
    NI_TOWER_CD,
    /* Components */
    NI_TOWER_ASNI,
    NI_TOWER_AMNI,
    NI_TOWER_PMU,
    NI_TOWER_HSNI,
    NI_TOWER_HMNI,
    NI_TOWER_PMNI
};

/**
 * \brief NI-Tower subfeature type value enumerations
 */
enum ni_tower_subfeature_type_value {
    NI_TOWER_APU = 0x0,
    NI_TOWER_PSAM,
    NI_TOWER_FCU,
    NI_TOWER_IDM
};

/**
 * \brief NI-Tower configuration node granularity enumeration
 */
enum ni_tower_granularity {
    NI_TOWER_4KB_CONFIG_NODES,
    NI_TOWER_64KB_CONFIG_NODES,
};

/**
 * \brief NI-Tower component node structure
 */
struct ni_tower_component_node{
    /* Component type of the node */
    const enum ni_tower_node_type_value type;
    /* Component id of the node */
    const uint32_t id;
};

/**
 * \brief NI-Tower skip component discovery node data structure
 */
struct ni_tower_skip_component_discovery_node_data {
    /* Parent component node of the node to be skipped */
    const struct ni_tower_component_node *parent_node;
    /*
     * Index of the child node of the \ref
     * ni_tower_skip_component_discovery_node_data.parent_node to be skipped.
     */
    const uint32_t node_idx;
};

/**
 * \brief NI-Tower skip component discovery list structure
 */
struct ni_tower_skip_component_discovery_list {
    /*
     * List of all skip node data to be skipped during discovery as specified
     * in struct \ref ni_tower_skip_component_discovery_list_node.
     */
    const struct ni_tower_skip_component_discovery_node_data *skip_node_data;
    /* Number of component nodes to be skipped */
    const uint32_t skip_node_count;
};

/**
 * \brief NI-Tower device structure
 */
struct ni_tower_dev {
    /* NI-Tower periphbase address, same as CFGNI0 address */
    const uintptr_t periphbase;
    /*
     * The memory-mapped registers of NI-Tower are organized in a series of
     * 4KB or 64KB regions. Specify whether NI-Tower has 4KB or 64KB config
     * nodes.
     */
    const enum ni_tower_granularity config_node_granularity;
    /*
     * Pointer to skip component discovery node list structure. This includes
     * list of all component nodes to be skipped during the discovery process.
     * This can be updated during runtime since the discovery reach of a
     * component node can depend on state of the host system.
     */
    struct ni_tower_skip_component_discovery_list *skip_discovery_list;
    /*
     * Address space offset for the current chip. This is typically updated
     * during the boot time by reading the current chip id (in case of RSE
     * present in a multichip platform). The offset will be added to the region
     * base and end address if the component enables \ref add_chip_addr_offset
     * flag.
     */
    uint64_t chip_addr_offset;
};

#endif /* __NI_TOWER_DRV_H__ */
