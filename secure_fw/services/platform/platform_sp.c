/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "platform_sp.h"

#include "platform/include/tfm_platform_system.h"
#include "secure_fw/core/tfm_secure_api.h"
#include "secure_fw/include/tfm_spm_services_api.h"

enum tfm_platform_err_t platform_sp_init(void)
{
    /* Nothing to be done */

    return TFM_PLATFORM_ERR_SUCCESS;
}

enum tfm_platform_err_t platform_sp_system_reset(void)
{
    /* Check if SPM allows the system reset */

    if (tfm_spm_request_reset_vote() != 0) {
        return TFM_PLATFORM_ERR_SYSTEM_ERROR;
    }

    /* FIXME: The system reset functionality is only supported in isolation
     *        level 1.
     *        Currently, the mechanism by which PRoT services should run in
     *        privileged mode in level 3, it is not in place due to an ongoing
     *        work in TF-M Core. So, the NVIC_SystemReset call performed by the
     *        service, it is expected to generate a memory fault when it tries
     *        to access the SCB->AIRCR register in level 3 isolation.
     */

    tfm_platform_hal_system_reset();

    return TFM_PLATFORM_ERR_SUCCESS;
}
