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

#include "Driver_MPC.h"

#include "mpc_sie_drv.h"
#include "cmsis_driver_config.h"
#include "RTE_Device.h"

/* Driver version */
#define ARM_MPC_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 0)

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_MPC_API_VERSION,
    ARM_MPC_DRV_VERSION
};

static ARM_DRIVER_VERSION ARM_MPC_GetVersion(void)
{
    return DriverVersion;
}

/*
 * \brief Translates error codes from native API to CMSIS API.
 *
 * \param[in] err  Error code to translate (\ref mpc_sie_error_t).
 *
 * \return Returns CMSIS error code.
 */
static int32_t error_trans(enum mpc_sie_error_t err)
{
    switch(err) {
    case MPC_SIE_ERR_NONE:
        return ARM_DRIVER_OK;
    case MPC_SIE_INVALID_ARG:
        return ARM_DRIVER_ERROR_PARAMETER;
    case MPC_SIE_NOT_INIT:
        return ARM_MPC_ERR_NOT_INIT;
    case MPC_SIE_ERR_NOT_IN_RANGE:
        return ARM_MPC_ERR_NOT_IN_RANGE;
    case MPC_SIE_ERR_NOT_ALIGNED:
        return ARM_MPC_ERR_NOT_ALIGNED;
    case MPC_SIE_ERR_INVALID_RANGE:
        return ARM_MPC_ERR_INVALID_RANGE;
    case MPC_SIE_ERR_RANGE_SEC_ATTR_NON_COMPATIBLE:
        return ARM_MPC_ERR_RANGE_SEC_ATTR_NON_COMPATIBLE;
    default:
        return ARM_MPC_ERR_UNSPECIFIED;
    }
}

#if (RTE_VM0_MPC)
/* Ranges controlled by this VM0_MPC */
static const struct mpc_sie_memory_range_t MPC_VM0_RANGE_S = {
    .base         = MPC_VM0_RANGE_BASE_S,
    .limit        = MPC_VM0_RANGE_LIMIT_S,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_SECURE
};

static const struct mpc_sie_memory_range_t MPC_VM0_RANGE_NS = {
    .base         = MPC_VM0_RANGE_BASE_NS,
    .limit        = MPC_VM0_RANGE_LIMIT_NS,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_NONSECURE
};

#define MPC_VM0_RANGE_LIST_LEN  2u
static const struct mpc_sie_memory_range_t*
    MPC_VM0_RANGE_LIST[MPC_VM0_RANGE_LIST_LEN] = {
        &MPC_VM0_RANGE_S,
        &MPC_VM0_RANGE_NS
    };

/* VM0_MPC Driver wrapper functions */
static int32_t VM0_MPC_Initialize(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_init(&MPC_VM0_DEV,
                       MPC_VM0_RANGE_LIST,
                       MPC_VM0_RANGE_LIST_LEN);

    return error_trans(ret);
}

static int32_t VM0_MPC_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t VM0_MPC_GetBlockSize(uint32_t *blk_size)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_block_size(&MPC_VM0_DEV, blk_size);

    return error_trans(ret);
}

static int32_t VM0_MPC_GetCtrlConfig(uint32_t *ctrl_val)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_ctrl(&MPC_VM0_DEV, ctrl_val);

    return error_trans(ret);
}

static int32_t VM0_MPC_SetCtrlConfig(uint32_t ctrl)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_set_ctrl(&MPC_VM0_DEV, ctrl);

    return error_trans(ret);
}

static int32_t VM0_MPC_GetRegionConfig(uintptr_t base,
                                          uintptr_t limit,
                                          ARM_MPC_SEC_ATTR *attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_region_config(&MPC_VM0_DEV, base, limit,
                                    (enum mpc_sie_sec_attr_t*)attr);

    return error_trans(ret);
}

static int32_t VM0_MPC_ConfigRegion(uintptr_t base,
                                       uintptr_t limit,
                                       ARM_MPC_SEC_ATTR attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_config_region(&MPC_VM0_DEV, base, limit,
                                (enum mpc_sie_sec_attr_t)attr);

    return error_trans(ret);
}

static int32_t VM0_MPC_EnableInterrupt(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_irq_enable(&MPC_VM0_DEV);

    return error_trans(ret);
}

static void VM0_MPC_DisableInterrupt(void)
{
    mpc_sie_irq_disable(&MPC_VM0_DEV);
}


static void VM0_MPC_ClearInterrupt(void)
{
    mpc_sie_clear_irq(&MPC_VM0_DEV);
}

static uint32_t VM0_MPC_InterruptState(void)
{
    return mpc_sie_irq_state(&MPC_VM0_DEV);
}

static int32_t VM0_MPC_LockDown(void)
{
    return mpc_sie_lock_down(&MPC_VM0_DEV);
}

