/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <inttypes.h>
#include "psa/client.h"
#include "svc_num.h"
#include "tfm_api.h"
#include "tfm_hal_device_header.h"
#include "tfm_psa_call_param.h"

__attribute__((naked))
uint32_t psa_framework_version(void)
{
    __ASM volatile("SVC %0           \n"
                   "BX LR            \n"
                   : : "I" (TFM_SVC_PSA_FRAMEWORK_VERSION));
}

__attribute__((naked))
uint32_t psa_version(uint32_t sid)
{
    __ASM volatile("SVC %0           \n"
                   "BX LR            \n"
                   : : "I" (TFM_SVC_PSA_VERSION));
}

__attribute__((naked))
psa_handle_t psa_connect(uint32_t sid, uint32_t version)
{
    __ASM volatile("SVC %0           \n"
                   "BX LR            \n"
                   : : "I" (TFM_SVC_PSA_CONNECT));
}

__attribute__((naked))
static psa_status_t psa_call_param_pack(psa_handle_t handle,
                                        uint32_t ctrl_param,
                                        const psa_invec *in_vec,
                                        psa_outvec *out_vec)
{
    __ASM volatile("SVC %0           \n"
                   "BX LR            \n"
                   : : "I" (TFM_SVC_PSA_CALL));
}

psa_status_t psa_call(psa_handle_t handle,
                      int32_t type,
                      const psa_invec *in_vec,
                      size_t in_len,
                      psa_outvec *out_vec,
                      size_t out_len)
{
    if ((type > INT16_MAX) ||
        (type < INT16_MIN) ||
        (in_len > UINT8_MAX) ||
        (out_len > UINT8_MAX)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    return psa_call_param_pack(handle,
                               PARAM_PACK(type, in_len, out_len),
                               in_vec,
                               out_vec);
}

__attribute__((naked))
void psa_close(psa_handle_t handle)
{
    __ASM volatile("SVC %0           \n"
                   "BX LR            \n"
                   : : "I" (TFM_SVC_PSA_CLOSE));
}
