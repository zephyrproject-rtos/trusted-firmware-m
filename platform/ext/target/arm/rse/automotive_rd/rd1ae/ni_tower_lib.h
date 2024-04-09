/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NI_TOWER_LIB_H__
#define __NI_TOWER_LIB_H__

#include "ni_tower_rse_drv.h"

#include "array.h"
#include <stdint.h>

/* Interface ID of xSNI components - completer interfaces */
enum sysctrl_xSNI_ids {
    /* Request from AP */
    SYSCTRL_APP_ASNI_ID = 0x0,
    /* Request from RSE ATU */
    SYSCTRL_RSE_MAIN_ASNI_ID = 0x3,
    /* Request from RSE and SCP targeting Cluster Utility address space */
    SYSCTRL_RSE_SCP_ASNI_ID = 0x4,
    /* Request from SCP ATU */
    SYSCTRL_SCP_ASNI_ID = 0x5,
};

/* Interface ID of xMNI components - requester interfaces */
enum sysctrl_xMNI_ids {
    /* Targets AP address space */
    SYSCTRL_APP_AMNI_ID = 0x0,
    /* Targets memory map shared between AP and SCP */
    SYSCTRL_APP_SCP_AMNI_ID = 0x2,
    /* Targets Cluster Utility address space */
    SYSCTRL_CLUS_UTIL_AMNI_ID = 0x3,
    /* Targets Shared SRAM between RSE and SCP */
    SYSCTRL_RSM_AMNI_ID = 0x5,
    /* Targets SI address space */
    SYSCTRL_RSE_SI_AMNI_ID = 0x6,
    /* Targets SCP address space */
    SYSCTRL_RSE_SCP_AMNI_ID = 0x7,
    /* Targets CMN GPV registers */
    SYSCTRL_CMN_PMNI_ID = 0x8,
    /* Targets Shared RAM between RSE and SCP */
    SYSCTRL_RSM_PMNI_ID = 0x9,
    /* Targets SYSCTRL SMMU registers */
    SYSCTRL_TCU_PMNI_ID = 0xA,
    /* Targets the System Control NI-Tower registers (default target) */
    SYSCTRL_CONFIG_SPACE_ID = 0xF
};

#endif /* __NI_TOWER_LIB_H__ */
