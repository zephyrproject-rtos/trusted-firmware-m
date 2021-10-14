/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_platform_system.h"
#include "platform_description.h"
#include "corstone1000_ioctl_requests.h"
#include "fwu_agent.h"

void tfm_platform_hal_system_reset(void)
{
    /* Reset the system */
    NVIC_SystemReset();
}

enum tfm_platform_err_t tfm_platform_hal_ioctl(tfm_platform_ioctl_req_t request,
                                               psa_invec  *in_vec,
                                               psa_outvec *out_vec)
{
    int32_t ret = TFM_PLATFORM_ERR_SUCCESS;
    corstone1000_ioctl_in_params_t *in_params =
                                    (corstone1000_ioctl_in_params_t *)in_vec->base;
    corstone1000_ioctl_out_params_t *out_params = (corstone1000_ioctl_out_params_t *)out_vec->base;

    switch(in_params->ioctl_id) {
        case IOCTL_CORSTONE1000_FWU_FLASH_IMAGES:
            out_params->result = corstone1000_fwu_flash_image();
            if (!out_params->result) {
                NVIC_SystemReset();
            }
            break;
        default:
            ret = TFM_PLATFORM_ERR_NOT_SUPPORTED;
            break;
    }

    return ret;
}
