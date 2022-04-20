/*
 * Copyright (c) 2021-2022, Arm Limited. All rights reserved.
 * Copyright (c) 2022, Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "compiler_ext_defs.h"
#include "current.h"
#include "runtime_defs.h"
#include "tfm_hal_platform.h"
#include "ffm/backend.h"
#include "ffm/stack_watermark.h"
#include "load/partition_defs.h"
#include "load/service_defs.h"
#include "load/spm_load_api.h"
#include "psa/error.h"
#include "psa/service.h"
#include "spm_ipc.h"

/* SFN Partition state */
#define SFN_PARTITION_STATE_NOT_INITED        0
#define SFN_PARTITION_STATE_INITED            1

/* Declare the global component list */
struct partition_head_t partition_listhead;

/* Current running partition. */
struct partition_t *p_current_partition;

/*
 * Send message and wake up the SP who is waiting on message queue, block the
 * current component state and activate the next component.
 */
static psa_status_t sfn_messaging(struct service_t *service,
                                  struct conn_handle_t *handle)
{
    struct partition_t *p_target;
    psa_status_t status;

    if (!handle || !service || !service->p_ldinf || !service->partition) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    handle->sfn_magic = TFM_MSG_MAGIC_SFN;
    p_target = service->partition;
    p_target->p_handles = handle;

    SET_CURRENT_COMPONENT(p_target);

    if (p_target->state == SFN_PARTITION_STATE_NOT_INITED) {
        if (p_target->p_ldinf->entry != 0) {
            status = ((sfn_init_fn_t)p_target->p_ldinf->entry)();
            /* Negative value indicates errors. */
            if (status < PSA_SUCCESS) {
                return PSA_ERROR_PROGRAMMER_ERROR;
            }
        }
        p_target->state = SFN_PARTITION_STATE_INITED;
    }

    status = ((service_fn_t)service->p_ldinf->sfn)(&handle->msg);

    handle->status = TFM_HANDLE_STATUS_ACTIVE;

    return status;
}

static psa_status_t sfn_replying(struct conn_handle_t *handle, int32_t status)
{
    SET_CURRENT_COMPONENT(handle->p_client);

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

static void spm_thread_fn(void)
{
    struct partition_t *p_part, *p_curr;

    p_curr = GET_CURRENT_COMPONENT();
    /* Call partition initialization routine one by one. */
    UNI_LIST_FOREACH(p_part, PARTITION_LIST_ADDR, next) {
        if (IS_PARTITION_IPC_MODEL(p_part->p_ldinf)) {
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
}

/* Parameters are treated as assuredly */
void sfn_comp_init_assuredly(struct partition_t *p_pt, uint32_t service_set)
{
    const struct partition_load_info_t *p_pldi = p_pt->p_ldinf;
    struct context_ctrl_t ns_agent_ctrl;
    p_pt->p_handles = NULL;
    p_pt->state = SFN_PARTITION_STATE_NOT_INITED;

    watermark_stack(p_pt);

    /*
     * Built-in partitions still have thread instances: NS Agent (TZ) and
     * IDLE partition, and NS Agent (TZ) needs to be specific cared here.
     */
    if (p_pldi->pid == TFM_SP_NON_SECURE_ID) {
        ARCH_CTXCTRL_INIT(&ns_agent_ctrl,
                          LOAD_ALLOCED_STACK_ADDR(p_pldi),
                          p_pldi->stack_size);
        tfm_arch_init_context(&ns_agent_ctrl, (uintptr_t)spm_thread_fn,
                              NULL, p_pldi->entry);
        tfm_arch_refresh_hardware_context(&ns_agent_ctrl);
        SET_CURRENT_COMPONENT(p_pt);
    }
}

uint32_t sfn_system_run(void)
{
    return EXC_RETURN_THREAD_S_PSP;
}

static psa_signal_t sfn_wait(struct partition_t *p_pt, psa_signal_t signal_mask)
{
    while (!(p_pt->signals_asserted & signal_mask))
        ;

    return p_pt->signals_asserted & signal_mask;
}

static void sfn_wake_up(struct partition_t *p_pt)
{
    (void)p_pt;
}

const struct backend_ops_t backend_instance = {
    .comp_init_assuredly = sfn_comp_init_assuredly,
    .system_run          = sfn_system_run,
    .messaging           = sfn_messaging,
    .replying            = sfn_replying,
    .wait                = sfn_wait,
    .wake_up             = sfn_wake_up,
};
