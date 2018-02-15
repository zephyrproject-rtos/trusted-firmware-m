/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cmsis.h"
#include "tfm_secure_api.h"
#include "secure_utilities.h"
#include "uart_stdout.h"
#include "secure_fw/spm/spm_api.h"
#include "region_defs.h"
#include "tfm_api.h"

#define SECURE_LOCK_STACK_LIMIT 5

/* NOTE: this does not account for possible FP stacking */
#define SVC_STACK_FRAME_SIZE 0x20

static struct tfm_secure_lock_ctx_t {
    uint32_t service_id;
    uint32_t orig_psp;
    uint32_t orig_psplim;
    uint32_t orig_lr;
    int32_t  ns_caller;
    enum tfm_buffer_share_region_e share;
} ctx[SECURE_LOCK_STACK_LIMIT];

#ifndef TFM_LVL
#error TFM_LVL is not defined!
#endif

#if TFM_LVL == 1
/* Macros to pick linker symbols and allow references to sections */
#define REGION(a, b, c) a##b##c
#define REGION_NAME(a, b, c) REGION(a, b, c)
#define REGION_DECLARE(a, b, c) extern uint32_t REGION_NAME(a, b, c)

REGION_DECLARE(Image$$, TFM_SECURE_STACK, $$ZI$$Base);
REGION_DECLARE(Image$$, TFM_SECURE_STACK, $$ZI$$Limit);
#endif

extern void tfm_core_service_return_svc(void);

static int32_t lock_cnt;

static uint32_t *prepare_service_ctx(
            struct tfm_sfn_req_s *desc_ptr, uint32_t *dst)
{
    /* XPSR  = Thumb, nothing else */
    *(--dst) = 0x01000000;
    /* ReturnAddress = sfn to be executed */
    *(--dst) = (uint32_t)(desc_ptr->sfn);
    /* LR = function to be called when sfn exits */
    *(--dst) = (uint32_t)tfm_core_service_return_svc;
    /* R12 = don't care */
    *(--dst) = 0;

    /* R0-R3 = sfn arguments */
    int32_t i = 4;

    while (i > 0) {
        i--;
        *(--dst) = desc_ptr->args[i];
    }
    return dst;
}

static int32_t tfm_push_lock(struct tfm_sfn_req_s *desc_ptr, uint32_t lr)
{
    if ((lock_cnt >= 0) && (lock_cnt < SECURE_LOCK_STACK_LIMIT)) {
        register uint32_t service_id = desc_ptr->ss_id;
        uint32_t psp = __get_PSP();
        uint32_t service_psp, service_psplim;
#if TFM_LVL == 1
        /* Prepare switch to shared secure service stack */
        service_psp =
            (uint32_t)&REGION_NAME(Image$$, TFM_SECURE_STACK, $$ZI$$Limit);
        service_psplim =
            (uint32_t)&REGION_NAME(Image$$, TFM_SECURE_STACK, $$ZI$$Base);
        if ((lock_cnt > 0) && (ctx[lock_cnt-1].service_id == desc_ptr->ss_id)) {
            /* Direct recursion is not permitted! */
            return TFM_ERROR_SERVICE_NON_REENTRANT;
        }
#else
        if (lock_cnt > 0) {
            /* Store the caller PSP in case we are doing a service to
             * service call
             */
            tfm_spm_service_set_stack(ctx[lock_cnt-1].service_id, psp);
        }

        service_psp = tfm_spm_service_get_stack(service_id);
        if (service_psp != tfm_spm_service_get_stack_top(service_id)) {
            /* We can't stack a service if it's already running, i.e. it has a
             * context stacked
             */
            return TFM_ERROR_SERVICE_NON_REENTRANT;
        }
        service_psplim = tfm_spm_service_get_stack_bottom(service_id);
#endif
        /* Stack the context for the service call */
        ctx[lock_cnt].service_id = service_id;
        ctx[lock_cnt].orig_psp = psp;
        ctx[lock_cnt].orig_psplim = __get_PSPLIM();
        ctx[lock_cnt].orig_lr = lr;
        ctx[lock_cnt].ns_caller = desc_ptr->ns_caller;
        /* Default share to scratch area in case of service-to-service calls
         * this way services always get default access to input buffers
         */
        ctx[lock_cnt].share = desc_ptr->ns_caller ?
            TFM_BUFFER_SHARE_NS_CODE : TFM_BUFFER_SHARE_SCRATCH;

#if (TFM_LVL != 1) && (TFM_LVL != 2)
        /* Dynamic service partitioning is only done is TFM level 3 */
        if (lock_cnt > 0) {
            /* In a service to service call, deconfigure the caller service */
            tfm_spm_service_sandbox_deconfig(ctx[lock_cnt-1].service_id);
        }

        /* Configure service execution environment */
        tfm_spm_service_sandbox_config(service_id);
#endif

#if TFM_LVL == 1
        /* In level one, only switch context and return from exception if in
         * handler mode
         */
        if (desc_ptr->exc_num != EXC_NUM_THREAD_MODE) {
            /* Prepare the service context, update stack ptr */
            psp = (uint32_t)prepare_service_ctx(
                        desc_ptr, (uint32_t *)service_psp);
            __set_PSP(psp);
            __set_PSPLIM(service_psplim);
        }
#else
        /* FixMe: return value/error handling TBD */
        tfm_spm_set_share_region(ctx[lock_cnt].share);

        /* Prepare the service context, update stack ptr */
        psp = (uint32_t)prepare_service_ctx(desc_ptr, (uint32_t *)service_psp);
        __set_PSP(psp);
        __set_PSPLIM(service_psplim);
#endif

        lock_cnt++;

        return TFM_SUCCESS;
    } else {
        /* Secure Service stacking limit exceeded */
        return TFM_SECURE_LOCK_FAILED;
    }
}

