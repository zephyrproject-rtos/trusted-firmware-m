/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PLATFORM_SYSTEM_H__
#define __TFM_PLATFORM_SYSTEM_H__
/**
 * \note The interfaces defined in this file must be implemented for each
 *       target.
 */

#include "tfm_plat_defs.h"
#include "psa_client.h"
#include "tfm_plat_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Resets the system.
 *
 * \details Requests a system reset to reset the MCU.
 */
TFM_LINK_SET_OBJECT_IN_PARTITION_SECTION("TFM_SP_PLATFORM")
void tfm_platform_hal_system_reset(void);

/*!
 * \brief Performs pin services of the platform
 *
 * \param[in]     in_vec     Pointer to in_vec array, which contains input
 *                           arguments for the pin service
 * \param[in]     num_invec  Number of elements in in_vec array
 * \param[in,out] out_vec    Pointer out_vec array, which contains output data
 *                           of the pin service
 * \param[in]     num_outvec Number of elements in out_vec array
 *
 * \return Returns values as specified by the \ref tfm_plat_err_t
 */
TFM_LINK_SET_OBJECT_IN_PARTITION_SECTION("TFM_SP_PLATFORM")
enum tfm_plat_err_t
tfm_platform_hal_pin_service(const psa_invec  *in_vec,  uint32_t num_invec,
                             const psa_outvec *out_vec, uint32_t num_outvec);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_PLATFORM_SYSTEM_H__ */
