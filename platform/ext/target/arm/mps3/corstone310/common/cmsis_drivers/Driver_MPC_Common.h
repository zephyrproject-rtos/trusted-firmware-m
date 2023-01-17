/*
 * Copyright (c) 2016-2022 Arm Limited. All rights reserved.
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

#ifndef __DRIVER_MPC_COMMON_H__
#define __DRIVER_MPC_COMMON_H__

#define ARM_DRIVER_MPC(MPC_NAME, MPC_DEV) \
static int32_t MPC_NAME##_Initialize(void) \
{ \
    enum mpc_sie_error_t ret; \
 \
    ret = mpc_sie_init(&MPC_DEV); \
 \
    return error_trans(ret); \
} \
 \
static int32_t MPC_NAME##_Uninitialize(void) \
{ \
    /* Nothing to be done */ \
    return ARM_DRIVER_OK; \
} \
 \
static int32_t MPC_NAME##_GetBlockSize(uint32_t *blk_size) \
{ \
    enum mpc_sie_error_t ret; \
 \
    ret = mpc_sie_get_block_size(&MPC_DEV, blk_size); \
 \
    return error_trans(ret); \
} \
 \
static int32_t MPC_NAME##_GetCtrlConfig(uint32_t *ctrl_val) \
{ \
    enum mpc_sie_error_t ret; \
 \
    ret = mpc_sie_get_ctrl(&MPC_DEV, ctrl_val); \
 \
    return error_trans(ret); \
} \
 \
static int32_t MPC_NAME##_SetCtrlConfig(uint32_t ctrl) \
{ \
    enum mpc_sie_error_t ret; \
 \
    ret = mpc_sie_set_ctrl(&MPC_DEV, ctrl); \
 \
    return error_trans(ret); \
} \
 \
static int32_t MPC_NAME##_GetRegionConfig(uintptr_t base, \
                                          uintptr_t limit, \
                                          ARM_MPC_SEC_ATTR *attr) \
{ \
    enum mpc_sie_error_t ret; \
 \
    ret = mpc_sie_get_region_config(&MPC_DEV, base, limit, \
                                    (enum mpc_sie_sec_attr_t*)attr); \
 \
    return error_trans(ret); \
} \
 \
static int32_t MPC_NAME##_ConfigRegion(uintptr_t base, \
                                       uintptr_t limit, \
                                       ARM_MPC_SEC_ATTR attr) \
{ \
    enum mpc_sie_error_t ret; \
 \
    ret = mpc_sie_config_region(&MPC_DEV, base, limit, \
                                (enum mpc_sie_sec_attr_t)attr); \
 \
    return error_trans(ret); \
} \
 \
static int32_t MPC_NAME##_EnableInterrupt(void) \
{ \
    enum mpc_sie_error_t ret; \
 \
    ret = mpc_sie_irq_enable(&MPC_DEV); \
 \
    return error_trans(ret); \
} \
 \
static void MPC_NAME##_DisableInterrupt(void) \
{ \
    mpc_sie_irq_disable(&MPC_DEV); \
} \
 \
 \
static void MPC_NAME##_ClearInterrupt(void) \
{ \
    mpc_sie_clear_irq(&MPC_DEV); \
} \
 \
static uint32_t MPC_NAME##_InterruptState(void) \
{ \
    return mpc_sie_irq_state(&MPC_DEV); \
} \
 \
static int32_t MPC_NAME##_LockDown(void) \
{ \
    return mpc_sie_lock_down(&MPC_DEV); \
} \
 \
/* MPC_NAME Driver CMSIS access structure */ \
ARM_DRIVER_MPC Driver_##MPC_NAME = { \
    .GetVersion       = ARM_MPC_GetVersion, \
    .Initialize       = MPC_NAME##_Initialize, \
    .Uninitialize     = MPC_NAME##_Uninitialize, \
    .GetBlockSize     = MPC_NAME##_GetBlockSize, \
    .GetCtrlConfig    = MPC_NAME##_GetCtrlConfig, \
    .SetCtrlConfig    = MPC_NAME##_SetCtrlConfig, \
    .ConfigRegion     = MPC_NAME##_ConfigRegion, \
    .GetRegionConfig  = MPC_NAME##_GetRegionConfig, \
    .EnableInterrupt  = MPC_NAME##_EnableInterrupt, \
    .DisableInterrupt = MPC_NAME##_DisableInterrupt, \
    .ClearInterrupt   = MPC_NAME##_ClearInterrupt, \
    .InterruptState   = MPC_NAME##_InterruptState, \
    .LockDown         = MPC_NAME##_LockDown, \
};

#endif  /* __DRIVER_MPC_COMMON_H */
