/*
 * Copyright (c) 2021-2024, Arm Limited. All rights reserved.
 * Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "internal_status_code.h"
#include "tfm_hal_device_header.h"
#include "device_definition.h"
#include "spm.h"
#include "tfm_hal_interrupt.h"
#include "tfm_peripherals_def.h"
#include "tfm_multi_core.h"
#include "interrupt.h"
#include "load/interrupt_defs.h"
#include "platform_irq.h"
#ifdef TFM_MULTI_CORE_TOPOLOGY
#include "rse_comms_hal.h"
#endif

static struct irq_t timer0_irq = {0};

void TFM_TIMER0_IRQ_Handler(void)
{
    spm_handle_interrupt(timer0_irq.p_pt, timer0_irq.p_ildi);
}

enum tfm_hal_status_t tfm_timer0_irq_init(void *p_pt,
                                          const struct irq_load_info_t *p_ildi)
{
    timer0_irq.p_ildi = p_ildi;
    timer0_irq.p_pt = p_pt;

    NVIC_SetPriority(TFM_TIMER0_IRQ, DEFAULT_IRQ_PRIORITY);
    NVIC_ClearTargetState(TFM_TIMER0_IRQ);
    NVIC_DisableIRQ(TFM_TIMER0_IRQ);

    return TFM_HAL_SUCCESS;
}

#ifdef TFM_MULTI_CORE_TOPOLOGY
static struct irq_t mbox_irq_info[2] = {0};

/* Platform specific inter-processor communication interrupt handler. */
void CMU_MHU0_Receiver_Handler(void)
{
    (void)tfm_multi_core_hal_receive(&MHU_AP_MONITOR_TO_RSE_DEV,
                                     &MHU_RSE_TO_AP_MONITOR_DEV,
                                     mbox_irq_info[0].p_ildi->source);

    /*
     * SPM will send a MAILBOX_INTERRUPT_SIGNAL to the corresponding partition
     * indicating that a message has arrived and can be processed.
     */
    spm_handle_interrupt(mbox_irq_info[0].p_pt, mbox_irq_info[0].p_ildi);
}

#ifdef MHU_AP_NS_TO_RSE
/* Platform specific inter-processor communication interrupt handler. */
void CMU_MHU1_Receiver_Handler(void)
{
    (void)tfm_multi_core_hal_receive(&MHU_AP_NS_TO_RSE_DEV,
                                     &MHU_RSE_TO_AP_NS_DEV,
                                     mbox_irq_info[1].p_ildi->source);

    /*
     * SPM will send a MAILBOX_INTERRUPT_SIGNAL to the corresponding partition
     * indicating that a message has arrived and can be processed.
     */
    spm_handle_interrupt(mbox_irq_info[1].p_pt, mbox_irq_info[1].p_ildi);
}
#endif /* MHU_AP_NS_TO_RSE */

enum tfm_hal_status_t mailbox_irq_init(void *p_pt,
                                       const struct irq_load_info_t *p_ildi)
{
    mbox_irq_info[0].p_pt = p_pt;
    mbox_irq_info[0].p_ildi = p_ildi;

    /* Set MHU interrupt priority to the same as PendSV (the lowest)
     * TODO: Consider advantages/disadvantages of setting it one higher
     */
    NVIC_SetPriority(CMU_MHU0_Receiver_IRQn, NVIC_GetPriority(PendSV_IRQn));

    /* CMU_MHU0 is a secure peripheral, so its IRQs have to target S state */
    NVIC_ClearTargetState(CMU_MHU0_Receiver_IRQn);
    NVIC_DisableIRQ(CMU_MHU0_Receiver_IRQn);

    if (tfm_multi_core_register_client_id_range(&MHU_RSE_TO_AP_MONITOR_DEV,
                                                p_ildi->client_id_base,
                                                p_ildi->client_id_limit)
        != SPM_SUCCESS) {
        return TFM_HAL_ERROR_INVALID_INPUT;
    }
    return TFM_HAL_SUCCESS;
}

#ifdef MHU_AP_NS_TO_RSE
enum tfm_hal_status_t mailbox_irq_1_init(void *p_pt,
                                         const struct irq_load_info_t *p_ildi)
{
    mbox_irq_info[1].p_pt = p_pt;
    mbox_irq_info[1].p_ildi = p_ildi;

    /* Set MHU interrupt priority to the same as PendSV (the lowest)
     * TODO: Consider advantages/disadvantages of setting it one higher
     */
    NVIC_SetPriority(CMU_MHU1_Receiver_IRQn, NVIC_GetPriority(PendSV_IRQn));

    /* CMU_MHU1 is a secure peripheral, so its IRQs have to target S state */
    NVIC_ClearTargetState(CMU_MHU1_Receiver_IRQn);
    NVIC_DisableIRQ(CMU_MHU1_Receiver_IRQn);

    if (tfm_multi_core_register_client_id_range(&MHU_RSE_TO_AP_NS_DEV,
                                                p_ildi->client_id_base,
                                                p_ildi->client_id_limit)
        != SPM_SUCCESS) {
        return TFM_HAL_ERROR_INVALID_INPUT;
    }
    return TFM_HAL_SUCCESS;
}
#else /* MHU_AP_NS_TO_RSE */
enum tfm_hal_status_t mailbox_irq_1_init(void *p_pt,
                                         const struct irq_load_info_t *p_ildi)
{
    (void)p_pt;
    (void)p_ildi;

    return TFM_HAL_ERROR_NOT_SUPPORTED;
}
#endif /* MHU_AP_NS_TO_RSE */
#endif /* TFM_MULTI_CORE_TOPOLOGY */

static struct irq_t dma0_ch0_irq = {0};

void DMA_Combined_S_Handler(void)
{
    spm_handle_interrupt(dma0_ch0_irq.p_pt, dma0_ch0_irq.p_ildi);
}

enum tfm_hal_status_t tfm_dma0_combined_s_irq_init(void *p_pt,
                                          struct irq_load_info_t *p_ildi)
{
    dma0_ch0_irq.p_ildi = p_ildi;
    dma0_ch0_irq.p_pt = p_pt;

    NVIC_SetPriority(TFM_DMA0_COMBINED_S_IRQ, DEFAULT_IRQ_PRIORITY);
    NVIC_ClearTargetState(TFM_DMA0_COMBINED_S_IRQ);
    NVIC_DisableIRQ(TFM_DMA0_COMBINED_S_IRQ);

    return TFM_HAL_SUCCESS;
}
