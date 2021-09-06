/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "spm_ipc.h"
#include "tfm_rpc.h"
#include "tfm_spm_hal.h" /* To be checked */
#include "ffm/backend.h"
#include "load/partition_defs.h"
#include "load/service_defs.h"
#include "load/spm_load_api.h"
#include "psa/error.h"

/* Declare the global component list */
struct partition_head_t partition_listhead;

/*
 * Send message and wake up the SP who is waiting on message queue, block the
 * current thread and triggere scheduler.
 */
static psa_status_t ipc_messaging(struct service_t *service,
                                  struct tfm_msg_body_t *msg)
{
    struct partition_t *p_owner = NULL;
    psa_signal_t signal = 0;

    if (!msg || !service || !service->p_ldinf || !service->partition) {
        tfm_core_panic();
    }

    p_owner = service->partition;
    signal = service->p_ldinf->signal;

    /* Add message to partition message list tail */
    BI_LIST_INSERT_BEFORE(&p_owner->msg_list, &msg->msg_node);

    /* Messages put. Update signals */
    p_owner->signals_asserted |= signal;

    if (p_owner->signals_waiting & signal) {
        thrd_wake_up(&p_owner->waitobj,
                     (p_owner->signals_asserted & p_owner->signals_waiting));
        p_owner->signals_waiting &= ~signal;
    }

    /*
     * If it is a NS request via RPC, it is unnecessary to block current
     * thread.
     */

    if (!is_tfm_rpc_msg(msg)) {
        thrd_wait_on(&msg->ack_evnt, CURRENT_THREAD);
    }

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
    }

    thrd_start(&p_pt->thrd,
               POSITION_TO_ENTRY(p_pldi->entry, thrd_fn_t), p_param,
               LOAD_ALLOCED_STACK_ADDR(p_pldi),
               LOAD_ALLOCED_STACK_ADDR(p_pldi) + p_pldi->stack_size);
}

static uint32_t ipc_system_run(void)
{
    return thrd_start_scheduler(&CURRENT_THREAD);
}

const struct backend_ops_t backend_instance = {
    .comp_init_assuredly = ipc_comp_init_assuredly,
    .system_run          = ipc_system_run,
    .messaging           = ipc_messaging,
};
