/*
 * Copyright (c) 2019-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <inttypes.h>
#include "tfm_core_utils.h"
#include "tfm_hal_device_header.h"
#include "tfm_arch.h"
#include "spm_ipc.h"
#include "svc_num.h"
#include "exception_info.h"

#if !defined(__ARM_ARCH_6M__) && !defined(__ARM_ARCH_7M__) && \
    !defined(__ARM_ARCH_7EM__)
#error "Unsupported ARM Architecture."
#endif

extern uint32_t SVCHandler_main(uint32_t *svc_args, uint32_t lr);

#if defined(__ICCARM__)
#pragma required = do_schedule
#endif

__attribute__((naked)) void PendSV_Handler(void)
{
    __ASM volatile(
#if !defined(__ICCARM__)
        ".syntax unified                    \n"
#endif
        "   push    {r0, lr}                \n"
        "   bl      do_schedule             \n"
        "   pop     {r2, r3}                \n"
        "   mov     lr, r3                  \n"
        "   cmp     r0, r1                  \n" /* ctx of curr and next thrd */
        "   beq     v6v7_pendsv_exit        \n" /* No schedule if curr = next */
        "   mrs     r2, psp                 \n"
        "   subs    r2, #32                 \n" /* Make room for r4-r11 */
        "   stm     r2!, {r4-r7}            \n" /* Save callee registers */
        "   mov     r4, r8                  \n"
        "   mov     r5, r9                  \n"
        "   mov     r6, r10                 \n"
        "   mov     r7, r11                 \n"
        "   stm     r2!, {r4-r7}            \n"
        "   mov     r5, lr                  \n"
        "   subs    r2, #32                 \n" /* reset r2(SP) to top */
        "   stm     r0!, {r2, r3, r4, r5}   \n" /* Save struct context_ctrl_t */
        "   ldm     r1!, {r2, r3, r4, r5}   \n" /* Load ctx of next thread */
        "   mov     lr, r5                  \n"
        "   adds    r2, #16                 \n" /* Start of popping r4-r11 */
        "   ldm     r2!, {r4-r7}            \n"
        "   mov     r8, r4                  \n"
        "   mov     r9, r5                  \n"
        "   mov     r10, r6                 \n"
        "   mov     r11, r7                 \n"
        "   subs    r2, #32                 \n"
        "   ldm     r2!, {r4-r7}            \n"
        "   adds    r2, #16                 \n" /* End of popping r4-r11 */
        "   msr     psp, r2                 \n"
        "v6v7_pendsv_exit:                  \n"
        "   bx      lr                      \n"
    );
}

#if defined(__ICCARM__)
uint32_t tfm_core_svc_handler(uint32_t *msp, uint32_t exc_return,
                              uint32_t *psp);
#pragma required = tfm_core_svc_handler
#endif

