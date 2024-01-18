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
};

#endif /* __NI_TOWER_DRV_H__ */
