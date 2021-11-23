/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "critical_section.h"
#include "compiler_ext_defs.h"
#include "spm_ipc.h"
#include "tfm_hal_isolation.h"
#include "tfm_rpc.h"
#include "tfm_spm_hal.h" /* To be checked */
#include "ffm/backend.h"
#include "utilities.h"
#include "load/partition_defs.h"
#include "load/service_defs.h"
#include "load/spm_load_api.h"
#include "psa/error.h"

/* Declare the global component list */
struct partition_head_t partition_listhead;

#ifdef CONFIG_TFM_PSA_API_THREAD_CALL

#ifdef TFM_MULTI_CORE_TOPOLOGY
/* TODO: To be checked when RPC design updates. */
static uint8_t spm_stack_local[CONFIG_TFM_SPM_THREAD_STACK_SIZE] __aligned(8);
struct context_ctrl_t spm_thread_context = {
    .sp       = (uint32_t)&spm_stack_local[CONFIG_TFM_SPM_THREAD_STACK_SIZE],
    .sp_limit = (uint32_t)spm_stack_local,
    .reserved = 0,
    .exc_ret  = 0,
};
struct context_ctrl_t *p_spm_thread_context = &spm_thread_context;
#else
struct context_ctrl_t *p_spm_thread_context;
#endif

#endif

/*
 * Send message and wake up the SP who is waiting on message queue, block the
 * current thread and trigger scheduler.
 */
static psa_status_t ipc_messaging(struct service_t *service,
                                  struct tfm_msg_body_t *msg)
{
    struct partition_t *p_owner = NULL;
    psa_signal_t signal = 0;
    struct critical_section_t cs_assert = CRITICAL_SECTION_STATIC_INIT;

    if (!msg || !service || !service->p_ldinf || !service->partition) {
        tfm_core_panic();
    }

    p_owner = service->partition;
    signal = service->p_ldinf->signal;

    CRITICAL_SECTION_ENTER(cs_assert);
    /* Add message to partition message list tail */
    BI_LIST_INSERT_BEFORE(&p_owner->msg_list, &msg->msg_node);

    /* Messages put. Update signals */
    p_owner->signals_asserted |= signal;

    if (p_owner->signals_waiting & signal) {
        thrd_wake_up(&p_owner->waitobj,
                     (p_owner->signals_asserted & p_owner->signals_waiting));
        p_owner->signals_waiting &= ~signal;
    }
    CRITICAL_SECTION_LEAVE(cs_assert);

    /*
     * If it is a NS request via RPC, it is unnecessary to block current
     * thread.
     */

    if (!is_tfm_rpc_msg(msg)) {
        thrd_wait_on(&msg->ack_evnt, CURRENT_THREAD);
    }

    return PSA_SUCCESS;
}

static int32_t ipc_replying(struct tfm_msg_body_t *p_msg, int32_t status)
{
    if (is_tfm_rpc_msg(p_msg)) {
        tfm_rpc_client_call_reply(p_msg, status);
    } else {
        thrd_wake_up(&p_msg->ack_evnt, status);
    }

    /*
     * 'psa_reply' exists in IPC model only and returns 'void'. Return
     * 'PSA_SUCCESS' here always since SPM does not forward the status
     * to the caller.
     */
    return PSA_SUCCESS;
}

/* Parameters are treated as assuredly */
static void ipc_comp_init_assuredly(struct partition_t *p_pt,
                                    uint32_t service_setting)
{
    const struct partition_load_info_t *p_pldi = p_pt->p_ldinf;
    void *p_param = NULL;

    p_pt->signals_allowed |= PSA_DOORBELL | service_setting;

    THRD_SYNC_INIT(&p_pt->waitobj);
    BI_LIST_INIT_NODE(&p_pt->msg_list);

    THRD_INIT(&p_pt->thrd, &p_pt->ctx_ctrl,
              TO_THREAD_PRIORITY(PARTITION_PRIORITY(p_pldi->flags)));

    if (p_pldi->pid == TFM_SP_NON_SECURE_ID) {
        p_param = (void *)tfm_spm_hal_get_ns_entry_point();

#ifdef CONFIG_TFM_PSA_API_THREAD_CALL
        SPM_THREAD_CONTEXT = &p_pt->ctx_ctrl;
#endif

    }

    thrd_start(&p_pt->thrd,
               POSITION_TO_ENTRY(p_pldi->entry, thrd_fn_t), p_param,
               LOAD_ALLOCED_STACK_ADDR(p_pldi),
               LOAD_ALLOCED_STACK_ADDR(p_pldi) + p_pldi->stack_size);
}

static uint32_t ipc_system_run(void)
{
    uint32_t control;
    struct partition_t *p_cur_pt;

    control = thrd_start_scheduler(&CURRENT_THREAD);

    p_cur_pt = TO_CONTAINER(CURRENT_THREAD->p_context_ctrl,
                            struct partition_t, ctx_ctrl);

    if (tfm_hal_update_boundaries(p_cur_pt->p_ldinf, p_cur_pt->p_boundaries)
            != TFM_HAL_SUCCESS) {
        tfm_core_panic();
    }

    return control;
}

const struct backend_ops_t backend_instance = {
    .comp_init_assuredly = ipc_comp_init_assuredly,
    .system_run          = ipc_system_run,
    .messaging           = ipc_messaging,
    .replying            = ipc_replying,
};
