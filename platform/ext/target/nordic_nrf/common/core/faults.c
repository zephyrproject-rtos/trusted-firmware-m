/*
 * Copyright (c) 2018-2024, Arm Limited. All rights reserved.
 * Copyright (c) 2020, Nordic Semiconductor ASA. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "tfm_hal_device_header.h"
#include "spu.h"
#include "utilities.h"
#include "nrf_exception_info.h"
/* "exception_info.h" must be the last include because of the IAR pragma */
#include "exception_info.h"

void SPU_Handler(void)
{
#ifdef TFM_EXCEPTION_INFO_DUMP
    nrf_exception_info_store_context();
#endif
    /* Clear SPU interrupt flag and pending SPU IRQ */
    spu_clear_events();

    NVIC_ClearPendingIRQ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) - NVIC_USER_IRQ_OFFSET);

    tfm_core_panic();
}

__attribute__((naked)) void SPU_IRQHandler(void)
{
    EXCEPTION_INFO();

    __ASM volatile(
        "BL        SPU_Handler             \n"
        "B         .                       \n"
    );
}

#ifdef NRF_SPU00
__attribute__((naked)) void SPU00_IRQHandler(void)
{
    EXCEPTION_INFO();

    __ASM volatile(
        "BL        SPU_Handler             \n"
        "B         .                       \n"
    );
}
#endif

#ifdef NRF_SPU10
__attribute__((naked)) void SPU10_IRQHandler(void)
{
    EXCEPTION_INFO();

    __ASM volatile(
        "BL        SPU_Handler             \n"
        "B         .                       \n"
    );
}
#endif

#ifdef NRF_SPU20
__attribute__((naked)) void SPU20_IRQHandler(void)
{
    EXCEPTION_INFO();

    __ASM volatile(
        "BL        SPU_Handler             \n"
        "B         .                       \n"
    );
}
#endif

#ifdef NRF_SPU30
__attribute__((naked)) void SPU30_IRQHandler(void)
{
    EXCEPTION_INFO();

    __ASM volatile(
        "BL        SPU_Handler             \n"
        "B         .                       \n"
    );
}
#endif

#ifdef NRF_MPC00
__attribute__((naked)) void MPC_Handler(void)
{
    EXCEPTION_INFO();

#ifdef TFM_EXCEPTION_INFO_DUMP
    nrf_exception_info_store_context();
#endif

    /* Clear MPC interrupt flag and pending MPC IRQ */
    mpc_clear_events();

    NVIC_ClearPendingIRQ(MPC00_IRQn);

    tfm_core_panic();

    __ASM volatile(
    "B         .                       \n"
    );
}

void MPC00_IRQHandler(void)
{
    /* Address 0xFFFFFFFE is used by TF-M as a return address in some cases
     * (e.g., THRD_GENERAL_EXIT). This causes the debugger to access this
     * address when analyzing stack frames upon hitting a breakpoint in TF-M.
     * Attempting to access this address triggers the MPC MEMACCERR event,
     * disrupting debugging. To prevent this, we ignore events from this address.
     * Note that this does not affect exception information in MPC_Handler,
     * except for scratch registers (R0-R3).
     **/
    if( nrf_mpc_event_check(NRF_MPC00, NRF_MPC_EVENT_MEMACCERR)){
        if(NRF_MPC00->MEMACCERR.ADDRESS == 0xFFFFFFFE)
        {
            mpc_clear_events();
            NVIC_ClearPendingIRQ(MPC00_IRQn);
            return;
        }
    }

    MPC_Handler();
}
#endif
