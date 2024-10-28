/*
 * Copyright (c) 2023-2024 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file host_device_definition.c
 * \brief This file defines exports the structures based on the peripheral
 * definitions from device_cfg.h.
 */

#include "host_base_address.h"
#include "host_device_definition.h"
#include "platform_base_address.h"

#include <stddef.h>

#ifdef RD_SYSCTRL_NI_TOWER
#ifdef NI_TOWER_PRETTY_PRINT_LOG_ENABLED
const char* sysctrl_xSNI_labels[] = {
    [SYSCTRL_APP_ASNI_ID     ] = "SYSCTRL_APP_ASNI",
    [SYSCTRL_LCP_ASNI_ID     ] = "SYSCTRL_LCP_ASNI",
    [SYSCTRL_MCP_ASNI_ID     ] = "SYSCTRL_MCP_ASNI",
    [SYSCTRL_RSE_MAIN_ASNI_ID] = "SYSCTRL_RSE_MAIN_ASNI",
    [SYSCTRL_RSE_SCP_ASNI_ID ] = "SYSCTRL_RSE_SCP_ASNI",
    [SYSCTRL_SCP_ASNI_ID     ] = "SYSCTRL_SCP_ASNI",
};

#define SYSCTRL_CONFIG_SPACE_LABEL_IDX SYSCTRL_TCU_PMNI_ID + 1

const char* sysctrl_xMNI_labels[] = {
    [SYSCTRL_APP_AMNI_ID    ] = "SYSCTRL_APP_AMNI",
    [SYSCTRL_APP_MCP_AMNI_ID] = "SYSCTRL_APP_MCP_AMNI",
    [SYSCTRL_APP_SCP_AMNI_ID] = "SYSCTRL_APP_SCP_AMNI",
    [SYSCTRL_LCP_AMNI_ID    ] = "SYSCTRL_LCP_AMNI",
    [SYSCTRL_LCP_SCP_AMNI_ID] = "SYSCTRL_LCP_SCP_AMNI",
    [SYSCTRL_RSM_AMNI_ID    ] = "SYSCTRL_RSM_AMNI",
    [SYSCTRL_RSE_MCP_AMNI_ID] = "SYSCTRL_RSE_MCP_AMNI",
    [SYSCTRL_RSE_SCP_AMNI_ID] = "SYSCTRL_RSE_SCP_AMNI",
    [SYSCTRL_CMN_PMNI_ID    ] = "SYSCTRL_CMN_PMNI",
    [SYSCTRL_RSM_PMNI_ID    ] = "SYSCTRL_RSM_PMNI",
    [SYSCTRL_TCU_PMNI_ID    ] = "SYSCTRL_TCU_PMNI",
    [SYSCTRL_CONFIG_SPACE_LABEL_IDX] = "SYSCTRL_CONFIG_SPACE",
};

const char* get_sysctrl_xSNI_label(uint64_t xSNI_id)
{
    if (xSNI_id > SYSCTRL_SCP_ASNI_ID)
        return NULL;

    return sysctrl_xSNI_labels[xSNI_id];
}

const char* get_sysctrl_xMNI_label(uint64_t xMNI_id)
{
    if (xMNI_id == SYSCTRL_CONFIG_SPACE_ID)
        return sysctrl_xMNI_labels[SYSCTRL_CONFIG_SPACE_LABEL_IDX];

    if (xMNI_id > SYSCTRL_TCU_PMNI_ID)
        return NULL;

    return sysctrl_xMNI_labels[xMNI_id];
}
#endif

/* System Control NI-Tower device */
struct ni_tower_dev SYSCTRL_NI_TOWER_DEV = {
    .periphbase = HOST_NI_TOWER_BASE,
    .config_node_granularity = NI_TOWER_64KB_CONFIG_NODES,
    .skip_discovery_list = NULL,
    .chip_addr_offset = 0, /* Updated at boot time */
#ifdef NI_TOWER_PRETTY_PRINT_LOG_ENABLED
    .get_xSNI_label = get_sysctrl_xSNI_label,
    .get_xMNI_label = get_sysctrl_xMNI_label,
#endif
};
#endif

#ifdef RD_PERIPH_NI_TOWER
#ifdef NI_TOWER_PRETTY_PRINT_LOG_ENABLED
const char* periph_xMNI_labels[] = {
    [PERIPH_RAM_AMNI_ID        ] = "PERIPH_RAM_AMNI_ID",
    [PERIPH_ECCREG_PMNI_ID     ] = "PERIPH_ECCREG_PMNI_ID",
    [PERIPH_GTIMERCTRL_PMNI_ID ] = "PERIPH_GTIMERCTRL_PMNI_ID",
    [PERIPH_NSGENWDOG_PMNI_ID  ] = "PERIPH_NSGENWDOG_PMNI_ID",
    [PERIPH_NSGTIMER_PMNI_ID   ] = "PERIPH_NSGTIMER_PMNI_ID",
    [PERIPH_NSUART0_PMNI_ID    ] = "PERIPH_NSUART0_PMNI_ID",
    [PERIPH_NSUART1_PMNI_ID    ] = "PERIPH_NSUART1_PMNI_ID",
    [PERIPH_ROOTGENWDOG_PMNI_ID] = "PERIPH_ROOTGENWDOG_PMNI_ID",
    [PERIPH_SECGENWDOG_PMNI_ID ] = "PERIPH_SECGENWDOG_PMNI_ID",
    [PERIPH_SECGTIMER_PMNI_ID  ] = "PERIPH_SECGTIMER_PMNI_ID",
    [PERIPH_SECUART_PMNI_ID    ] = "PERIPH_SECUART_PMNI_ID",
};

const char* get_periph_xMNI_labels(uint64_t xMNI_id)
{
    if (xMNI_id > PERIPH_SECUART_PMNI_ID)
        return NULL;

    return periph_xMNI_labels[xMNI_id];
}
#endif

/* Peripheral NI-Tower device */
struct ni_tower_dev PERIPH_NI_TOWER_DEV = {
    .periphbase = HOST_NI_TOWER_BASE,
    .config_node_granularity = NI_TOWER_64KB_CONFIG_NODES,
    .skip_discovery_list = NULL,
    .chip_addr_offset = 0, /* Updated at boot time */
#ifdef NI_TOWER_PRETTY_PRINT_LOG_ENABLED
    .get_xSNI_label = NULL,
    .get_xMNI_label = get_periph_xMNI_labels,
#endif
};
#endif

#ifdef HOST_SCP
struct mscp_dev_t HOST_SCP_DEV = {
    .init_ctrl_base = HOST_SCP_INIT_CTRL_BASE_S,
};
#endif

#ifdef HOST_MCP
struct mscp_dev_t HOST_MCP_DEV = {
    .init_ctrl_base = HOST_MCP_INIT_CTRL_BASE_S,
};
#endif

#ifdef HOST_SMMU
#include "smmu_v3_drv.h"
struct smmu_dev_t HOST_SYSCTRL_SMMU_DEV = {
    .smmu_base = HOST_SYSCTRL_SMMU_BASE,
    .ack_timeout = SMMU_DEFAULT_ACK_TIMEOUT,
};
#endif
