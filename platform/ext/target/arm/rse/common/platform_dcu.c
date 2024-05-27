/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "platform_dcu.h"
#include <stdint.h>
#include "device_definition.h"
#include "lcm_drv.h"

static const uint32_t dcu_reg_lock_values[NUM_OF_DCU_REG] = {
        DCU_LOCK_REG0_VALUE,
        DCU_LOCK_REG1_VALUE,
        DCU_LOCK_REG2_VALUE,
        DCU_LOCK_REG3_VALUE
};

int32_t tfm_plat_apply_debug_permissions(uint16_t zone)
{
    uint32_t dcu_reg_values[NUM_OF_DCU_REG];

    /* TODO: Assign appropriate values based on selected zone */
    (void)zone;
    /* Below values provide same access as when platform is in development
       life cycle state */
    dcu_reg_values[0] = DCU_DEBUG_ENABLED_REG0_VALUE;
    dcu_reg_values[1] = DCU_DEBUG_ENABLED_REG1_VALUE;
    dcu_reg_values[2] = DCU_DEBUG_ENABLED_REG2_VALUE;
    dcu_reg_values[3] = DCU_DEBUG_ENABLED_REG3_VALUE;

    return lcm_dcu_set_enabled(&LCM_DEV_S, (uint8_t*)&dcu_reg_values[0]);
}

int32_t tfm_plat_lock_dcu(void)
{
    return lcm_dcu_set_locked(&LCM_DEV_S, (uint8_t*)&dcu_reg_lock_values[0]);
}
