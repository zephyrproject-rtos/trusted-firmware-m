/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 * Copyright 2026 NXP
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __TARGET_CFG_H__
#define __TARGET_CFG_H__

#include "target_cfg_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*${macro:start}*/
#define TFM_DRIVER_STDIO    Driver_LPUART
#define NS_DRIVER_STDIO     Driver_LPUART

/*MBC0 Configs*/
#define MBC_SLAVE_FLASH      0U /* 0=Flash, 1=IFR0, 2=IFR1 */
#define TRDC_ACCESS_CONTROL_POLICY_SEC_PRIV_INDEX    0
#define TRDC_ACCESS_CONTROL_POLICY_SEC_INDEX         1
#define TRDC_ACCESS_CONTROL_POLICY_ALL_INDEX         2
#define TRDC_ACCESS_CONTROL_POLICY_NS_INDEX          4
/*${macro:end}*/

/**
 * \brief Holds the data necessary to do isolation for a specific peripheral.
 */
struct platform_data_t
{
    uint32_t periph_start;
    uint32_t periph_limit;
    volatile uint32_t *periph_ppc_bank; /* Secure control register address */
    uint32_t periph_ppc_loc;            /* Position in the secure control register */
};

void ppc_configure_to_secure(struct platform_data_t *platform_data, bool privileged);

#ifdef __cplusplus
}
#endif

#endif /* __TARGET_CFG_H__ */
