/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NOC_S3_DRV_H__
#define __NOC_S3_DRV_H__

#include <stdint.h>

/**
 * \brief NoC S3 error enumeration types
 */
enum noc_s3_err {
    NOC_S3_SUCCESS,
    NOC_S3_ERR_INVALID_ARG,
    NOC_S3_ERR_NOT_FOUND,
    NOC_S3_DEVICE_INVALID,
    NOC_S3_ERR_NOT_PERMITTED,
    NOC_S3_ERR_REGION_OVERLAPS,
    NOC_S3_ERR
};

/**
 * \brief NoC S3 node type value enumerations
 */
enum noc_s3_node_type_value {
    /* Domains */
    NOC_S3_CFGNI = 0x0,
    NOC_S3_VD,
    NOC_S3_PD,
    NOC_S3_CD,
    /* Components */
    NOC_S3_ASNI,
    NOC_S3_AMNI,
    NOC_S3_PMU,
    NOC_S3_HSNI,
    NOC_S3_HMNI,
    NOC_S3_PMNI
};

/**
 * \brief NoC S3 subfeature type value enumerations
 */
enum noc_s3_subfeature_type_value {
    NOC_S3_APU = 0x0,
    NOC_S3_PSAM,
    NOC_S3_FCU,
    NOC_S3_IDM
};

/**
 * \brief NoC S3 configuration node granularity enumeration
 */
enum noc_s3_granularity {
    NOC_S3_4KB_CONFIG_NODES,
    NOC_S3_64KB_CONFIG_NODES,
};

/**
 * \brief NoC S3 component node structure
 */
struct noc_s3_component_node{
    /* Component type of the node */
    const enum noc_s3_node_type_value type;
    /* Component id of the node */
    const uint32_t id;
};

/**
 * \brief NoC S3 skip component discovery node data structure
 */
struct noc_s3_skip_component_discovery_node_data {
    /* Parent component node of the node to be skipped */
    const struct noc_s3_component_node *parent_node;
    /*
     * Index of the child node of the \ref
     * noc_s3_skip_component_discovery_node_data.parent_node to be skipped.
     */
    const uint32_t node_idx;
};

/**
 * \brief NoC S3 skip component discovery list structure
 */
struct noc_s3_skip_component_discovery_list {
    /*
     * List of all skip node data to be skipped during discovery as specified
     * in struct \ref noc_s3_skip_component_discovery_list_node.
     */
    const struct noc_s3_skip_component_discovery_node_data *skip_node_data;
    /* Number of component nodes to be skipped */
    const uint32_t skip_node_count;
};

/**
 * \brief NoC S3 device structure
 */
struct noc_s3_dev {
    /* NoC S3 periphbase address, same as CFGNI0 address */
    const uintptr_t periphbase;
    /*
     * The memory-mapped registers of NoC S3 are organized in a series of
     * 4KB or 64KB regions. Specify whether NoC S3 has 4KB or 64KB config
     * nodes.
     */
    const enum noc_s3_granularity config_node_granularity;
    /*
     * Pointer to skip component discovery node list structure. This includes
     * list of all component nodes to be skipped during the discovery process.
     * This can be updated during runtime since the discovery reach of a
     * component node can depend on state of the host system.
     */
    struct noc_s3_skip_component_discovery_list *skip_discovery_list;
    /*
     * Address space offset for the current chip. This is typically updated
     * during the boot time by reading the current chip id (in case of RSE
     * present in a multichip platform). The offset will be added to the region
     * base and end address if the component enables \ref add_chip_addr_offset
     * flag.
     */
    uint64_t chip_addr_offset;
#ifdef NOC_S3_PRETTY_PRINT_LOG_ENABLED
    /* Function to fetch xSNI labels for log prints by passing the xSNI id */
    const char* (*get_xSNI_label)(uint64_t xSNI_id);
    /* Function to fetch xMNI labels for log prints by passing the xMNI id */
    const char* (*get_xMNI_label)(uint64_t xMNI_id);
#endif
};

#ifdef NOC_S3_PRETTY_PRINT_LOG_ENABLED
/**
 * \brief NoC S3 device data structure for a subfeature
 */
struct noc_s3_dev_data {
    /* Pointer to the NoC S3 device where the subfeature exists */
    struct noc_s3_dev const* noc_s3_dev;
    /* Pointer to the component node where the subfeature exists */
    struct noc_s3_component_node const* component;
};
#endif

#endif /* __NOC_S3_DRV_H__ */