static int32_t tfm_pop_lock(uint32_t *lr_ptr)
{
    if ((lock_cnt > 0) && (lock_cnt <= SECURE_LOCK_STACK_LIMIT)) {
        lock_cnt--;
#if (TFM_LVL != 1) && (TFM_LVL != 2)
        /* Deconfigure completed service environment */
        tfm_spm_service_sandbox_deconfig(ctx[lock_cnt].service_id);
        if (lock_cnt > 0) {
            /* Configure the caller service environment in case this was a
             * service to service call
             */
            tfm_spm_service_sandbox_config(ctx[lock_cnt-1].service_id);
            /* Restore share status */
            tfm_spm_set_share_region(ctx[lock_cnt-1].share);
        }
#endif

#if TFM_LVL == 1
        if (ctx[lock_cnt].ns_caller) {
            /* In TFM level 1 context restore is only done when
             * returning to NS
             */
            /* Restore caller PSP and LR ptr */
            __set_PSP(ctx[lock_cnt].orig_psp);
            __set_PSPLIM(ctx[lock_cnt].orig_psplim);
            *lr_ptr = ctx[lock_cnt].orig_lr;
        }
#else
        uint32_t psp = __get_PSP();

        /* Discount SVC call stack frame when storing sfn ctx */
        tfm_spm_service_set_stack(
                    ctx[lock_cnt].service_id, psp + SVC_STACK_FRAME_SIZE);

        /* Restore caller PSP and LR ptr */
        __set_PSP(ctx[lock_cnt].orig_psp);
        __set_PSPLIM(ctx[lock_cnt].orig_psplim);
        *lr_ptr = ctx[lock_cnt].orig_lr;
#endif

        /* Clear the context entry in the context stack before returning */
        memset(&ctx[lock_cnt], 0, sizeof(ctx[0]));

        return TFM_SUCCESS;
    } else {
        /* Secure Service stack count incorrect */
        return TFM_SECURE_UNLOCK_FAILED;
    }
}

void tfm_secure_api_error_handler(void)
{
    ERROR_MSG("Secure fault when calling secure API");
    while (1) {
        ;
    }
}

