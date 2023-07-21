/*
 * Copyright (c) 2023 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "ffm/agent_api.h"

#include "ffm/psa_api.h"
#include "psa/error.h"
#include "tfm_multi_core.h"
#include "tfm_psa_call_pack.h"

psa_status_t agent_psa_call(psa_handle_t handle, int32_t ctrl_param,
                            const struct client_vectors *vecs,
                            const struct client_params *params)
{
    size_t in_num = PARAM_UNPACK_IN_LEN(ctrl_param);
    size_t out_num = PARAM_UNPACK_OUT_LEN(ctrl_param);

    (void)params;

    /*
     * Validate the parameters.
     * SPM will reject overlapping vectors.
     * Note that the caller must pass a copy of the two vector arrays in its
     * own address space, so we don't need to check the arrays themselves or
     * make a copy to avoid TOCTOU attacks
     */
    for (int i = 0; i < in_num; i++) {
        const psa_invec invec = vecs->in_vec[i];

        if (tfm_has_access_to_region(invec.base,
                                     invec.len,
                                     MEM_CHECK_NONSECURE | MEM_CHECK_MPU_READ) != TFM_SUCCESS) {
            return PSA_ERROR_PROGRAMMER_ERROR;
        }
    }
    for (int i = 0; i < out_num; i++) {
        const psa_outvec outvec = vecs->out_vec[i];

        if (tfm_has_access_to_region(outvec.base,
                                     outvec.len,
                                     MEM_CHECK_NONSECURE | MEM_CHECK_MPU_READWRITE) != TFM_SUCCESS) {
            return PSA_ERROR_PROGRAMMER_ERROR;
        }
    }

    /* For now, just call the non-agent API */
    return tfm_spm_client_psa_call(handle, ctrl_param,
                                   vecs->in_vec, vecs->out_vec);
}

#if CONFIG_TFM_CONNECTION_BASED_SERVICE_API == 1

psa_handle_t agent_psa_connect(uint32_t sid, uint32_t version,
                               int32_t ns_client_id, const void *client_data)
{
    (void)ns_client_id;
    (void)client_data;

    return tfm_spm_client_psa_connect(sid, version);
}

#endif /* CONFIG_TFM_CONNECTION_BASED_SERVICE_API == 1 */
