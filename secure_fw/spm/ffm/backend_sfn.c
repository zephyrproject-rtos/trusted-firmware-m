/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "compiler_ext_defs.h"
#include "current.h"
#include "tfm_spm_hal.h"
#include "ffm/backend.h"
#include "load/partition_defs.h"
#include "load/service_defs.h"
#include "load/spm_load_api.h"
#include "psa/error.h"
#include "psa/service.h"
#include "spm_ipc.h"

/* SFN Partition state */
#define SFN_PARTITION_STATE_NOT_INITED        0
#define SFN_PARTITION_STATE_INITED            1

typedef psa_status_t (*service_fn_t)(psa_msg_t *msg);
typedef psa_status_t (*sfn_init_fn_t)(void);

/* Declare the global component list */
struct partition_head_t partition_listhead;

/* TODO: To be checked with the RPC design. */
static uintptr_t spm_stack_limit;
static uintptr_t spm_stack_base;

/*
 * Send message and wake up the SP who is waiting on message queue, block the
 * current component state and activate the next component.
 */
static psa_status_t sfn_messaging(struct service_t *service,
                                  struct tfm_msg_body_t *msg)
{
    struct partition_t *p_target;
    psa_status_t status;

    if (!msg || !service || !service->p_ldinf || !service->partition) {
        tfm_core_panic();
    }

    msg->sfn_magic = TFM_MSG_MAGIC_SFN;
    p_target = service->partition;
    p_target->p_msg = msg;

    SET_CURRENT_COMPONENT(p_target);

    if (p_target->state == SFN_PARTITION_STATE_NOT_INITED) {
        if (p_target->p_ldinf->entry != 0) {
            status = ((sfn_init_fn_t)p_target->p_ldinf->entry)();
            /* Negative value indicates errors. */
            if (status < PSA_SUCCESS) {
                tfm_core_panic();
            }
        }
        p_target->state = SFN_PARTITION_STATE_INITED;
    }

    status = ((service_fn_t)service->p_ldinf->sfn)(&msg->msg);

    return status;
}

static int32_t sfn_replying(struct tfm_msg_body_t *p_msg, int32_t status)
{
    SET_CURRENT_COMPONENT(p_msg->p_client);

    /*
     * Returning a value here is necessary, because 'psa_reply' is absent
     * for SFN clients, the 'reply' method is performed by SPM internally
     * when SFN case, to forward the 'status' to the caller.
     *
     * For example:
     *   'status' MAY contain a 'psa_handle_t' returned by SPM 'connect' and
     *   SPM needs to 'reply' it back to the caller. Treat 'psa_handle_t' value
     *   as SPM specific return value and represnent it as 'psa_status_t'.
     */
    return status;
}

/* Parameters are treated as assuredly */
void sfn_comp_init_assuredly(struct partition_t *p_pt, uint32_t service_set)
{
    const struct partition_load_info_t *p_pldi = p_pt->p_ldinf;

    p_pt->p_msg = NULL;
    p_pt->state = SFN_PARTITION_STATE_NOT_INITED;

    THRD_SYNC_INIT(&p_pt->waitobj);

    /*
     * Built-in partitions still have thread instances: NS Agent (TZ) and
     * IDLE partition, and NS Agent (TZ) needs to be specific cared here.
     */
    if (p_pldi->flags & PARTITION_MODEL_IPC) {
        THRD_INIT(&p_pt->thrd, &p_pt->ctx_ctrl,
                  TO_THREAD_PRIORITY(PARTITION_PRIORITY(p_pldi->flags)));

        thrd_start(&p_pt->thrd,
               POSITION_TO_ENTRY(p_pldi->entry, thrd_fn_t),
               (void *)tfm_spm_hal_get_ns_entry_point(),
               LOAD_ALLOCED_STACK_ADDR(p_pldi),
               LOAD_ALLOCED_STACK_ADDR(p_pldi) + p_pldi->stack_size);

        /* SPM reuses the ns agent stack, use once only at initialization. */
        if (p_pldi->pid == TFM_SP_NON_SECURE_ID) {
            spm_stack_limit = p_pt->ctx_ctrl.sp_limit;
            spm_stack_base = p_pt->ctx_ctrl.sp;
        }
    }

    (void)service_set;
}

static void spm_thread_fn(void *p)
{
    struct partition_t *p_part, *p_curr;

    p_curr = GET_CURRENT_COMPONENT();
    /* Call partition initialization routine one by one. */
    UNI_LIST_FOR_EACH(p_part, PARTITION_LIST_ADDR) {
        if (p_part->p_ldinf->flags & PARTITION_MODEL_IPC) {
            continue;
        }

        if (p_part->state == SFN_PARTITION_STATE_INITED) {
            continue;
        }

        SET_CURRENT_COMPONENT(p_part);

        if (p_part->p_ldinf->entry != 0) {
            if (((sfn_init_fn_t)p_part->p_ldinf->entry)() < PSA_SUCCESS) {
                tfm_core_panic();
            }
        }

        p_part->state = SFN_PARTITION_STATE_INITED;
    }

    SET_CURRENT_COMPONENT(p_curr);

    thrd_set_state(CURRENT_THREAD, THRD_STATE_DETACH);

    tfm_arch_trigger_pendsv();
}

uint32_t sfn_system_run(void)
{
    struct thread_t *p_thrd = (struct thread_t *)spm_stack_limit;
    struct context_ctrl_t *p_ctxctrl = (struct context_ctrl_t *)(p_thrd + 1);
    uintptr_t sp_limit = (((uintptr_t)(p_ctxctrl + 1)) + 7) & ~0x7;
    uintptr_t sp_base = spm_stack_base;

    THRD_INIT(p_thrd, p_ctxctrl, THRD_PRIOR_HIGHEST);
    thrd_start(p_thrd, spm_thread_fn, NULL, sp_limit, sp_base);

    return thrd_start_scheduler(&CURRENT_THREAD);
}

const struct backend_ops_t backend_instance = {
    .comp_init_assuredly = sfn_comp_init_assuredly,
    .system_run          = sfn_system_run,
    .messaging           = sfn_messaging,
    .replying            = sfn_replying
};
