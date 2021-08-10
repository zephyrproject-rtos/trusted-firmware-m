/*
 * Copyright (c) 2017-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "region.h"
#include "spm_ipc.h"
#include "svc_num.h"
#include "tfm_api.h"
#include "tfm_arch.h"
#include "tfm_core_trustzone.h"
#include "tfm_core_utils.h"
#include "tfm_svcalls.h"
#include "utilities.h"
#include "load/spm_load_api.h"
#include "ffm/tfm_boot_data.h"
#include "ffm/psa_api.h"
#include "tfm_hal_isolation.h"
#include "tfm_hal_spm_logdev.h"
#include "load/partition_defs.h"
#include "psa/client.h"

/* MSP bottom (higher address) */
REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Limit);

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
    switch (svc_num) {
    case TFM_SVC_PSA_FRAMEWORK_VERSION:
        return tfm_spm_client_psa_framework_version();
    case TFM_SVC_PSA_VERSION:
        return tfm_spm_client_psa_version(ctx[0]);
    case TFM_SVC_PSA_CONNECT:
        return tfm_spm_client_psa_connect(ctx[0], ctx[1]);
    case TFM_SVC_PSA_CALL:
        return tfm_spm_client_psa_call((psa_handle_t)ctx[0], ctx[1],
                                       (const psa_invec *)ctx[2],
                                       (psa_outvec *)ctx[3]);
    case TFM_SVC_PSA_CLOSE:
        tfm_spm_client_psa_close((psa_handle_t)ctx[0]);
        break;
    case TFM_SVC_PSA_WAIT:
        return tfm_spm_partition_psa_wait((psa_signal_t)ctx[0], ctx[1]);
    case TFM_SVC_PSA_GET:
        return tfm_spm_partition_psa_get((psa_signal_t)ctx[0],
                                         (psa_msg_t *)ctx[1]);
    case TFM_SVC_PSA_SET_RHANDLE:
        tfm_spm_partition_psa_set_rhandle((psa_handle_t)ctx[0], (void *)ctx[1]);
        break;
    case TFM_SVC_PSA_READ:
        return tfm_spm_partition_psa_read((psa_handle_t)ctx[0], ctx[1],
                                          (void *)ctx[2], (size_t)ctx[3]);
    case TFM_SVC_PSA_SKIP:
        return tfm_spm_partition_psa_skip((psa_handle_t)ctx[0], ctx[1],
                                          (size_t)ctx[2]);
    case TFM_SVC_PSA_WRITE:
        tfm_spm_partition_psa_write((psa_handle_t)ctx[0], ctx[1],
                                    (void *)ctx[2], (size_t)ctx[3]);
        break;
    case TFM_SVC_PSA_REPLY:
        tfm_spm_partition_psa_reply((psa_handle_t)ctx[0], (psa_status_t)ctx[1]);
        break;
    case TFM_SVC_PSA_NOTIFY:
        tfm_spm_partition_psa_notify((int32_t)ctx[0]);
        break;
    case TFM_SVC_PSA_CLEAR:
        tfm_spm_partition_psa_clear();
        break;
    case TFM_SVC_PSA_EOI:
        tfm_spm_partition_psa_eoi((psa_signal_t)ctx[0]);
        break;
    case TFM_SVC_PSA_PANIC:
        tfm_spm_partition_psa_panic();
        break;
    case TFM_SVC_PSA_LIFECYCLE:
        return tfm_spm_get_lifecycle_state();
#if TFM_SP_LOG_RAW_ENABLED
    case TFM_SVC_OUTPUT_UNPRIV_STRING:
        return tfm_hal_output_spm_log((const char *)ctx[0], ctx[1]);
#endif
    case TFM_SVC_PSA_IRQ_ENABLE:
        tfm_spm_partition_irq_enable((psa_signal_t)ctx[0]);
        break;
    case TFM_SVC_PSA_IRQ_DISABLE:
        return tfm_spm_partition_irq_disable((psa_signal_t)ctx[0]);
    case TFM_SVC_PSA_RESET_SIGNAL:
        tfm_spm_partition_psa_reset_signal((psa_signal_t)ctx[0]);
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
        if (curr_sp->p_boundaries != irq_sp->p_boundaries) {
            tfm_hal_update_boundaries(irq_sp->p_ldinf,
                                      irq_sp->p_boundaries);
        }
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
                (uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK, $$ZI$$Limit);

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
        if (prev_sp->p_boundaries != curr_sp->p_boundaries) {
            tfm_hal_update_boundaries(prev_sp->p_ldinf,
                                      prev_sp->p_boundaries);
        }
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
        /* The following call does not return */
        tfm_arch_free_msp_and_exc_ret(exc_return);
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
        if (((uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK, $$ZI$$Limit)
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
