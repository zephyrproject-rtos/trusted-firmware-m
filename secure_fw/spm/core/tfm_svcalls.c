/*
 * Copyright (c) 2017-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "config_spm.h"
#include "interrupt.h"
#include "internal_status_code.h"
#include "memory_symbols.h"
#include "spm.h"
#include "svc_num.h"
#include "tfm_arch.h"
#include "tfm_svcalls.h"
#include "tfm_boot_data.h"
#include "tfm_hal_platform.h"
#include "tfm_hal_isolation.h"
#include "tfm_hal_spm_logdev.h"
#include "tfm_core_trustzone.h"
#include "utilities.h"
#include "ffm/backend.h"
#include "ffm/psa_api.h"
#include "load/spm_load_api.h"
#include "load/partition_defs.h"
#include "psa/client.h"

#ifdef PLATFORM_SVC_HANDLERS
extern int32_t platform_svc_handlers(uint8_t svc_number,
                                     uint32_t *ctx, uint32_t lr);
#endif

#if TFM_ISOLATION_LEVEL > 1
/*
 * TODO: To be updated after secure context management is going to implemented.
 * The variables are used to save PSP, PSPLimit and the EXC_RETURN payload because
 * they will be changed when preparing to Thread mode to run the PSA API functions.
 * Later they will be restored when returning from the functions.
 */
static uint32_t saved_psp;
static uint32_t saved_psp_limit;
static uint32_t saved_exc_return;

typedef psa_status_t (*psa_api_svc_func_t)(uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3);

/* The order of the functions must match the SVC number index defined in svc_num.h */
static psa_api_svc_func_t psa_api_svc_func_table[] = {
    /* Client APIs */
    (psa_api_svc_func_t)tfm_spm_client_psa_framework_version,
    (psa_api_svc_func_t)tfm_spm_client_psa_version,
    (psa_api_svc_func_t)tfm_spm_client_psa_call,
    (psa_api_svc_func_t)tfm_spm_client_psa_connect,
    (psa_api_svc_func_t)tfm_spm_client_psa_close,
    /* Secure Partition APIs */
    (psa_api_svc_func_t)tfm_spm_partition_psa_wait,
    (psa_api_svc_func_t)tfm_spm_partition_psa_get,
    (psa_api_svc_func_t)tfm_spm_partition_psa_set_rhandle,
    (psa_api_svc_func_t)tfm_spm_partition_psa_read,
    (psa_api_svc_func_t)tfm_spm_partition_psa_skip,
    (psa_api_svc_func_t)tfm_spm_partition_psa_write,
    (psa_api_svc_func_t)tfm_spm_partition_psa_reply,
    (psa_api_svc_func_t)tfm_spm_partition_psa_notify,
    (psa_api_svc_func_t)tfm_spm_partition_psa_clear,
    (psa_api_svc_func_t)tfm_spm_partition_psa_eoi,
    (psa_api_svc_func_t)tfm_spm_partition_psa_panic,
    (psa_api_svc_func_t)tfm_spm_get_lifecycle_state,
    (psa_api_svc_func_t)tfm_spm_partition_psa_irq_enable,
    (psa_api_svc_func_t)tfm_spm_partition_psa_irq_disable,
    (psa_api_svc_func_t)tfm_spm_partition_psa_reset_signal,
    (psa_api_svc_func_t)tfm_spm_agent_psa_call,
    (psa_api_svc_func_t)tfm_spm_agent_psa_connect,
};

static uint32_t thread_mode_spm_return(psa_status_t result)
{
    uint32_t recorded_attempts;
    struct tfm_state_context_t *p_tctx = (struct tfm_state_context_t *)saved_psp;

    ARCH_STATE_CTX_SET_R0(p_tctx, result);

    tfm_arch_set_psplim(saved_psp_limit);
    __set_PSP(saved_psp);

    spm_handle_programmer_errors(result);

    /* Release scheduler lock and check the record of schedule attempt. */
    recorded_attempts = arch_release_sched_lock();

    if ((result == STATUS_NEED_SCHEDULE) ||
        (recorded_attempts != SCHEDULER_UNLOCKED)) {
        tfm_arch_trigger_pendsv();
    }

    return saved_exc_return;
}

