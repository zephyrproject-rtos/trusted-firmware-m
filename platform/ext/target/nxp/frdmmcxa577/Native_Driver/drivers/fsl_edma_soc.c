/*
 * Copyright 2025 NXP
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_edma_soc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.edma_soc"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
extern void DMA0_CH0_DriverIRQHandler(void);
extern void DMA0_CH1_DriverIRQHandler(void);
extern void DMA0_CH2_DriverIRQHandler(void);
extern void DMA0_CH3_DriverIRQHandler(void);
extern void DMA0_CH4_DriverIRQHandler(void);
extern void DMA0_CH5_DriverIRQHandler(void);
extern void DMA0_CH6_DriverIRQHandler(void);
extern void DMA0_CH7_DriverIRQHandler(void);
extern void DMA0_CH8_DriverIRQHandler(void);
extern void DMA0_CH9_DriverIRQHandler(void);
extern void DMA0_CH10_DriverIRQHandler(void);
extern void DMA0_CH11_DriverIRQHandler(void);
extern void DMA1_CH0_DriverIRQHandler(void);
extern void DMA1_CH1_DriverIRQHandler(void);
extern void DMA1_CH2_DriverIRQHandler(void);
extern void DMA1_CH3_DriverIRQHandler(void);
extern void EDMA_DriverIRQHandler(uint32_t instance, uint32_t channel);
/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * brief DMA instance 0, channel 0 IRQ handler.
 *
 */
void DMA0_CH0_DriverIRQHandler(void)
{
    /* Instance 0 channel 0 */
    EDMA_DriverIRQHandler(0U, 0U);
}

/*!
 * brief DMA instance 0, channel 1 IRQ handler.
 *
 */
void DMA0_CH1_DriverIRQHandler(void)
{
    /* Instance 0 channel 1 */
    EDMA_DriverIRQHandler(0U, 1U);
}

/*!
 * brief DMA instance 0, channel 2 IRQ handler.
 *
 */
void DMA0_CH2_DriverIRQHandler(void)
{
    /* Instance 0 channel 2 */
    EDMA_DriverIRQHandler(0U, 2U);
}

/*!
 * brief DMA instance 0, channel 3 IRQ handler.
 *
 */
void DMA0_CH3_DriverIRQHandler(void)
{
    /* Instance 0 channel 3 */
    EDMA_DriverIRQHandler(0U, 3U);
}

/*!
 * brief DMA instance 0, channel 4 IRQ handler.
 *
 */
void DMA0_CH4_DriverIRQHandler(void)
{
    /* Instance 0 channel 4 */
    EDMA_DriverIRQHandler(0U, 4U);
}

/*!
 * brief DMA instance 0, channel 5 IRQ handler.
 *
 */
void DMA0_CH5_DriverIRQHandler(void)
{
    /* Instance 0 channel 5 */
    EDMA_DriverIRQHandler(0U, 5U);
}

/*!
 * brief DMA instance 0, channel 6 IRQ handler.
 *
 */
void DMA0_CH6_DriverIRQHandler(void)
{
    /* Instance 0 channel 6 */
    EDMA_DriverIRQHandler(0U, 6U);
}

/*!
 * brief DMA instance 0, channel 7 IRQ handler.
 *
 */
void DMA0_CH7_DriverIRQHandler(void)
{
    /* Instance 0 channel 7 */
    EDMA_DriverIRQHandler(0U, 7U);
}

/*!
 * brief DMA instance 0, channel 8 IRQ handler.
 *
 */
void DMA0_CH8_DriverIRQHandler(void)
{
    /* Instance 0 channel 8 */
    EDMA_DriverIRQHandler(0U, 8U);
}

/*!
 * brief DMA instance 0, channel 9 IRQ handler.
 *
 */
void DMA0_CH9_DriverIRQHandler(void)
{
    /* Instance 0 channel 9 */
    EDMA_DriverIRQHandler(0U, 9U);
}

/*!
 * brief DMA instance 0, channel 10 IRQ handler.
 *
 */
void DMA0_CH10_DriverIRQHandler(void)
{
    /* Instance 0 channel 10 */
    EDMA_DriverIRQHandler(0U, 10U);
}

/*!
 * brief DMA instance 0, channel 11 IRQ handler.
 *
 */
void DMA0_CH11_DriverIRQHandler(void)
{
    /* Instance 0 channel 11 */
    EDMA_DriverIRQHandler(0U, 11U);
}

/*!
 * brief DMA instance 1, channel 0 IRQ handler.
 *
 */
void DMA1_CH0_DriverIRQHandler(void)
{
    /* Instance 1 channel 0 */
    EDMA_DriverIRQHandler(1U, 0U);
}

/*!
 * brief DMA instance 1, channel 1 IRQ handler.
 *
 */
void DMA1_CH1_DriverIRQHandler(void)
{
    /* Instance 1 channel 1 */
    EDMA_DriverIRQHandler(1U, 1U);
}

/*!
 * brief DMA instance 1, channel 2 IRQ handler.
 *
 */
void DMA1_CH2_DriverIRQHandler(void)
{
    /* Instance 1 channel 2 */
    EDMA_DriverIRQHandler(1U, 2U);
}

/*!
 * brief DMA instance 1, channel 3 IRQ handler.
 *
 */
void DMA1_CH3_DriverIRQHandler(void)
{
    /* Instance 1 channel 3 */
    EDMA_DriverIRQHandler(1U, 3U);
}

/*!
 * brief Enable or disable EDMA request for SoC.
 *
 */
void EDMA_SocRequestEnable(DMA_Type *base, uint32_t request, bool value) 
{
    /* Calculate register index based on DMA instance and request number */
    uint32_t regIndex = ((base == DMA0) ? 0U : 5U) + (request / 32U);
    /* Calculate bit position within the register */
    uint32_t bitPos = request % 32U;
    /* Ensure the register index is within valid range */
    assert(regIndex < AHBSC_SEC_GP_REG_COUNT);
    assert(request < 160U);
 
    if (value)
    {
        /* Set the bit to enable the EDMA request */
        AHBSC->SEC_GP_REG[regIndex] |= (1U << bitPos);
    }
    else
    {
        /* Clear the bit to disable the EDMA request */
        AHBSC->SEC_GP_REG[regIndex] &= ~(1U << bitPos);
    }
}