static int32_t tfm_check_sfn_req_integrity(struct tfm_sfn_req_s *desc_ptr)
{
    if ((desc_ptr == NULL) ||
        (desc_ptr->ss_id == 0) ||
        (desc_ptr->sfn == NULL)) {
        /* invalid parameter */
        return TFM_ERROR_INVALID_PARAMETER;
    }
    return TFM_SUCCESS;
}

static int32_t tfm_core_check_service_req_rules(struct tfm_sfn_req_s *desc_ptr)
{
    if ((desc_ptr->exc_num != EXC_NUM_THREAD_MODE) &&
        (desc_ptr->exc_num != EXC_NUM_SVCALL)) {
        return TFM_ERROR_INVALID_EXC_MODE;
    }

    if (desc_ptr->ns_caller) {
        if (desc_ptr->exc_num == EXC_NUM_THREAD_MODE) {
            /* Veneer should not be called from NS thread mode!
             * FixMe: this is potential attack, trigger fault handling
             */
            return TFM_ERROR_NS_THREAD_MODE_CALL;
        }
        if (lock_cnt != 0) {
            /*
             * Secure domain is already locked!
             * This should only happen if caller is secure function!
             * FixMe: This scenario is a potential security breach
             * Take appropriate action!
             */
            return TFM_ERROR_SECURE_DOMAIN_LOCKED;
        }
    } else {
        if (desc_ptr->exc_num != EXC_NUM_THREAD_MODE) {
            /* Secure service can only call a different secure service
             * from thread mode
             */
            return TFM_ERROR_INVALID_EXC_MODE;
        }
    }
    return TFM_SUCCESS;
}

static union ns_state_u {
    uint32_t AIRCR;
    uint32_t SHCSR_NS;
} ns_state;

static int32_t tfm_core_configure_secure_exception(void)
{
#ifdef TFM_API_DEPRIORITIZE
    uint32_t VECTKEY;
    SCB_Type *scb = SCB;

    ns_state.AIRCR = scb->AIRCR;
    VECTKEY = (~ns_state.AIRCR & SCB_AIRCR_VECTKEYSTAT_Msk);
#endif

    SCB->SHCSR |= SCB_SHCSR_SVCALLACT_Msk;

#ifdef TFM_API_DEPRIORITIZE
    scb->AIRCR = SCB_AIRCR_PRIS_Msk |
                 VECTKEY |
                 (ns_state.AIRCR & ~SCB_AIRCR_VECTKEY_Msk);
#elif defined(TFM_API_SVCCLEAR)
    ns_state.SHCSR_NS = SCB_NS->SHCSR;
    SCB_NS->SHCSR &= ~SCB_SHCSR_SVCALLACT_Msk;
#endif

    return TFM_SUCCESS;
}

static int32_t tfm_core_deconfigure_secure_exception(void)
{
#ifdef TFM_API_DEPRIORITIZE
    SCB_Type *scb = SCB;
    uint32_t VECTKEY = (~ns_state.AIRCR & SCB_AIRCR_VECTKEYSTAT_Msk);

    scb->AIRCR = (~SCB_AIRCR_PRIS_Msk) &
                 (VECTKEY | (ns_state.AIRCR & ~SCB_AIRCR_VECTKEY_Msk));
#elif defined(TFM_API_SVCCLEAR)
    SCB_NS->SHCSR = ns_state.SHCSR_NS;
#endif
    SCB->SHCSR &= ~SCB_SHCSR_SVCALLACT_Msk;

    /*
     * SPSEL value is cleared by HW when entering Handler mode (e.g.
     * service return SVC).
     * If execution is returned to NS instead of performing an Exception
     * return, HW will not restore original SPSEL value so this has to
     * be done manually.
     */
     __set_CONTROL_SPSEL(1);

    return TFM_SUCCESS;
}

