/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NI_TOWER_LIB_H__
#define __NI_TOWER_LIB_H__

#include "ni_tower_rse_drv.h"

#include <stdint.h>

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

/* Interface ID of xSNI components - completer interfaces */
enum sysctrl_xSNI_ids {
    /* Request from AP */
    SYSCTRL_APP_ASNI_ID = 0x0,
    /* Request from LCP */
    SYSCTRL_LCP_ASNI_ID,
    /* Request from MCP ATU */
    SYSCTRL_MCP_ASNI_ID,
    /* Request from RSE ATU */
    SYSCTRL_RSE_MAIN_ASNI_ID,
    /* Request from RSE and SCP targeting LCP address space */
    SYSCTRL_RSE_SCP_ASNI_ID,
    /* Request from SCP ATU */
    SYSCTRL_SCP_ASNI_ID
};

/* Interface ID of xMNI components - requester interfaces */
enum sysctrl_xMNI_ids {
    /* Targets AP address space */
    SYSCTRL_APP_AMNI_ID = 0x0,
    /* Targets memory map shared between AP and MCP */
    SYSCTRL_APP_MCP_AMNI_ID,
    /* Targets memory map shared between AP and SCP */
    SYSCTRL_APP_SCP_AMNI_ID,
    /* Targets LCP address space */
    SYSCTRL_LCP_AMNI_ID,
    /* Targets memory map shared between LCP and SCP */
    SYSCTRL_LCP_SCP_AMNI_ID,
    /* Targets Shared SRAM between RSE, SCP and MCP */
    SYSCTRL_RSM_AMNI_ID,
    /* Targets MCP address space */
    SYSCTRL_RSE_MCP_AMNI_ID,
    /* Targets SCP address space */
    SYSCTRL_RSE_SCP_AMNI_ID,
    /* Targets CMN GPV registers */
    SYSCTRL_CMN_PMNI_ID,
    /* Targets Shared RAM between RSE/SCP/MCP */
    SYSCTRL_RSM_PMNI_ID,
    /* Targets SYSCTRL SMMU registers */
    SYSCTRL_TCU_PMNI_ID,
    /* Targets the System Control NI-Tower registers (default target) */
    SYSCTRL_CONFIG_SPACE_ID = 0xF
};

#endif /* __NI_TOWER_LIB_H__ */
