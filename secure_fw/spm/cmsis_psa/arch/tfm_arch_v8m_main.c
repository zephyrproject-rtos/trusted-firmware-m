/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <inttypes.h>
#include "tfm_hal_device_header.h"
#include "region_defs.h"
#include "tfm_arch.h"
#include "tfm_memory_utils.h"
#include "tfm_core_utils.h"
#include "tfm_secure_api.h"
#include "spm_ipc.h"
#include "tfm/tfm_core_svc.h"

#if !defined(__ARM_ARCH_8M_MAIN__) && !defined(__ARM_ARCH_8_1M_MAIN__)
#error "Unsupported ARM Architecture."
#endif

struct tfm_fault_context_s {
    uint32_t R0;
    uint32_t R1;
    uint32_t R2;
    uint32_t R3;
    uint32_t R12;
    uint32_t LR;
    uint32_t ReturnAddress;
    uint32_t RETPSR;
} tfm_fault_context;

/*
 * Stack status at PendSV entry:
 *
 *                                            [ R0 - R3  ]<- PSP
 *                                            [ R12      ]
 *                                            [ LR_of_RA ]
 *                       MSP->[ ........ ]    [ RA       ]
 *                            [ ........ ]    [ XPSR     ]
 *                                            [ ........ ]
 *                                            [ ........ ]
 *
 * Stack status before calling pendsv_do_schedule():
 *
 *                       MSP->[ R4 - R11 ]
 *                            [ PSP      ]--->[ R0 - R3  ]
 *                            [ PSP Limit]    [ R12      ]
 *                            [ R2(dummy)]    [ LR_of_RA ]
 *                            [ LR       ]    [ RA       ]
 *                            [ ........ ]    [ XPSR     ]
 *                            [ ........ ]    [ ........ ]
 *                                            [ ........ ]
 *
 * tfm_pendsv_do_schedule() updates stacked context into current thread and
 * replace stacked context with context of next thread.
 *
 * Scheduler does not support handler mode thread so take PSP/PSP_LIMIT as
 * thread SP/SP_LIMIT. R2 holds dummy data due to stack operation is 8 bytes
 * aligned.
 */
#if defined(__ICCARM__)
#pragma required = tfm_pendsv_do_schedule
#endif

__attribute__((naked)) void PendSV_Handler(void)
{
    __ASM volatile(
        "mrs     r0, psp                    \n"
        "mrs     r1, psplim                 \n"
        "push    {r0, r1, r2, lr}           \n"
        "push    {r4-r11}                   \n"
        "mov     r0, sp                     \n"
        "bl      tfm_pendsv_do_schedule     \n"
        "pop     {r4-r11}                   \n"
        "pop     {r0, r1, r2, lr}           \n"
        "msr     psp, r0                    \n"
        "msr     psplim, r1                 \n"
        "bx      lr                         \n"
    );
}

void tfm_arch_init_actx(struct tfm_arch_ctx_t *p_actx,
                        uint32_t sp, uint32_t sp_limit)
{
    p_actx->sp = sp;
    p_actx->sp_limit = sp_limit;
    p_actx->lr = EXC_RETURN_THREAD_S_PSP;
}

/**
 * \brief Overwrites default Secure fault handler.
 */
void SecureFault_Handler(void)
{
    /* figure out context from which we landed in fault handler */
    uint32_t lr = __get_LR();
    uint32_t sp;

    if (lr & EXC_RETURN_SECURE_STACK) {
        if (lr & EXC_RETURN_STACK_PROCESS) {
            sp = __get_PSP();
        } else {
            sp = __get_MSP();
        }
    } else {
        if (lr & EXC_RETURN_STACK_PROCESS) {
            sp =  __TZ_get_PSP_NS();
        } else {
            sp = __TZ_get_MSP_NS();
        }
    }

    /* Only save the context if sp is valid */
    if ((sp >=  S_DATA_START &&
         sp <=  (S_DATA_LIMIT - sizeof(tfm_fault_context)) + 1) ||
        (sp >= NS_DATA_START &&
         sp <= (NS_DATA_LIMIT - sizeof(tfm_fault_context)) + 1)) {
        spm_memcpy(&tfm_fault_context, (const void *)sp,
                   sizeof(tfm_fault_context));
    }

    ERROR_MSG("Oops... Secure fault!!! You're not going anywhere!");
    while (1) {
        ;
    }
}

#if defined(__ICCARM__)
uint32_t tfm_core_svc_handler(uint32_t *svc_args, uint32_t exc_return);
#pragma required = tfm_core_svc_handler
#endif

__attribute__((naked)) void SVC_Handler(void)
{
    __ASM volatile(
    "MRS     r2, MSP                        \n"
    /* Check store SP in thread mode to r0 */
    "TST     lr, #4                         \n"
    "ITE     EQ                             \n"
    "MOVEQ   r0, r2                         \n"
    "MRSNE   r0, PSP                        \n"
    "MOV     r1, lr                         \n"
    "BL      tfm_core_svc_handler           \n"
    "BX      r0                             \n"
    );
}

/* Reserved for future usage */
__attribute__((naked)) void MemManage_Handler(void)
{
    __ASM volatile("b    .");
}

__attribute__((naked)) void BusFault_Handler(void)
{
    __ASM volatile("b    .");
}
__attribute__((naked)) void UsageFault_Handler(void)
{
    __ASM volatile("b    .");
}

void tfm_arch_prioritize_secure_exception(void)
{
    uint32_t VECTKEY;
    SCB_Type *scb = SCB;
    uint32_t AIRCR;

    /* Set PRIS flag in AIRCR */
    AIRCR = scb->AIRCR;
    VECTKEY = (~AIRCR & SCB_AIRCR_VECTKEYSTAT_Msk);
    scb->AIRCR = SCB_AIRCR_PRIS_Msk |
                 VECTKEY |
                 (AIRCR & ~SCB_AIRCR_VECTKEY_Msk);
}

__attribute__((naked, noinline)) void tfm_arch_clear_fp_status(void)
{
    __ASM volatile(
                   "mrs  r0, control         \n"
                   "bics r0, r0, #4          \n"
                   "msr  control, r0         \n"
                   "isb                      \n"
                   "bx   lr                  \n"
                  );
}
