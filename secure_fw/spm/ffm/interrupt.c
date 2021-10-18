/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "interrupt.h"

#include "bitops.h"
#include "spm_ipc.h"
#include "tfm_arch.h"
#include "tfm_hal_interrupt.h"
#include "tfm_hal_isolation.h"
#include "thread.h"
#include "utilities.h"

#include "load/spm_load_api.h"

__attribute__((naked))
static psa_flih_result_t tfm_flih_deprivileged_handling(void *p_pt,
                                                        uintptr_t fn_flih,
                                                        void *p_context_ctrl)
{
    __ASM volatile("SVC %0           \n"
                   "BX LR            \n"
                   : : "I" (TFM_SVC_PREPARE_DEPRIV_FLIH));
}

struct irq_load_info_t *get_irq_info_for_signal(
                                    const struct partition_load_info_t *p_ldinf,
                                    psa_signal_t signal)
{
    size_t i;
    struct irq_load_info_t *irq_info;

    if (!IS_ONLY_ONE_BIT_IN_UINT32(signal)) {
        return NULL;
    }

    irq_info = (struct irq_load_info_t *)LOAD_INFO_IRQ(p_ldinf);
    for (i = 0; i < p_ldinf->nirqs; i++) {
        if (irq_info[i].signal == signal) {
            return &irq_info[i];
        }
    }

    return NULL;
}

extern void tfm_flih_func_return(psa_flih_result_t result);

uint32_t tfm_flih_prepare_depriv_flih(uint32_t *svc_args)
{
    struct partition_t *p_curr_sp;
    struct partition_t *p_owner_sp = (struct partition_t *)svc_args[0];
    uintptr_t sp_limit, stack;
    struct context_ctrl_t flih_ctx_ctrl;

    /* Come too early before runtime setup, should not happen. */
    if (!CURRENT_THREAD) {
        tfm_core_panic();
    }

    p_curr_sp = GET_CTX_OWNER(CURRENT_THREAD->p_context_ctrl);
    sp_limit =
           ((struct context_ctrl_t *)p_owner_sp->thrd.p_context_ctrl)->sp_limit;

    if (p_owner_sp == p_curr_sp) {
        stack = (uintptr_t)__get_PSP();
    } else {
        stack = ((struct context_ctrl_t *)p_owner_sp->thrd.p_context_ctrl)->sp;

        if (p_owner_sp->p_boundaries != p_curr_sp->p_boundaries) {
            tfm_hal_update_boundaries(p_owner_sp->p_ldinf,
                                      p_owner_sp->p_boundaries);
        }

        /*
         * CURRENT_THREAD->p_context_ctrl is the svc_args[2] on MSP, safe to
         * update it. It is only used to track the owner of the thread data,
         * i.e. the partition that has been interrupted.
         */
        THRD_UPDATE_CUR_CTXCTRL(&(p_owner_sp->ctx_ctrl));
    }

    tfm_arch_init_context(&flih_ctx_ctrl,
                          (uintptr_t)svc_args[1], NULL,
                          (uintptr_t)tfm_flih_func_return,
                          sp_limit, stack);

    (void)tfm_arch_refresh_hardware_context(&flih_ctx_ctrl);

    return flih_ctx_ctrl.exc_ret;
}

/* Go back to ISR from FLIH functions */
uint32_t tfm_flih_return_to_isr(psa_flih_result_t result, uint32_t *msp)
{
    struct partition_t *p_prev_sp, *p_owner_sp;
    struct context_flih_ret_t *p_ctx_flih_ret =
                                               (struct context_flih_ret_t *)msp;

    p_prev_sp = GET_CTX_OWNER(p_ctx_flih_ret->state_ctx.r2);
    p_owner_sp = GET_CTX_OWNER(CURRENT_THREAD->p_context_ctrl);

    if (p_owner_sp->p_boundaries != p_prev_sp->p_boundaries) {
        tfm_hal_update_boundaries(p_prev_sp->p_ldinf,
                                  p_prev_sp->p_boundaries);
    }

    /* Restore context pointer */
    THRD_UPDATE_CUR_CTXCTRL(p_ctx_flih_ret->state_ctx.r2);

    tfm_arch_set_psplim(
        ((struct context_ctrl_t *)CURRENT_THREAD->p_context_ctrl)->sp_limit);
    __set_PSP(p_ctx_flih_ret->psp);

    /* Set FLIH result to the ISR */
    p_ctx_flih_ret->state_ctx.r0 = (uint32_t)result;

    return p_ctx_flih_ret->exc_ret;
}

void spm_handle_interrupt(void *p_pt, struct irq_load_info_t *p_ildi)
{
    psa_flih_result_t flih_result;
    struct partition_t *p_part;

    if (!p_pt || !p_ildi) {
        tfm_core_panic();
    }

    p_part = (struct partition_t *)p_pt;

    if (p_ildi->pid != p_part->p_ldinf->pid) {
        tfm_core_panic();
    }

    if (p_ildi->flih_func == NULL) {
        /* SLIH Model Handling */
        tfm_hal_irq_disable(p_ildi->source);
        flih_result = PSA_FLIH_SIGNAL;
    } else {
        /* FLIH Model Handling */
        if (tfm_spm_partition_get_privileged_mode(p_part->p_ldinf->flags) ==
                                                TFM_PARTITION_PRIVILEGED_MODE) {
            flih_result = p_ildi->flih_func();
        } else {
            flih_result = tfm_flih_deprivileged_handling(
                                                p_part,
                                                (uintptr_t)p_ildi->flih_func,
                                                CURRENT_THREAD->p_context_ctrl);
        }
    }

    if (flih_result == PSA_FLIH_SIGNAL) {
        spm_assert_signal(p_pt, p_ildi->signal);
    }
}
