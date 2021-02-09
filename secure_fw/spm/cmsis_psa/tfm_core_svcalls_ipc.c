/*
 * Copyright (c) 2017-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "load/partition_defs.h"
#include "region.h"
#include "spm_ipc.h"
#include "tfm_api.h"
#include "tfm_arch.h"
#include "tfm_core_trustzone.h"
#include "tfm_core_utils.h"
#include "tfm_svcalls.h"
#include "utilities.h"
#include "svc_num.h"
#include "ffm/tfm_boot_data.h"
#include "ffm/psa_client_service_apis.h"
#include "tfm_hal_spm_logdev.h"

/* The section names come from the scatter file */
REGION_DECLARE(Image$$, TFM_UNPRIV_CODE, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_UNPRIV_CODE, $$RO$$Limit);

/* MSP bottom (higher address) */
REGION_DECLARE(Image$$, ARM_LIB_STACK_MSP, $$ZI$$Limit);

#ifdef PLATFORM_SVC_HANDLERS
extern int32_t platform_svc_handlers(uint8_t svc_num,
                                     uint32_t *ctx, uint32_t lr);
#endif

struct tfm_svc_flih_ctx_t {
    uint32_t exc_ret;                      /* EXC_RETURN payload at SVC entry */
    uint32_t psp;                          /* PSP value at SVC entry */
    const struct partition_load_info_t *p_ldinf;/* Load info of IRQ Partition */
    uint32_t signal;                       /* The IRQ signal */
};

static int32_t SVC_Handler_IPC(uint8_t svc_num, uint32_t *ctx,
                               uint32_t lr)
{
    bool ns_caller = false;
    struct partition_t *partition = NULL;
    uint32_t veneer_base =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Base);
    uint32_t veneer_limit =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Limit);

    /*
     * The caller security attribute detection bases on LR of state context.
     * However, if SP calls PSA APIs based on its customized SVC, the LR may be
     * occupied by general purpose value while calling SVC.
     * Check if caller comes from non-secure: return address (ctx[6]) is belongs
     * to veneer section, and the bit0 of LR (ctx[5]) is zero.
     */
    if (ctx[6] >= veneer_base && ctx[6] < veneer_limit &&
        !(ctx[5] & TFM_VENEER_LR_BIT0_MASK)) {
        ns_caller = true;
    }

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }

    tfm_spm_validate_caller(partition, ctx, lr, ns_caller);

    switch (svc_num) {
    case TFM_SVC_PSA_FRAMEWORK_VERSION:
        return tfm_spm_psa_framework_version();
    case TFM_SVC_PSA_VERSION:
        return tfm_spm_psa_version(ctx, ns_caller);
    case TFM_SVC_PSA_CONNECT:
        return tfm_spm_psa_connect(ctx, ns_caller);
    case TFM_SVC_PSA_CALL:
        return tfm_spm_psa_call(ctx, ns_caller, lr);
    case TFM_SVC_PSA_CLOSE:
        tfm_spm_psa_close(ctx, ns_caller);
        break;
    case TFM_SVC_PSA_WAIT:
        return tfm_spm_psa_wait(ctx);
    case TFM_SVC_PSA_GET:
        return tfm_spm_psa_get(ctx);
    case TFM_SVC_PSA_SET_RHANDLE:
        tfm_spm_psa_set_rhandle(ctx);
        break;
    case TFM_SVC_PSA_READ:
        return tfm_spm_psa_read(ctx);
    case TFM_SVC_PSA_SKIP:
        return tfm_spm_psa_skip(ctx);
    case TFM_SVC_PSA_WRITE:
        tfm_spm_psa_write(ctx);
        break;
    case TFM_SVC_PSA_REPLY:
        tfm_spm_psa_reply(ctx);
        break;
    case TFM_SVC_PSA_NOTIFY:
        tfm_spm_psa_notify(ctx);
        break;
    case TFM_SVC_PSA_CLEAR:
        tfm_spm_psa_clear();
        break;
    case TFM_SVC_PSA_EOI:
        tfm_spm_psa_eoi(ctx);
        break;
    case TFM_SVC_PSA_PANIC:
        tfm_spm_psa_panic();
        break;
    case TFM_SVC_SPM_REQUEST:
        tfm_spm_request_handler((const struct tfm_state_context_t *)ctx);
        break;
    case TFM_SVC_PSA_LIFECYCLE:
        return tfm_spm_get_lifecycle_state();
#if (TFM_SPM_LOG_LEVEL > TFM_SPM_LOG_LEVEL_SILENCE)
    case TFM_SVC_OUTPUT_UNPRIV_STRING:
        return tfm_hal_output_spm_log((const char *)ctx[0], ctx[1]);
#endif
    case TFM_SVC_PSA_IRQ_ENABLE:
        tfm_spm_irq_enable(ctx);
        break;
    case TFM_SVC_PSA_IRQ_DISABLE:
        return tfm_spm_irq_disable(ctx);
    case TFM_SVC_PSA_RESET_SIGNAL:
        tfm_spm_psa_reset_signal(ctx);
        break;
    default:
#ifdef PLATFORM_SVC_HANDLERS
        return (platform_svc_handlers(svc_num, ctx, lr));
#else
        SPMLOG_ERRMSG("Unknown SVC number requested!\r\n");
        return PSA_ERROR_GENERIC_ERROR;
#endif
    }
    return PSA_SUCCESS;
}

