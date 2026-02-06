/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_EDMA_SOC_H_
#define _FSL_EDMA_SOC_H_

#include "fsl_common.h"

/*!
 * @addtogroup edma_soc
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @name Driver version */
/*@{*/
/*! @brief Driver version 2.0.0. */
#define FSL_EDMA_SOC_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/*!@brief DMA IP version */
#define FSL_EDMA_SOC_IP_DMA3 (1)
#define FSL_EDMA_SOC_IP_DMA4 (0)

/*!@brief DMA base table */
#define EDMA_BASE_PTRS {DMA0, DMA1}

#define EDMA_CHN_IRQS                                                                                 \
    {                                                                                                 \
        {DMA0_CH0_IRQn, DMA0_CH1_IRQn, DMA0_CH2_IRQn, DMA0_CH3_IRQn, DMA0_CH4_IRQn,  DMA0_CH5_IRQn,   \
         DMA0_CH6_IRQn, DMA0_CH7_IRQn, DMA0_CH8_IRQn, DMA0_CH9_IRQn, DMA0_CH10_IRQn, DMA0_CH11_IRQn}, \
        {                                                                                             \
            DMA1_CH0_IRQn, DMA1_CH1_IRQn, DMA1_CH2_IRQn, DMA1_CH3_IRQn                                \
        }                                                                                             \
    }

/*!@brief EDMA base address convert macro */
#define EDMA_CHANNEL_OFFSET           0x1000U
#define EDMA_CHANNEL_ARRAY_STEP(base) (0x1000U)

/*******************************************************************************
 * API
 ******************************************************************************/
/**
 * @brief Enable or disable EDMA request for SoC.
 *
 * This function enables or disables a specific EDMA request by setting or clearing
 * the corresponding bit in the AHBSC security general purpose register.
 *
 * @param base DMA peripheral base address (DMA0 or other DMA instance).
 * @param request The EDMA request number to enable or disable.
 * @param value true to enable the request, false to disable it.
 */
void EDMA_SocRequestEnable(DMA_Type *base, uint32_t request, bool value);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/*!
 * @}
 */

#endif /* _FSL_EDMA_SOC_H_ */
