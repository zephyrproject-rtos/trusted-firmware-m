/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "current.h"
#include "tfm_psa_call_pack.h"
#include "ffm/backend.h"
#include "ffm/psa_api.h"
#include "psa/client.h"

#ifdef CONFIG_TFM_PSA_API_SFN_CALL

uint32_t psa_framework_version_sfn(void)
{
    return tfm_spm_client_psa_framework_version();
}

uint32_t psa_version_sfn(uint32_t sid)
{
    return tfm_spm_client_psa_version(sid);
}

psa_handle_t psa_connect_sfn(uint32_t sid, uint32_t version)
{
    struct partition_t *p_client, *p_target;
    psa_status_t stat;

    p_client = GET_CURRENT_COMPONENT();

    stat = tfm_spm_client_psa_connect(sid, version);

    p_target = GET_CURRENT_COMPONENT();
    if (p_client == p_target) {
        /* There should be errors if the 'current' is not switched. */
        TFM_CORE_ASSERT(stat < PSA_SUCCESS);

        return stat;
    }

    return (psa_handle_t)tfm_spm_partition_psa_reply(
                                p_target->p_msg->msg.handle, stat);
}

psa_status_t psa_call_pack_sfn(psa_handle_t handle, uint32_t ctrl_param,
                               const psa_invec *in_vec, psa_outvec *out_vec)
{
    struct partition_t *p_client, *p_target;
    psa_status_t stat;

    p_client = GET_CURRENT_COMPONENT();

    stat = tfm_spm_client_psa_call(handle, ctrl_param, in_vec, out_vec);

    p_target = GET_CURRENT_COMPONENT();
    if (p_client == p_target) {
        /* There should be errors if the 'current' is not switched. */
        TFM_CORE_ASSERT(stat < PSA_SUCCESS);
        return stat;
    }

    return (psa_status_t)tfm_spm_partition_psa_reply(
                                p_target->p_msg->msg.handle, stat);
}

void psa_close_sfn(psa_handle_t handle)
{
    struct partition_t *p_client, *p_target;

    p_client = GET_CURRENT_COMPONENT();

    tfm_spm_client_psa_close(handle);

    p_target = GET_CURRENT_COMPONENT();
    if (p_client == p_target) {
        /* There should be problems if the 'current' is not switched. */
        return;
    }

    (void)tfm_spm_partition_psa_reply(p_target->p_msg->msg.handle, PSA_SUCCESS);
}

psa_signal_t psa_wait_sfn(psa_signal_t signal_mask, uint32_t timeout)
{
    /*
     * TODO:
     *   Need to trigger scheduler here, when 'psa_wait'
     *   is needed (interrupt case, e.g.).
     */
    return tfm_spm_partition_psa_wait(signal_mask, timeout);
}

size_t psa_read_sfn(psa_handle_t msg_handle, uint32_t invec_idx,
                    void *buffer, size_t num_bytes)
{
    return tfm_spm_partition_psa_read(msg_handle, invec_idx,
                                       buffer, num_bytes);
}

size_t psa_skip_sfn(psa_handle_t msg_handle, uint32_t invec_idx,
                    size_t num_bytes)
{
    return tfm_spm_partition_psa_skip(msg_handle, invec_idx, num_bytes);
}

void psa_write_sfn(psa_handle_t msg_handle, uint32_t outvec_idx,
                   const void *buffer, size_t num_bytes)
{
    tfm_spm_partition_psa_write(msg_handle, outvec_idx, buffer, num_bytes);
}

void psa_panic_sfn(void)
{
    tfm_spm_partition_psa_panic();
}

#endif /* CONFIG_TFM_PSA_API_SFN_CALL */
