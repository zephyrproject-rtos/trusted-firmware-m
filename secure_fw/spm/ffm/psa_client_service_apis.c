/*
 * Copyright (c) 2020-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "spm_psa_client_call.h"
#include "psa/lifecycle.h"
#include "spm_ipc.h"
#include "tfm_core_utils.h"
#include "tfm_hal_defs.h"
#include "tfm_hal_platform.h"
#include "ffm/spm_error_base.h"
#include "tfm_rpc.h"
#include "tfm_spm_hal.h"
#include "tfm_psa_call_param.h"
#include "load/irq_defs.h"
#include "load/partition_defs.h"
#include "load/service_defs.h"

/****** SVC-use only. SVC args unstacking for PSA Client APIs ******/

uint32_t tfm_spm_get_lifecycle_state(void)
{
    /*
     * FixMe: return PSA_LIFECYCLE_UNKNOWN to the caller directly. It will be
     * implemented in the future.
     */
    return PSA_LIFECYCLE_UNKNOWN;
}

uint32_t tfm_spm_psa_framework_version(void)
{
    return tfm_spm_client_psa_framework_version();
}

uint32_t tfm_spm_psa_version(uint32_t *args, bool ns_caller)
{
    uint32_t sid;

    TFM_CORE_ASSERT(args != NULL);
    sid = (uint32_t)args[0];

    return tfm_spm_client_psa_version(sid, ns_caller);
}

psa_status_t tfm_spm_psa_connect(uint32_t *args, bool ns_caller)
{
    uint32_t sid;
    uint32_t version;

    TFM_CORE_ASSERT(args != NULL);
    sid = (uint32_t)args[0];
    version = (uint32_t)args[1];

    return tfm_spm_client_psa_connect(sid, version, ns_caller);
}

psa_status_t tfm_spm_psa_call(uint32_t *args, bool ns_caller, uint32_t lr)
{
    psa_handle_t handle;
    psa_invec *inptr;
    psa_outvec *outptr;
    size_t in_num, out_num;
    struct partition_t *partition = NULL;
    uint32_t privileged;
    int32_t type;

    TFM_CORE_ASSERT(args != NULL);
    handle = (psa_handle_t)args[0];

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }
    privileged = tfm_spm_partition_get_privileged_mode(
        partition->p_ldinf->flags);

    type = (int32_t)(int16_t)((args[1] & TYPE_MASK) >> TYPE_OFFSET);
    in_num = (size_t)((args[1] & IN_LEN_MASK) >> IN_LEN_OFFSET);
    out_num = (size_t)((args[1] & OUT_LEN_MASK) >> OUT_LEN_OFFSET);
    inptr = (psa_invec *)args[2];
    outptr = (psa_outvec *)args[3];

    /* The request type must be zero or positive. */
    if (type < 0) {
        TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_PROGRAMMER_ERROR);
    }

    return tfm_spm_client_psa_call(handle, type, inptr, in_num, outptr, out_num,
                                   ns_caller, privileged);
}

void tfm_spm_psa_close(uint32_t *args, bool ns_caller)
{
    psa_handle_t handle;

    TFM_CORE_ASSERT(args != NULL);
    handle = args[0];

    tfm_spm_client_psa_close(handle, ns_caller);
}

/****** SVC-use only. SVC args unstacking for PSA Partition APIs ******/

psa_signal_t tfm_spm_psa_wait(uint32_t *args)
{
    psa_signal_t signal_mask;
    uint32_t timeout;

    TFM_CORE_ASSERT(args != NULL);
    signal_mask = (psa_signal_t)args[0];
    timeout = args[1];

    return tfm_spm_partition_psa_wait(signal_mask, timeout);
}

psa_status_t tfm_spm_psa_get(uint32_t *args)
{
    psa_signal_t signal;
    psa_msg_t *msg = NULL;

    TFM_CORE_ASSERT(args != NULL);
    signal = (psa_signal_t)args[0];
    msg = (psa_msg_t *)args[1];

    return tfm_spm_partition_psa_get(signal, msg);
}