#if defined(__ARM_ARCH_8M_MAIN__)
__attribute__((naked)) static int32_t tfm_core_exc_return_to_service(void)
{
    /* Save all callee-saved registers to prevent malicious service from
     * modifying execution state.
     * Save LR for return address.
     * r12 is used as padding for 8-byte stack alignment
     */
    __ASM(
    "PUSH   {r4-r12, lr}\n"
    "MVN    r0, #2\n"
    "BX     r0\n"
"return_from_service:\n"
    "POP    {r4-r12, pc}\n");
}
#elif defined(__ARM_ARCH_8M_BASE__)
__attribute__((naked)) static int32_t tfm_core_exc_return_to_service(void)
{
    /* Save all callee-saved registers to prevent malicious service from
     * modifying execution state.
     * Save LR for return address.
     * r12 is used as padding for 8-byte stack alignment
     */
    __ASM(
    "PUSH   {lr}\n"
    "PUSH   {r4-r7}\n"
    "MOV    r4, r8\n"
    "MOV    r5, r9\n"
    "MOV    r6, r10\n"
    "MOV    r7, r11\n"
    "PUSH   {r4-r7}\n"
    "MOV    r4, r12\n"
    "PUSH   {r4}\n"
    "MOVS   r0, #2\n"
    "MVNS   r0, r0\n"
    "BX     r0\n"
"return_from_service:\n"
    "POP    {r4}\n"
    "MOV    r12, r4\n"
    "POP    {r4-r7}\n"
    "MOV    r8, r4\n"
    "MOV    r9, r5\n"
    "MOV    r10, r6\n"
    "MOV    r11, r7\n"
    "POP    {r4-r7}\n"
    "POP    {pc}\n");
}
#else
#error "Unsupported ARM Architecture."
#endif

extern void return_from_service(void);

static int32_t tfm_core_call_service(struct tfm_sfn_req_s *desc_ptr)
{
#if TFM_LVL == 1
    if (desc_ptr->exc_num == EXC_NUM_THREAD_MODE) {
        /* service to service call in TFM level 1 */
        int32_t res;
        uint32_t *args = desc_ptr->args;
        int32_t retVal = desc_ptr->sfn(args[0], args[1], args[2], args[3]);
        /* return handler should restore original exc_return value... */
        res = tfm_pop_lock(NULL);
        if (res == TFM_SUCCESS) {
            /* If unlock successful, pass SS return value to caller */
            res = retVal;
        } else {
            /* Unlock errors indicate ctx database corruption or unknown
             * anomalies. Halt execution
             */
            ERROR_MSG("Secure API error during unlock!");
            tfm_secure_api_error_handler();
        }
        return res;
    }
#endif

    /* Exception return to service start */
    return tfm_core_exc_return_to_service();
}

int32_t tfm_core_service_request_function(struct tfm_sfn_req_s *desc_ptr)
{
    int32_t res;

    res = tfm_check_sfn_req_integrity(desc_ptr);
    if (res != TFM_SUCCESS) {
        return TFM_ERROR_STATUS(res);
    }

    __disable_irq();
    res = tfm_core_check_service_req_rules(desc_ptr);
    if (res != TFM_SUCCESS) {
        __enable_irq();
        return TFM_ERROR_STATUS(res);
    }

    /* return to ASM label in call_service() function */
    res = tfm_push_lock(desc_ptr, (uint32_t)return_from_service | 1);
    if (res != TFM_SUCCESS) {
        /* FixMe: consider possible fault scenarios */
        __enable_irq();
        return TFM_ERROR_STATUS(res);
    }

    if (desc_ptr->ns_caller) {
        tfm_core_configure_secure_exception();
    }

    __enable_irq();
    res = tfm_core_call_service(desc_ptr);

    if (desc_ptr->ns_caller) {
        __disable_irq();
        tfm_core_deconfigure_secure_exception();
        __enable_irq();
    }

    return res;
}

void tfm_core_validate_secure_caller_handler(uint32_t *svc_args)
{

    int32_t res = TFM_ERROR_GENERIC;

    if (lock_cnt == 0)  {
        /* This handler shouldn't be called from outside service context.
         * Services are only allowed to run while S domain is locked.
         */
        svc_args[0] = TFM_ERROR_INVALID_PARAMETER;
        return;
    }

    /* Store return value in r0 */
    if (!ctx[lock_cnt-1].ns_caller) {
        res = TFM_SUCCESS;
    }
    svc_args[0] = res;
}