static void init_spm_func_context(struct context_ctrl_t *p_ctx_ctrl,
                                  psa_api_svc_func_t svc_func, uint32_t *ctx)
{
    uint32_t sp = __get_PSP();
    uint32_t sp_limit = tfm_arch_get_psplim();
    struct full_context_t *p_tctx = NULL;
    struct partition_t *caller = GET_CURRENT_COMPONENT();

    saved_psp       = sp;
    saved_psp_limit = sp_limit;

    /* Check if caller stack is within SPM stack. If not, then stack needs to switch. */
    if ((caller->ctx_ctrl.sp <= SPM_THREAD_CONTEXT->sp_limit) ||
        (caller->ctx_ctrl.sp >  SPM_THREAD_CONTEXT->sp_base)) {
        sp       = SPM_THREAD_CONTEXT->sp;
        sp_limit = SPM_THREAD_CONTEXT->sp_limit;
    }

    /* Build PSA API function context */
    p_ctx_ctrl->sp       = sp;
    p_ctx_ctrl->sp_limit = sp_limit;

    p_tctx = (struct full_context_t *)(sp);

    /* Check if enough space on stack */
    if ((uintptr_t)p_tctx - sizeof(struct full_context_t) < sp_limit) {
        tfm_core_panic();
    }

    /* Reserve a full context (state context + additional context) on the stack. */
    p_tctx--;
    spm_memset(p_tctx, 0, sizeof(*p_tctx));

    /* ctx[0] ~ ctx[3] correspond to r0 ~ r3 */
    ARCH_CTXCTRL_EXCRET_PATTERN(&p_tctx->stat_ctx, ctx[0], ctx[1], ctx[2], ctx[3],
                                svc_func, tfm_svc_thread_mode_spm_return);

    /* Assign stack and return code to the context control instance. */
    p_ctx_ctrl->sp             = (uint32_t)(p_tctx);
    p_ctx_ctrl->exc_ret        = (uint32_t)(EXC_RETURN_THREAD_PSP);
}

static int32_t prepare_to_thread_mode_spm(uint8_t svc_number, uint32_t *ctx, uint32_t exc_return)
{
    psa_api_svc_func_t svc_func = NULL;
    struct context_ctrl_t spm_func_ctx_ctrl;
    uint8_t svc_idx = svc_number & TFM_SVC_NUM_INDEX_MSK;

    if (TFM_SVC_IS_HANDLER_MODE(svc_number)) {
        /* PSA APIs are not allowed to be called from Handler mode */
        tfm_core_panic();
    }

    if (svc_idx >= sizeof(psa_api_svc_func_table)/sizeof(psa_api_svc_func_t)) {
        SPMLOG_ERRMSGVAL("Invalid PSA API SVC requested: ", svc_number);
        ctx[0] = PSA_ERROR_GENERIC_ERROR;
        return exc_return;
    }

    svc_func = psa_api_svc_func_table[svc_idx];
    if (!svc_func) {
        SPMLOG_ERRMSGVAL("Corresponding SVC function is not included for number ", svc_number);
        ctx[0] = PSA_ERROR_GENERIC_ERROR;
        return exc_return;
    }

    saved_exc_return = exc_return;

    init_spm_func_context(&spm_func_ctx_ctrl, svc_func, ctx);

    (void)tfm_arch_refresh_hardware_context(&spm_func_ctx_ctrl);

    /* svc_func can be executed in privileged Thread mode */
    __set_CONTROL_nPRIV(0);

    /* Lock scheduler during Thread mode SPM execution */
    arch_acquire_sched_lock();

    ctx[0] = PSA_SUCCESS;

    return EXC_RETURN_THREAD_PSP;
}
#endif

