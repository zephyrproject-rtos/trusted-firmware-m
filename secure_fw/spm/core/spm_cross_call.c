/*
 * Copyright (c) 2021-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "config_impl.h"
#include "compiler_ext_defs.h"
#include "current.h"
#include "spm.h"
#include "tfm_arch.h"
#include "ffm/backend.h"
#include "ffm/psa_api.h"
#include "internal_status_code.h"
#include "aapcs_local.h"

__used
uint64_t cross_call_entering_c(void)
{
    struct partition_t *caller = GET_CURRENT_COMPONENT();
    uint32_t sp = 0, sp_limit = 0;
    AAPCS_DUAL_U32_T spm_stack_info;

    /* PSA APIs must be called from Thread mode */
    if (__get_active_exc_num() != EXC_NUM_THREAD_MODE) {
        tfm_core_panic();
    }

    /*
     * Check if caller stack is within SPM stack. If not, then stack needs to
     * switch.
     */
    if ((caller->ctx_ctrl.sp <= SPM_THREAD_CONTEXT->sp_limit) ||
        (caller->ctx_ctrl.sp >  SPM_THREAD_CONTEXT->sp_base)) {
        sp       = SPM_THREAD_CONTEXT->sp;
        sp_limit = SPM_THREAD_CONTEXT->sp_limit;
    }

    AAPCS_DUAL_U32_SET(spm_stack_info, sp, sp_limit);

    arch_acquire_sched_lock();

    return AAPCS_DUAL_U32_AS_U64(spm_stack_info);
}

__used
psa_status_t cross_call_exiting_c(psa_status_t status)
{
    uint32_t sched_attempted;

    spm_handle_programmer_errors(status);

    /* Release scheduler lock and check the record of schedule attempt. */
    sched_attempted = arch_release_sched_lock();

    /* Interrupt is masked, PendSV will not happen immediately. */
    if (status == STATUS_NEED_SCHEDULE ||
        sched_attempted == SCHEDULER_ATTEMPTED) {
        tfm_arch_trigger_pendsv();
    }

    return status;
}