void tfm_core_memory_permission_check_handler(uint32_t *svc_args)
{
    uint32_t ptr = svc_args[0];
    uint32_t size = svc_args[1];
    int32_t access = svc_args[2];

    uint32_t max_buf_size, ptr_start, range_limit, range_check = false;
    int32_t res;

    if ((lock_cnt == 0) || (size == 0)) {
        /* This handler shouldn't be called from outside service context.
         * Services are only allowed to run while S domain is locked.
         */
        svc_args[0] = TFM_ERROR_INVALID_PARAMETER;
        return;
    }

    int32_t flags = 0;

    if (ctx[lock_cnt-1].share != TFM_BUFFER_SHARE_PRIV) {
        flags |= CMSE_MPU_UNPRIV;
    }

    if (access == TFM_MEMORY_ACCESS_RW) {
        flags |= CMSE_MPU_READWRITE;
    } else {
        flags |= CMSE_MPU_READ;
    }

    /* Check if service access to address would fail */
    void *rangeptr = cmse_check_address_range((void *)ptr, size, flags);

    /* Get regions associated with address */
    cmse_address_info_t addr_info = cmse_TT((void *)ptr);

    if (rangeptr == NULL) {
        svc_args[0] = TFM_ERROR_INVALID_PARAMETER;
        return;
    }

    if (addr_info.flags.secure) {
#if TFM_LVL == 1
        /* For privileged service execution, all secure data memory is
         * accessible
         */
        max_buf_size = S_DATA_SIZE;
        ptr_start = S_DATA_START;
        range_limit = S_DATA_LIMIT;
#else
        /* Only scratch is permitted in secure memory */
        max_buf_size = (uint32_t)tfm_scratch_area_size;
        ptr_start = (uint32_t)tfm_scratch_area;
        range_limit = (uint32_t)tfm_scratch_area + tfm_scratch_area_size - 1;
#endif
        range_check = true;
    } else {
        if (!addr_info.flags.sau_region_valid) {
            /* If address is NS, TF-M expects SAU to be configured
             */
            svc_args[0] = TFM_ERROR_INVALID_PARAMETER;
            return;
        }
        switch (addr_info.flags.sau_region) {
        case TFM_NS_REGION_CODE:
            if (access == TFM_MEMORY_ACCESS_RW) {
                res = TFM_ERROR_INVALID_PARAMETER;
            } else {
                /* Currently TF-M does not support checks for NS Memory
                 * accesses by services
                 */
                res = TFM_SUCCESS;
            }
            break;
        case TFM_NS_REGION_DATA:
            /* Currently TF-M does not support checks for NS Memory
             * accesses by services
             */
            res = TFM_SUCCESS;
            break;
        default:
            /* Only NS data and code regions can be accessed as buffers */
            res = TFM_ERROR_INVALID_PARAMETER;
            break;
        }
    }

    if (range_check == true) {
        if ((size <= max_buf_size) && (ptr >= ptr_start)
            && (ptr <= range_limit + 1 - size)) {
            res = TFM_SUCCESS;
        } else {
            res = TFM_ERROR_INVALID_PARAMETER;
        }
    }

    /* Store return value in r0 */
    svc_args[0] = res;
}

/* This SVC handler is called if veneer is running in thread mode */
void tfm_core_service_request_svc_handler(
        uint32_t *svc_ctx, uint32_t *lr_ptr)
{
    if (!(*lr_ptr & EXC_RETURN_STACK_PROCESS)) {
        /* Service request SVC called with MSP active.
         * Either invalid configuration for Thread mode or SVC called
         * from Handler mode, which is not supported.
         * FixMe: error severity TBD
         */
        ERROR_MSG("Service request SVC called with MSP active!");
        tfm_secure_api_error_handler();
    }

    struct tfm_sfn_req_s *desc_ptr = (struct tfm_sfn_req_s *) svc_ctx[0];

    int32_t res = tfm_check_sfn_req_integrity(desc_ptr);

    if (res != TFM_SUCCESS) {
        /* The descriptor is incorrectly filled
         * FixMe: error severity TBD
         */
        svc_ctx[0] = TFM_ERROR_STATUS(res);
        return;
    }

    if (desc_ptr->exc_num != EXC_NUM_THREAD_MODE) {
        /* The descriptor is incorrectly filled for SVC handler
         * FixMe: error severity TBD
         */
        svc_ctx[0] = TFM_ERROR_STATUS(TFM_ERROR_INVALID_PARAMETER);
    } else {
        svc_ctx[0] = (uint32_t)tfm_core_service_request_function(desc_ptr);
    }
    return;
}