static uint32_t handle_spm_svc_requests(uint32_t svc_number, uint32_t exc_return,
                                        uint32_t *svc_args, uint32_t *msp)
{
    switch (svc_number) {
    case TFM_SVC_SPM_INIT:
        exc_return = tfm_spm_init();
        tfm_arch_check_msp_sealing();
        /* The following call does not return */
        tfm_arch_free_msp_and_exc_ret(SPM_BOOT_STACK_BOTTOM, exc_return);
        break;
    case TFM_SVC_GET_BOOT_DATA:
        tfm_core_get_boot_data_handler(svc_args);
        break;
#if (TFM_ISOLATION_LEVEL != 1) && (CONFIG_TFM_FLIH_API == 1)
    case TFM_SVC_PREPARE_DEPRIV_FLIH:
        exc_return = tfm_flih_prepare_depriv_flih((struct partition_t *)svc_args[0],
                                                  (uintptr_t)svc_args[1]);
        break;
    case TFM_SVC_FLIH_FUNC_RETURN:
        exc_return = tfm_flih_return_to_isr(svc_args[0], (struct context_flih_ret_t *)msp);
        break;
#endif
#if TFM_SP_LOG_RAW_ENABLED
    case TFM_SVC_OUTPUT_UNPRIV_STRING:
        svc_args[0] = tfm_hal_output_spm_log((const char *)svc_args[0], svc_args[1]);
        break;
#endif
#if TFM_ISOLATION_LEVEL > 1
    case TFM_SVC_THREAD_MODE_SPM_RETURN:
        exc_return = thread_mode_spm_return(svc_args[0]);
        break;
#endif
    default:
        SPMLOG_ERRMSGVAL("Unknown SPM SVC requested: ", svc_number);
        svc_args[0] = PSA_ERROR_GENERIC_ERROR;
    }

    return exc_return;
}

uint32_t spm_svc_handler(uint32_t *msp, uint32_t exc_return, uint32_t *psp)
{
    uint8_t svc_number = TFM_SVC_PSA_FRAMEWORK_VERSION;
    uint32_t *svc_args = msp;

    if ((exc_return & EXC_RETURN_MODE) && (exc_return & EXC_RETURN_SPSEL)) {
        /* Use PSP when both EXC_RETURN.MODE and EXC_RETURN.SPSEL are set */
        svc_args = psp;
    }

    if (is_return_secure_stack(exc_return)) {
        if (is_default_stacking_rules_apply(exc_return) == false) {
            /* In this case offset the svc_args and only use
             * the caller-saved registers
             */
            svc_args = &svc_args[10];
        }

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

    if (!!(exc_return & EXC_RETURN_MODE) == TFM_SVC_IS_HANDLER_MODE(svc_number)) {
        /* Mode of caller does match mode of the target SVC */
        tfm_core_panic();
    }

    if (TFM_SVC_IS_SPM(svc_number)) {
        /* SPM SVC */
        return handle_spm_svc_requests(svc_number, exc_return, svc_args, msp);
    }

#if TFM_ISOLATION_LEVEL > 1
    if (TFM_SVC_IS_PSA_API(svc_number)) {
        if (((uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK, $$ZI$$Limit) - (uint32_t)msp) > 0) {
            /* The Main Stack has contents, not calling from Partition thread */
            tfm_core_panic();
        }

        return prepare_to_thread_mode_spm(svc_number, svc_args, exc_return);
    }
#endif

#ifdef PLATFORM_SVC_HANDLERS
    if (TFM_SVC_IS_PLATFORM(svc_number)) {
        svc_args[0] = (platform_svc_handlers(svc_number, svc_args, exc_return));
        return exc_return;
    }
#endif

    SPMLOG_ERRMSGVAL("Unknown SVC number requested: ", svc_number);
    svc_args[0] = PSA_ERROR_GENERIC_ERROR;

    return exc_return;
}

__attribute__((naked))
void tfm_svc_thread_mode_spm_return(psa_status_t result)
{
    __ASM volatile("SVC "M2S(TFM_SVC_THREAD_MODE_SPM_RETURN)"           \n");
}

__attribute__ ((naked)) void tfm_core_handler_mode(void)
{
    __ASM volatile("SVC %0           \n"
                   "BX LR            \n"
                   : : "I" (TFM_SVC_SPM_INIT));
}