extern void tfm_flih_func_return(psa_flih_result_t result);

/*
 * Prepare execution context for deprivileged FLIH functions
 * svc_args: IRQ owner Partition load info, flih_func, signal
 */
uint32_t tfm_flih_prepare_depriv_flih(uintptr_t ctx, uint32_t *svc_args)
{
    struct tfm_core_thread_t *irq_sp_thread;
    struct partition_t *irq_sp, *curr_sp;
    struct tfm_state_context_t *p_stat_ctx;
    struct tfm_svc_flih_ctx_t *flih_ctx;

    irq_sp = tfm_spm_get_partition_by_id(
                            ((struct partition_load_info_t *)svc_args[0])->pid);

    if (!irq_sp) {
        tfm_core_panic();
    }
    irq_sp_thread = &irq_sp->sp_thread;

    curr_sp = TO_CONTAINER(tfm_core_thrd_get_curr(),
                           struct partition_t,
                           sp_thread);

    flih_ctx = (struct tfm_svc_flih_ctx_t *)ctx;
    flih_ctx->signal = svc_args[2];
    flih_ctx->p_ldinf = curr_sp->p_ldinf;

    if (curr_sp == irq_sp) {
        /*
         * Either the Partition Thread or FLIH Function within the same
         * Partition was preempted.
         */
        p_stat_ctx = (struct tfm_state_context_t *)flih_ctx->psp;
    } else {
        p_stat_ctx = (struct tfm_state_context_t *)irq_sp_thread->arch_ctx.sp;
        tfm_core_thrd_set_curr(irq_sp_thread);
        tfm_set_up_isolation_boundary(irq_sp);
        tfm_arch_set_psplim(irq_sp_thread->stk_btm);
    }

    p_stat_ctx--;
    if ((uintptr_t)p_stat_ctx < irq_sp_thread->stk_btm) {
        tfm_core_panic();
    }
    spm_memset(p_stat_ctx, 0, sizeof(struct tfm_state_context_t));

    p_stat_ctx->ra = (uint32_t)svc_args[1];
    p_stat_ctx->lr = (uint32_t)tfm_flih_func_return;
    p_stat_ctx->xpsr = XPSR_T32;

    __set_PSP((uint32_t)p_stat_ctx);

    return EXC_RETURN_THREAD_S_PSP;
}