/* VM0_MPC Driver CMSIS access structure */
ARM_DRIVER_MPC Driver_VM0_MPC = {
    .GetVersion       = ARM_MPC_GetVersion,
    .Initialize       = VM0_MPC_Initialize,
    .Uninitialize     = VM0_MPC_Uninitialize,
    .GetBlockSize     = VM0_MPC_GetBlockSize,
    .GetCtrlConfig    = VM0_MPC_GetCtrlConfig,
    .SetCtrlConfig    = VM0_MPC_SetCtrlConfig,
    .ConfigRegion     = VM0_MPC_ConfigRegion,
    .GetRegionConfig  = VM0_MPC_GetRegionConfig,
    .EnableInterrupt  = VM0_MPC_EnableInterrupt,
    .DisableInterrupt = VM0_MPC_DisableInterrupt,
    .ClearInterrupt   = VM0_MPC_ClearInterrupt,
    .InterruptState   = VM0_MPC_InterruptState,
    .LockDown         = VM0_MPC_LockDown,
};
#endif /* RTE_VM0_MPC */

#if (RTE_VM1_MPC)
/* Ranges controlled by this VM1_MPC */
static const struct mpc_sie_memory_range_t MPC_VM1_RANGE_S = {
    .base         = MPC_VM1_RANGE_BASE_S,
    .limit        = MPC_VM1_RANGE_LIMIT_S,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_SECURE
};

static const struct mpc_sie_memory_range_t MPC_VM1_RANGE_NS = {
    .base         = MPC_VM1_RANGE_BASE_NS,
    .limit        = MPC_VM1_RANGE_LIMIT_NS,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_NONSECURE
};

#define MPC_VM1_RANGE_LIST_LEN  2u
static const struct mpc_sie_memory_range_t*
    MPC_VM1_RANGE_LIST[MPC_VM1_RANGE_LIST_LEN] = {
        &MPC_VM1_RANGE_S,
        &MPC_VM1_RANGE_NS
    };

/* VM1_MPC Driver wrapper functions */
static int32_t VM1_MPC_Initialize(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_init(&MPC_VM1_DEV,
                       MPC_VM1_RANGE_LIST,
                       MPC_VM1_RANGE_LIST_LEN);

    return error_trans(ret);
}

static int32_t VM1_MPC_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t VM1_MPC_GetBlockSize(uint32_t *blk_size)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_block_size(&MPC_VM1_DEV, blk_size);

    return error_trans(ret);
}

static int32_t VM1_MPC_GetCtrlConfig(uint32_t *ctrl_val)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_ctrl(&MPC_VM1_DEV, ctrl_val);

    return error_trans(ret);
}

static int32_t VM1_MPC_SetCtrlConfig(uint32_t ctrl)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_set_ctrl(&MPC_VM1_DEV, ctrl);

    return error_trans(ret);
}

static int32_t VM1_MPC_GetRegionConfig(uintptr_t base,
                                          uintptr_t limit,
                                          ARM_MPC_SEC_ATTR *attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_get_region_config(&MPC_VM1_DEV, base, limit,
                                    (enum mpc_sie_sec_attr_t*)attr);

    return error_trans(ret);
}

static int32_t VM1_MPC_ConfigRegion(uintptr_t base,
                                       uintptr_t limit,
                                       ARM_MPC_SEC_ATTR attr)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_config_region(&MPC_VM1_DEV, base, limit,
                                (enum mpc_sie_sec_attr_t)attr);

    return error_trans(ret);
}

static int32_t VM1_MPC_EnableInterrupt(void)
{
    enum mpc_sie_error_t ret;

    ret = mpc_sie_irq_enable(&MPC_VM1_DEV);

    return error_trans(ret);
}

static void VM1_MPC_DisableInterrupt(void)
{
    mpc_sie_irq_disable(&MPC_VM1_DEV);
}


static void VM1_MPC_ClearInterrupt(void)
{
    mpc_sie_clear_irq(&MPC_VM1_DEV);
}

static uint32_t VM1_MPC_InterruptState(void)
{
    return mpc_sie_irq_state(&MPC_VM1_DEV);
}

static int32_t VM1_MPC_LockDown(void)
{
    return mpc_sie_lock_down(&MPC_VM1_DEV);
}

/* VM1_MPC Driver CMSIS access structure */
ARM_DRIVER_MPC Driver_VM1_MPC = {
    .GetVersion       = ARM_MPC_GetVersion,
    .Initialize       = VM1_MPC_Initialize,
    .Uninitialize     = VM1_MPC_Uninitialize,
    .GetBlockSize     = VM1_MPC_GetBlockSize,
    .GetCtrlConfig    = VM1_MPC_GetCtrlConfig,
    .SetCtrlConfig    = VM1_MPC_SetCtrlConfig,
    .ConfigRegion     = VM1_MPC_ConfigRegion,
    .GetRegionConfig  = VM1_MPC_GetRegionConfig,
    .EnableInterrupt  = VM1_MPC_EnableInterrupt,
    .DisableInterrupt = VM1_MPC_DisableInterrupt,
    .ClearInterrupt   = VM1_MPC_ClearInterrupt,
    .InterruptState   = VM1_MPC_InterruptState,
    .LockDown         = VM1_MPC_LockDown,
};
#endif /* RTE_VM1_MPC */