void tfm_spm_psa_set_rhandle(uint32_t *args)
{
    psa_handle_t msg_handle;
    void *rhandle = NULL;

    TFM_CORE_ASSERT(args != NULL);
    msg_handle = (psa_handle_t)args[0];
    rhandle = (void *)args[1];

    tfm_spm_partition_psa_set_rhandle(msg_handle, rhandle);
}

size_t tfm_spm_psa_read(uint32_t *args)
{
    psa_handle_t msg_handle;
    uint32_t invec_idx;
    void *buffer = NULL;
    size_t num_bytes;

    TFM_CORE_ASSERT(args != NULL);
    msg_handle = (psa_handle_t)args[0];
    invec_idx = args[1];
    buffer = (void *)args[2];
    num_bytes = (size_t)args[3];

    return tfm_spm_partition_psa_read(msg_handle, invec_idx, buffer, num_bytes);
}

size_t tfm_spm_psa_skip(uint32_t *args)
{
    psa_handle_t msg_handle;
    uint32_t invec_idx;
    size_t num_bytes;

    TFM_CORE_ASSERT(args != NULL);
    msg_handle = (psa_handle_t)args[0];
    invec_idx = args[1];
    num_bytes = (size_t)args[2];

    return tfm_spm_partition_psa_skip(msg_handle, invec_idx, num_bytes);
}

void tfm_spm_psa_write(uint32_t *args)
{
    psa_handle_t msg_handle;
    uint32_t outvec_idx;
    void *buffer = NULL;
    size_t num_bytes;

    TFM_CORE_ASSERT(args != NULL);
    msg_handle = (psa_handle_t)args[0];
    outvec_idx = args[1];
    buffer = (void *)args[2];
    num_bytes = (size_t)args[3];

    tfm_spm_partition_psa_write(msg_handle, outvec_idx, buffer, num_bytes);
}

void tfm_spm_psa_reply(uint32_t *args)
{
    psa_handle_t msg_handle;
    psa_status_t status;

    TFM_CORE_ASSERT(args != NULL);
    msg_handle = (psa_handle_t)args[0];
    status = (psa_status_t)args[1];

    tfm_spm_partition_psa_reply(msg_handle, status);
}

void tfm_spm_psa_notify(uint32_t *args)
{
    int32_t partition_id;

    TFM_CORE_ASSERT(args != NULL);
    partition_id = (int32_t)args[0];

    tfm_spm_partition_psa_notify(partition_id);
}

void tfm_spm_psa_clear(void)
{
    tfm_spm_partition_psa_clear();
}

void tfm_spm_psa_eoi(uint32_t *args)
{
    psa_signal_t irq_signal;

    TFM_CORE_ASSERT(args != NULL);
    irq_signal = (psa_signal_t)args[0];

    tfm_spm_partition_psa_eoi(irq_signal);
}

void tfm_spm_psa_panic(void)
{
    tfm_spm_partition_psa_panic();
}

void tfm_spm_irq_enable(uint32_t *args)
{
    psa_signal_t irq_signal;

    TFM_CORE_ASSERT(args != NULL);
    irq_signal = (psa_signal_t)args[0];

    tfm_spm_partition_irq_enable(irq_signal);
}

psa_irq_status_t tfm_spm_irq_disable(uint32_t *args)
{
    psa_signal_t irq_signal;

    TFM_CORE_ASSERT(args != NULL);
    irq_signal = (psa_signal_t)args[0];

    return tfm_spm_partition_irq_disable(irq_signal);
}

void tfm_spm_psa_reset_signal(uint32_t *args)
{
    psa_signal_t irq_signal;

    if (!args) {
        tfm_core_panic();
    }

    irq_signal = (psa_signal_t)args[0];

    tfm_spm_partition_psa_reset_signal(irq_signal);
}
