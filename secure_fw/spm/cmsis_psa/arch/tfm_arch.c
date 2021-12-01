/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "compiler_ext_defs.h"
#include "security_defs.h"
#include "tfm_arch.h"
#include "tfm_core_utils.h"
#include "utilities.h"

__naked void tfm_arch_free_msp_and_exc_ret(uint32_t msp_base,
                                           uint32_t exc_return)
{
    __ASM volatile(
#if !defined(__ICCARM__)
        ".syntax unified                        \n"
#endif
        "subs    r0, #8                         \n" /* SEAL room */
        "msr     msp, r0                        \n"
        "bx      r1                             \n"
    );
}

void tfm_arch_set_context_ret_code(void *p_ctx_ctrl, uintptr_t ret_code)
{
    ((struct full_context_t *)(((struct context_ctrl_t *)p_ctx_ctrl)->sp))
                                                       ->stat_ctx.r0 = ret_code;
}

/* Caution: Keep 'uint32_t' always for collecting thread return values! */
__attribute__((naked)) uint32_t tfm_arch_trigger_pendsv(void)
{
    __ASM volatile(
#ifndef __ICCARM__
        ".syntax unified                               \n"
#endif
        "ldr     r0, ="M2S(SCB_ICSR_ADDR)"             \n"
        "ldr     r1, ="M2S(SCB_ICSR_PENDSVSET_BIT)"    \n"
        "str     r1, [r0, #0]                          \n"
        "dsb     #0xf                                  \n"
        "isb                                           \n"
        "bx      lr                                    \n"
    );
}

/*
 * Initializes the State Context. The Context is used to do Except Return to
 * Thread Mode to start a function.
 *
 * p_sctx[out] - pointer to the State Context to be initialized.
 * param [in]  - The parameter for the function to start
 * pfn   [in]  - Pointer to the function to excute
 * pfnlr [in]  - The Link Register of the State Context - the return address of
 *               the function
 */
static void tfm_arch_init_state_context(struct tfm_state_context_t *p_sctx,
                                        void *param,
                                        uintptr_t pfn, uintptr_t pfnlr)
{
    p_sctx->r0 = (uint32_t)param;
    p_sctx->ra = (uint32_t)pfn;
    p_sctx->lr = (uint32_t)pfnlr;
    p_sctx->xpsr = XPSR_T32;
}

void tfm_arch_init_context(void *p_ctx_ctrl,
                           uintptr_t pfn, void *param, uintptr_t pfnlr,
                           uintptr_t sp_limit, uintptr_t sp)
{
    struct full_context_t *p_tctx =
            (struct full_context_t *)arch_seal_thread_stack(sp);

    p_tctx--;

    spm_memset(p_tctx, 0, sizeof(*p_tctx));

    tfm_arch_init_state_context(&p_tctx->stat_ctx, param, pfn, pfnlr);

    ((struct context_ctrl_t *)p_ctx_ctrl)->exc_ret  = EXC_RETURN_THREAD_S_PSP;
    ((struct context_ctrl_t *)p_ctx_ctrl)->sp_limit = sp_limit;
    ((struct context_ctrl_t *)p_ctx_ctrl)->sp       = (uintptr_t)p_tctx;
}

uint32_t tfm_arch_refresh_hardware_context(void *p_ctx_ctrl)
{
    struct context_ctrl_t *ctx_ctrl;
    struct tfm_state_context_t *sc;

    ctx_ctrl  = (struct context_ctrl_t *)p_ctx_ctrl;
    sc = &(((struct full_context_t *)(ctx_ctrl->sp))->stat_ctx);

    tfm_arch_set_psplim(ctx_ctrl->sp_limit);
    __set_PSP((uintptr_t)sc);

    return ctx_ctrl->exc_ret;
}
