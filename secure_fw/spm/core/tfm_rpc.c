/*
 * Copyright (c) 2019-2024, Arm Limited. All rights reserved.
 * Copyright (c) 2023 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "config_impl.h"
#include "ffm/mailbox_agent_api.h"
#include "ffm/psa_api.h"
#include "tfm_rpc.h"
#include "private/assert.h"
#include "tfm_psa_call_pack.h"

uint32_t tfm_rpc_psa_framework_version(void)
{
    return tfm_spm_client_psa_framework_version();
}

uint32_t tfm_rpc_psa_version(uint32_t sid)
{
    return tfm_spm_client_psa_version(sid);
}

psa_status_t tfm_rpc_psa_call(psa_handle_t handle, uint32_t control,
                              const struct client_params_t *params,
                              const void *client_data_stateless)
{
    SPM_ASSERT(params != NULL);
    SPM_ASSERT(client_data_stateless != NULL);

    return agent_psa_call(handle, control, params, client_data_stateless);
}

/* Following PSA APIs are only needed by connection-based services */
#if CONFIG_TFM_CONNECTION_BASED_SERVICE_API == 1

psa_status_t tfm_rpc_psa_connect(uint32_t sid,
                                 uint32_t version,
                                 int32_t ns_client_id,
                                 const void *client_data)
{
    SPM_ASSERT(client_data != NULL);

    return agent_psa_connect(sid, version, ns_client_id, client_data);
}

psa_status_t tfm_rpc_psa_close(psa_handle_t handle, int32_t ns_client_id)
{
    return agent_psa_close(handle, ns_client_id);
}

#endif /* CONFIG_TFM_CONNECTION_BASED_SERVICE_API */