/* Go back to ISR from FLIH functions */
uint32_t tfm_flih_return_to_isr(uintptr_t ctx, psa_flih_result_t result)
{
    struct partition_t *curr_sp, *prev_sp;
    struct tfm_svc_flih_ctx_t *flih_ctx;
    uint32_t msp_top =
                (uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK_MSP, $$ZI$$Limit);

    /* Skip one tfm_svc_flih_ctx_t + 8 words (R4- R11) + seals (2 words) */
    flih_ctx = (struct tfm_svc_flih_ctx_t *)
              (ctx + sizeof(struct tfm_svc_flih_ctx_t) + 10 * sizeof(uint32_t));
    if ((uint32_t)flih_ctx > msp_top - TFM_STACK_SEALED_SIZE) {
        tfm_core_panic();
    }

    curr_sp = TO_CONTAINER(tfm_core_thrd_get_curr(),
                           struct partition_t,
                           sp_thread);

    prev_sp = tfm_spm_get_partition_by_id(flih_ctx->p_ldinf->pid);
    if (!prev_sp) {
        tfm_core_panic();
    }

    if (curr_sp != prev_sp) {
        tfm_set_up_isolation_boundary(prev_sp);
        tfm_core_thrd_set_curr(&(prev_sp->sp_thread));
        tfm_arch_set_psplim(prev_sp->sp_thread.stk_btm);
    }

    __set_PSP(flih_ctx->psp);

    if (result == PSA_FLIH_SIGNAL) {
        notify_with_signal(curr_sp->p_ldinf->pid, flih_ctx->signal);
    } else if (result != PSA_FLIH_NO_SIGNAL) {
        /*
         * Nothing needed to do for PSA_FLIH_NO_SIGNAL
         * But if the flih_result is invalid, should panic.
         */
        tfm_core_panic();
    }

    return flih_ctx->exc_ret;
}

uint32_t tfm_core_svc_handler(uint32_t *msp, uint32_t exc_return,
                              uint32_t *psp)
{
    uint8_t svc_number = TFM_SVC_PSA_FRAMEWORK_VERSION;
    uint32_t *svc_args = msp;
    uintptr_t flih_ctx =
                 (uintptr_t)((uint32_t)msp - sizeof(struct tfm_svc_flih_ctx_t));

    if ((exc_return & EXC_RETURN_MODE) && (exc_return & EXC_RETURN_SPSEL)) {
        /* Use PSP when both EXC_RETURN.MODE and EXC_RETURN.SPSEL are set */
        svc_args = psp;
    } else {
        svc_args = msp;
    }

    /*
     * Stack contains:
     * r0, r1, r2, r3, r12, r14 (lr), the return address and xPSR
     * First argument (r0) is svc_args[0]
     */
    if (is_return_secure_stack(exc_return)) {
        /* SV called directly from secure context. Check instruction for
         * svc_number
         */
        svc_number = ((uint8_t *)svc_args[6])[-2];
    } else {
        /* Secure SV executing with NS return.
         * NS cannot directly trigger S SVC so this should not happen. This is
         * an unrecoverable error.
         */
        tfm_core_panic();
    }

    if (!(exc_return & EXC_RETURN_MODE)
                                  != (svc_number > TFM_SVC_THREAD_NUMBER_END)) {
        tfm_core_panic();
    }

    switch (svc_number) {
    case TFM_SVC_SPM_INIT:
        tfm_arch_clear_fp_status();
        exc_return = tfm_spm_init();
        break;
    case TFM_SVC_GET_BOOT_DATA:
        tfm_core_get_boot_data_handler(svc_args);
        break;
    case TFM_SVC_PREPARE_DEPRIV_FLIH:
        exc_return = tfm_flih_prepare_depriv_flih(flih_ctx, svc_args);
        break;
    case TFM_SVC_FLIH_FUNC_RETURN:
        exc_return = tfm_flih_return_to_isr(flih_ctx, svc_args[0]);
        break;
    default:
        if (((uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK_MSP, $$ZI$$Limit)
                                     - (uint32_t)msp) > TFM_STACK_SEALED_SIZE) {
            /* The Main Stack has contents, not calling from Partition thread */
            tfm_core_panic();
        }
        svc_args[0] = SVC_Handler_IPC(svc_number, svc_args, exc_return);
        break;
    }

    return exc_return;
}

__attribute__ ((naked)) void tfm_core_handler_mode(void)
{
    __ASM volatile("SVC %0           \n"
                   "BX LR            \n"
                   : : "I" (TFM_SVC_SPM_INIT));
}

void tfm_access_violation_handler(void)
{
    while (1) {
        ;
    }
}
