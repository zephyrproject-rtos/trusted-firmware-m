/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*********** WARNING: This is an auto-generated file. Do not edit! ***********/

#include "cmsis_compiler.h"
#include "tfm_irq_signal_defs.h"

/* Definitions of the signals of the IRQs (if any) */
const struct tfm_core_irq_signal_data_t tfm_core_irq_signals[] = {
#ifdef TFM_PARTITION_TEST_CORE
    { TFM_IRQ_TEST_1_ID, SPM_CORE_IRQ_TEST_1_SIGNAL_TIMER_0_IRQ, TFM_TIMER0_IRQ, 64 },
#endif /* TFM_PARTITION_TEST_CORE */
};

const size_t tfm_core_irq_signals_count = sizeof(tfm_core_irq_signals) /
                                          sizeof(*tfm_core_irq_signals);

/* Definitions of privileged IRQ handlers (if any) */
#ifdef TFM_PARTITION_TEST_CORE
void TFM_TIMER0_IRQ_Handler(void)
{
    __disable_irq();
    /* It is OK to call tfm_irq_handler directly from here, as we are already
     * in handler mode, and we will not be pre-empted as we disabled interrupts
     */
    tfm_irq_handler(TFM_IRQ_TEST_1_ID, SPM_CORE_IRQ_TEST_1_SIGNAL_TIMER_0_IRQ, TFM_TIMER0_IRQ);
    __enable_irq();
}

#endif /* TFM_PARTITION_TEST_CORE */

