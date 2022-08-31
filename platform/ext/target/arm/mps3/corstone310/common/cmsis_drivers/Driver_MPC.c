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
#include "Driver_MPC_Common.h"
#include "mpc_sie_drv.h"
#include "cmsis_driver_config.h"
#include "RTE_Device.h"

/* Driver version */
#define ARM_MPC_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 0)

#if (defined (RTE_SRAM_MPC) && (RTE_SRAM_MPC == 1)) || \
    (defined (RTE_ISRAM0_MPC) && (RTE_ISRAM0_MPC == 1)) || \
    (defined (RTE_ISRAM1_MPC) && (RTE_ISRAM1_MPC == 1)) || \
    (defined (RTE_QSPI_MPC) && (RTE_QSPI_MPC == 1)) || \
    (defined (RTE_DDR4_MPC) && (RTE_DDR4_MPC == 1))

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
    case MPC_SIE_UNSUPPORTED_HARDWARE_VERSION:
    case MPC_SIE_ERR_GATING_NOT_PRESENT:
    default:
        return ARM_MPC_ERR_UNSPECIFIED;
    }
}

#if (defined (RTE_SRAM_MPC) && (RTE_SRAM_MPC == 1))
ARM_DRIVER_MPC(SRAM_MPC, MPC_SRAM_DEV)
#endif /* RTE_SRAM_MPC */

#if (defined (RTE_ISRAM0_MPC) && (RTE_ISRAM0_MPC == 1))
ARM_DRIVER_MPC(ISRAM0_MPC, MPC_ISRAM0_DEV)
#endif /* RTE_ISRAM0_MPC */

#if (defined (RTE_ISRAM1_MPC) && (RTE_ISRAM1_MPC == 1))
ARM_DRIVER_MPC(ISRAM1_MPC, MPC_ISRAM1_DEV)
#endif /* RTE_ISRAM1_MPC */

#if (defined (RTE_QSPI_MPC) && (RTE_QSPI_MPC == 1))
ARM_DRIVER_MPC(QSPI_MPC, MPC_QSPI_DEV)
#endif /* RTE_QSPI_MPC */

#if (defined (RTE_DDR4_MPC) && (RTE_DDR4_MPC == 1))
ARM_DRIVER_MPC(DDR4_MPC, MPC_DDR4_DEV)
#endif /* RTE_DDR4_MPC */

#endif
