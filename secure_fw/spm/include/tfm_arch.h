/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __TFM_ARCH_H__
#define __TFM_ARCH_H__

/* This header file collects the architecture related operations. */

#include <stddef.h>
#include <inttypes.h>
#include "tfm_hal_device_header.h"
#include "cmsis_compiler.h"

#if defined(__ARM_ARCH_8_1M_MAIN__) || \
    defined(__ARM_ARCH_8M_MAIN__)  || defined(__ARM_ARCH_8M_BASE__)
#include "tfm_arch_v8m.h"
#elif defined(__ARM_ARCH_6M__) || defined(__ARM_ARCH_7M__) || \
      defined(__ARM_ARCH_7EM__)
#include "tfm_arch_v6m_v7m.h"
#else
#error "Unsupported ARM Architecture."
#endif

#define SCHEDULER_LOCKED    1
#define SCHEDULER_UNLOCKED  0

#define XPSR_T32            0x01000000

/* State context defined by architecture */
struct tfm_state_context_t {
    uint32_t    r0;
    uint32_t    r1;
    uint32_t    r2;
    uint32_t    r3;
    uint32_t    r12;
    uint32_t    lr;
    uint32_t    ra;
    uint32_t    xpsr;
};

/* Context addition to state context */
struct tfm_additional_context_t {
    uint32_t    callee[8];     /* R4-R11. NOT ORDERED!! */
};

/* Full thread context */
struct full_context_t {
    struct tfm_additional_context_t addi_ctx;
    struct tfm_state_context_t      stat_ctx;
};

/* Context control */
struct context_ctrl_t {
    uint32_t                sp;        /* Stack pointer (higher address)   */
    uint32_t                sp_limit;  /* Stack limit (lower address)      */
    uint32_t                reserved;  /* Reserved                         */
    uint32_t                exc_ret;   /* EXC_RETURN pattern.              */
};

/*
 * The context on MSP when de-privileged FLIH Function calls SVC to return.
 * It is the same when de-privileged FLIH Function is ready to run.
 */
struct context_flih_ret_t {
    uint64_t stack_seal;    /* Two words stack seal */
    struct tfm_additional_context_t addi_ctx;
    uint32_t exc_ret;   /* EXC_RETURN value when interrupt exception ocurrs */
    uint32_t psp;       /* PSP when interrupt exception ocurrs              */
    struct tfm_state_context_t state_ctx; /* ctx on SVC_PREPARE_DEPRIV_FLIH */
};

/**
 * \brief Get Link Register
 * \details Returns the value of the Link Register (LR)
 * \return LR value
 */
#if !defined ( __ICCARM__ )
__attribute__ ((always_inline)) __STATIC_INLINE uint32_t __get_LR(void)
{
    register uint32_t result;

    __ASM volatile ("MOV %0, LR\n" : "=r" (result));
    return result;
}
#endif

__STATIC_INLINE uint32_t __save_disable_irq(void)
{
    uint32_t result;

    __ASM volatile ("mrs %0, primask \n cpsid i" : "=r" (result) :: "memory");
    return result;
}

__STATIC_INLINE void __restore_irq(uint32_t status)
{
    __ASM volatile ("msr primask, %0" :: "r" (status) : "memory");
}

__attribute__ ((always_inline))
__STATIC_INLINE uint32_t __get_active_exc_num(void)
{
    IPSR_Type IPSR;

    /* if non-zero, exception is active. NOT banked S/NS */
    IPSR.w = __get_IPSR();
    return IPSR.b.ISR;
}

__attribute__ ((always_inline))
__STATIC_INLINE void __set_CONTROL_SPSEL(uint32_t SPSEL)
{
    CONTROL_Type ctrl;

    ctrl.w = __get_CONTROL();
    ctrl.b.SPSEL = SPSEL;
    __set_CONTROL(ctrl.w);
    __ISB();
}

#if (CONFIG_TFM_SPE_FP >= 1) && CONFIG_TFM_LAZY_STACKING_SPE
#define ARCH_FLUSH_FP_CONTEXT()  __asm volatile("vmov  s0, s0 \n":::"memory")
#else
#define ARCH_FLUSH_FP_CONTEXT()
#endif

/* Set secure exceptions priority. */
void tfm_arch_set_secure_exception_priorities(void);

/* Configure various extensions. */
void tfm_arch_config_extensions(void);

/* Clear float point status. */
void tfm_arch_clear_fp_status(void);

#if (CONFIG_TFM_SPE_FP >= 1)
/*
 * Clear float point data.
 */
void tfm_arch_clear_fp_data(void);
#endif

/*
 * This function is called after SPM has initialized.
 * It frees the stack used by SPM initialization and do Exception Return.
 * It does not return.
 */
void tfm_arch_free_msp_and_exc_ret(uint32_t msp_base, uint32_t exc_return);

/*
 * This function sets return value on APIs that cause scheduling, for example
 * psa_wait(), by manipulating the control context - this is usaully setting the
 * R0 register of the thread context.
 */
void tfm_arch_set_context_ret_code(void *p_ctx_ctrl, uintptr_t ret_code);

/* Init a thread context on thread stack and update the control context. */
void tfm_arch_init_context(void *p_ctx_ctrl,
                           uintptr_t pfn, void *param, uintptr_t pfnlr,
                           uintptr_t sp_limit, uintptr_t sp);

/*
 * Refresh the HW (sp, splimit) according to the given control context and
 * returns the EXC_RETURN payload (caller might need it for following codes).
 *
 * The p_ctx_ctrl must have been initialized by tfm_arch_init_context
 */
uint32_t tfm_arch_refresh_hardware_context(void *p_ctx_ctrl);

/*
 * Triggers scheduler. A return type is assigned in case
 * SPM returns values by the context.
 */
uint32_t tfm_arch_trigger_pendsv(void);


/*
 * Switch to a new stack area, lock scheduler and call function.
 * If 'stk_base' is ZERO, stack won't be switched and re-use caller stack.
 */
uint32_t arch_non_preempt_call(uintptr_t fn_addr, uintptr_t frame_addr,
                               uint32_t stk_base, uint32_t stk_limit);

#endif