__attribute__((naked)) void SVC_Handler(void)
{
    __ASM volatile(
#if !defined(__ICCARM__)
    ".syntax unified                        \n"
#endif
    "MRS     r0, MSP                        \n"
    "MOV     r1, lr                         \n"
    "MRS     r2, PSP                        \n"
    "PUSH    {r1, r2}                       \n" /* Orig_exc_return, PSP */
    "BL      tfm_core_svc_handler           \n"
    "MOV     lr, r0                         \n"
    "LDR     r1, [sp]                       \n" /* Original EXC_RETURN */
    "MOVS    r2, #8                         \n"
    "ANDS    r0, r2                         \n" /* Mode bit */
    "ANDS    r1, r2                         \n"
    "SUBS    r0, r1                         \n" /* Compare EXC_RETURN values */
    "BGT     to_flih_func                   \n"
    "BLT     from_flih_func                 \n"
    "POP     {r1, r2}                       \n" /* Orig_exc_return, PSP */
    "BX      lr                             \n"
    "to_flih_func:                          \n"
    "PUSH    {r4-r7}                        \n"
    "MOV     r4, r8                         \n"
    "MOV     r5, r9                         \n"
    "MOV     r6, r10                        \n"
    "MOV     r7, r11                        \n"
    "PUSH    {r4-r7}                        \n"
    "LDR     r4, =0xFEF5EDA5                \n" /* clear r4-r11 */
    "MOV     r5, r4                         \n"
    "MOV     r6, r4                         \n"
    "MOV     r7, r4                         \n"
    "MOV     r8, r4                         \n"
    "MOV     r9, r4                         \n"
    "MOV     r10, r4                        \n"
    "MOV     r11, r4                        \n"
    "PUSH    {r4, r5}                       \n" /* Seal stack before EXC_RET */
    "BX      lr                             \n"
    "from_flih_func:                        \n"
    "POP     {r1, r2}                       \n" /* Orig_exc_return, PSP */
    "POP     {r4, r5}                       \n" /* Seal stack */
    "POP     {r4-r7}                        \n"
    "MOV     r8, r4                         \n"
    "MOV     r9, r5                         \n"
    "MOV     r10, r6                        \n"
    "MOV     r11, r7                        \n"
    "POP     {r4-r7}                        \n"
    "POP     {r1, r2}                       \n" /* Orig_exc_return, PSP */
    "BX      lr                             \n"
    );
}

/**
 * \brief Overwrites default Hard fault handler.
 */
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
void HardFault_Handler(void)
{
    EXCEPTION_INFO(EXCEPTION_TYPE_HARDFAULT);
    /* HFSR can be read to provide further information of cause of HardFault */
     __ASM volatile("b    .");
}
#elif defined(__ARM_ARCH_6M__)
void HardFault_Handler(void)
{
    EXCEPTION_INFO(EXCEPTION_TYPE_HARDFAULT);
    /* In a baseline implementation there is no way, to find out whether this is
     * a hard fault triggered directly, or another fault that has been
     * escalated.
     */
     __ASM volatile("b    .");
}
#endif

/* Reserved for future usage */
__attribute__((naked)) void MemManage_Handler(void)
{
    EXCEPTION_INFO(EXCEPTION_TYPE_MEMFAULT);
    __ASM volatile("b    .");
}

__attribute__((naked)) void BusFault_Handler(void)
{
    EXCEPTION_INFO(EXCEPTION_TYPE_BUSFAULT);
    __ASM volatile("b    .");
}

__attribute__((naked)) void UsageFault_Handler(void)
{
    EXCEPTION_INFO(EXCEPTION_TYPE_USAGEFAULT);
    __ASM volatile("b    .");
}

void tfm_arch_set_secure_exception_priorities(void)
{
    /* Set fault priority to the highest */
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
    NVIC_SetPriority(MemoryManagement_IRQn, 0);
    NVIC_SetPriority(BusFault_IRQn, 0);
#endif

    NVIC_SetPriority(SVCall_IRQn, 0);
    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}

void tfm_arch_config_extensions(void)
{
    /* There are no coprocessors in Armv6-M implementations */
#ifndef __ARM_ARCH_6M__
#if defined(__FPU_USED) && (__FPU_USED == 1U)
    /* Enable privileged and unprivilged access to the floating-point
     * coprocessor.
     */
    SCB->CPACR |= (3U << 10U*2U)     /* enable CP10 full access */
                  | (3U << 11U*2U);  /* enable CP11 full access */
#endif
#endif
}

/* There is no FPCA in v6m */
#ifndef __ARM_ARCH_6M__
__attribute__((naked, noinline)) void tfm_arch_clear_fp_status(void)
{
    __ASM volatile(
                   ".syntax unified          \n"
                   "mrs  r0, control         \n"
                   "bics r0, r0, #4          \n"
                   "msr  control, r0         \n"
                   "isb                      \n"
                   "bx   lr                  \n"
                  );
}
#else
void tfm_arch_clear_fp_status(void)
{
}
#endif
