/*
 * Copyright (c) 2019-2022 Arm Limited
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

#include "Driver_PPC.h"
#include "Driver_PPC_Common.h"
#include "Driver_Common.h"
#include "cmsis.h"
#include "cmsis_driver_config.h"
#include "RTE_Device.h"
#include "ppc_corstone310_drv.h"

/* Driver version */
#define ARM_PPC_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,0)

#if (defined (RTE_MAIN0_PPC_CORSTONE310) && (RTE_MAIN0_PPC_CORSTONE310 == 1)) || \
    (defined (RTE_MAIN_EXP0_PPC_CORSTONE310) && (RTE_MAIN_EXP0_PPC_CORSTONE310 == 1)) || \
    (defined (RTE_MAIN_EXP1_PPC_CORSTONE310) && (RTE_MAIN_EXP1_PPC_CORSTONE310 == 1)) || \
    (defined (RTE_MAIN_EXP2_PPC_CORSTONE310) && (RTE_MAIN_EXP2_PPC_CORSTONE310 == 1)) || \
    (defined (RTE_MAIN_EXP3_PPC_CORSTONE310) && (RTE_MAIN_EXP3_PPC_CORSTONE310 == 1)) || \
    (defined (RTE_PERIPH0_PPC_CORSTONE310) && (RTE_PERIPH0_PPC_CORSTONE310 == 1)) || \
    (defined (RTE_PERIPH1_PPC_CORSTONE310) && (RTE_PERIPH1_PPC_CORSTONE310 == 1)) || \
    (defined (RTE_PERIPH_EXP0_PPC_CORSTONE310) && (RTE_PERIPH_EXP0_PPC_CORSTONE310 == 1)) || \
    (defined (RTE_PERIPH_EXP1_PPC_CORSTONE310) && (RTE_PERIPH_EXP1_PPC_CORSTONE310 == 1)) || \
    (defined (RTE_PERIPH_EXP2_PPC_CORSTONE310) && (RTE_PERIPH_EXP2_PPC_CORSTONE310 == 1)) || \
    (defined (RTE_PERIPH_EXP3_PPC_CORSTONE310) && (RTE_PERIPH_EXP3_PPC_CORSTONE310 == 1))

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_PPC_API_VERSION,
    ARM_PPC_DRV_VERSION
};

static ARM_DRIVER_VERSION ARM_PPC_GetVersion(void)
{
    return DriverVersion;
}

/*
 * \brief Translates error codes from native API to CMSIS API.
 *
 * \param[in] err  Error code to translate (\ref ppc_corstone310_error_t).
 *
 * \return Returns CMSIS error code.
 */
static int32_t error_trans(enum ppc_corstone310_error_t err)
{
    switch(err) {
    case PPC_CORSTONE310_ERR_NONE:
        return ARM_DRIVER_OK;
    case PPC_CORSTONE310_ERR_INVALID_PARAM:
        return ARM_DRIVER_ERROR_PARAMETER;
    case PPC_CORSTONE310_ERR_NOT_INIT:
        return ARM_PPC_ERR_NOT_INIT;
    case PPC_CORSTONE310_ERR_NOT_PERMITTED:
        return ARM_PPC_ERR_NOT_PERMITTED;
    default:
        return ARM_PPC_ERR_UNSPECIFIED;
    }
}

#if (defined (RTE_MAIN0_PPC_CORSTONE310) && (RTE_MAIN0_PPC_CORSTONE310 == 1))
ARM_DRIVER_PPC(MAIN0_PPC_CORSTONE310, PPC_CORSTONE310_MAIN0_DEV)
#endif /* RTE_MAIN0 */

#if (defined (RTE_MAIN_EXP0_PPC_CORSTONE310) && (RTE_MAIN_EXP0_PPC_CORSTONE310 == 1))
ARM_DRIVER_PPC(MAIN_EXP0_PPC_CORSTONE310, PPC_CORSTONE310_MAIN_EXP0_DEV)
#endif /* RTE_MAIN_EXP0 */

#if (defined (RTE_MAIN_EXP1_PPC_CORSTONE310) && (RTE_MAIN_EXP1_PPC_CORSTONE310 == 1))
ARM_DRIVER_PPC(MAIN_EXP1_PPC_CORSTONE310, PPC_CORSTONE310_MAIN_EXP1_DEV)
#endif /* RTE_MAIN_EXP1 */

#if (defined (RTE_MAIN_EXP2_PPC_CORSTONE310) && (RTE_MAIN_EXP2_PPC_CORSTONE310 == 1))
ARM_DRIVER_PPC(MAIN_EXP2_PPC_CORSTONE310, PPC_CORSTONE310_MAIN_EXP2_DEV)
#endif /* RTE_MAIN_EXP2 */

#if (defined (RTE_MAIN_EXP3_PPC_CORSTONE310) && (RTE_MAIN_EXP3_PPC_CORSTONE310 == 1))
ARM_DRIVER_PPC(MAIN_EXP3_PPC_CORSTONE310, PPC_CORSTONE310_MAIN_EXP3_DEV)
#endif /* RTE_MAIN_EXP3 */

#if (defined (RTE_PERIPH0_PPC_CORSTONE310) && (RTE_PERIPH0_PPC_CORSTONE310 == 1))
ARM_DRIVER_PPC(PERIPH0_PPC_CORSTONE310, PPC_CORSTONE310_PERIPH0_DEV)
#endif /* RTE_PERIPH0 */

#if (defined (RTE_PERIPH1_PPC_CORSTONE310) && (RTE_PERIPH1_PPC_CORSTONE310 == 1))
ARM_DRIVER_PPC(PERIPH1_PPC_CORSTONE310, PPC_CORSTONE310_PERIPH1_DEV)
#endif /* RTE_PERIPH1 */

#if (defined (RTE_PERIPH_EXP0_PPC_CORSTONE310) && (RTE_PERIPH_EXP0_PPC_CORSTONE310 == 1))
ARM_DRIVER_PPC(PERIPH_EXP0_PPC_CORSTONE310, PPC_CORSTONE310_PERIPH_EXP0_DEV)
#endif /* RTE_PERIPH_EXP0 */

#if (defined (RTE_PERIPH_EXP1_PPC_CORSTONE310) && (RTE_PERIPH_EXP1_PPC_CORSTONE310 == 1))
ARM_DRIVER_PPC(PERIPH_EXP1_PPC_CORSTONE310, PPC_CORSTONE310_PERIPH_EXP1_DEV)
#endif /* RTE_PERIPH_EXP1 */

#if (defined (RTE_PERIPH_EXP2_PPC_CORSTONE310) && (RTE_PERIPH_EXP2_PPC_CORSTONE310 == 1))
ARM_DRIVER_PPC(PERIPH_EXP2_PPC_CORSTONE310, PPC_CORSTONE310_PERIPH_EXP2_DEV)
#endif /* RTE_PERIPH_EXP2 */

#if (defined (RTE_PERIPH_EXP3_PPC_CORSTONE310) && (RTE_PERIPH_EXP3_PPC_CORSTONE310 == 1))
ARM_DRIVER_PPC(PERIPH_EXP3_PPC_CORSTONE310, PPC_CORSTONE310_PERIPH_EXP3_DEV)
#endif /* RTE_PERIPH_EXP3 */

#endif
