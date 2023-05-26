/*
 * Copyright (c) 2023 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "ffm/agent_api.h"

#include "ffm/psa_api.h"

psa_status_t agent_psa_call(psa_handle_t handle, int32_t ctrl_param,
                            const struct client_vectors *vecs,
                            const struct client_params *params)
{
    (void)params;

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