/* This SVC handler is called when sfn returns */
int32_t tfm_core_service_return_handler(uint32_t *lr_ptr)
{
    int32_t res;

    __disable_irq();

    /* Store return value from secure service */
    if (!(*lr_ptr & EXC_RETURN_STACK_PROCESS)) {
        /* Service return SVC called with MSP active. This should not happen! */
        ERROR_MSG("Service return SVC called with MSP active!");
        tfm_secure_api_error_handler();
    }
    int32_t retVal = *(int32_t *)__get_PSP();

    switch (retVal) {
    case TFM_SUCCESS:
    case TFM_SERVICE_BUSY:
        /* Secure service is allowed to return these pre-defined values */
        break;
    default:
        if ((retVal > TFM_SUCCESS) &&
            (retVal < TFM_SERVICE_SPECIFIC_ERROR_MIN)) {
            /* Secure function returned a reserved value */
#ifdef TFM_CORE_DEBUG
            LOG_MSG("Invalid return value from secure service!");
#endif
            /* FixMe: error can be traced to specific secure partition
             * and Core is not compromised. Error handling flow can be
             * refined
             */
            tfm_secure_api_error_handler();
        }
    }

    /* return handler should restore original exc_return value... */
    res = tfm_pop_lock(lr_ptr);
    if (res == TFM_SUCCESS) {
        /* If unlock successful, pass SS return value to caller */
        res = retVal;
    } else {
        /* Unlock errors indicate ctx database corruption or unknown anomalies
         * Halt execution
         */
        ERROR_MSG("Secure API error during unlock!");
        tfm_secure_api_error_handler();
    }

    /* FixMe: keeping legacy requirement to check consistency, can be removed
     * when push/pop lock is updated to not store redundant return value
     */
    if (*lr_ptr != ((uint32_t)return_from_service | 1)) {
        /* Return address does not match expected value
         * This indicates an inconsistency in ctx database
         */
        tfm_secure_api_error_handler();
    }
    __enable_irq();
    return res;
}

void tfm_core_set_buffer_area_handler(uint32_t *args)
{
    /* r0 is stored in args[0] in exception stack frame
     * Store input parameter before writing return value to that address
     */
    enum tfm_buffer_share_region_e share;
     /* tfm_core_set_buffer_area() returns int32_t */
    int32_t *res_ptr = (int32_t *)&args[0];

    if (lock_cnt == 0) {
        /* This handler shouldn't be called from outside service context.
         * Services are only allowed to run while S domain is locked
         */
        *res_ptr = TFM_ERROR_INVALID_PARAMETER;
        return;
    }

    switch (args[0]) {
    case TFM_BUFFER_SHARE_DEFAULT:
        share = (ctx[lock_cnt-1].ns_caller) ?
            (TFM_BUFFER_SHARE_NS_CODE) : (TFM_BUFFER_SHARE_SCRATCH);
        break;
    case TFM_BUFFER_SHARE_SCRATCH:
    case TFM_BUFFER_SHARE_NS_CODE:
        share = args[0];
        break;
    default:
        *res_ptr = TFM_ERROR_INVALID_PARAMETER;
        return;
    }

#if TFM_LVL == 1
    /* No configuration needed in level 1 */
    ctx[lock_cnt-1].share = share;
    *res_ptr = TFM_SUCCESS;
#else
    if (tfm_spm_set_share_region(share) == SPM_ERR_OK) {
        ctx[lock_cnt-1].share = share;
        *res_ptr = TFM_SUCCESS;
    } else {
        *res_ptr = TFM_ERROR_INVALID_PARAMETER;
    }
#endif

    return;
}
