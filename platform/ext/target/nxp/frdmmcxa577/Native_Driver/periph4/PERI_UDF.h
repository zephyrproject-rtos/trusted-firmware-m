/*
** ###################################################################
**     Processors:          MCXA286VLL
**                          MCXA286VLQ
**                          MCXA286VPN
**                          MCXA287VLL
**                          MCXA287VLQ
**                          MCXA287VPN
**                          MCXA456VLL
**                          MCXA456VLQ
**                          MCXA456VPN
**                          MCXA457VLL
**                          MCXA457VLQ
**                          MCXA457VPN
**                          MCXA536VLL
**                          MCXA536VLQ
**                          MCXA536VPN
**                          MCXA537VLL
**                          MCXA537VLQ
**                          MCXA537VPN
**                          MCXA556VPN
**                          MCXA566VLL
**                          MCXA566VLQ
**                          MCXA566VPN
**                          MCXA567VLL
**                          MCXA567VLQ
**                          MCXA567VPN
**                          MCXA577VLL
**                          MCXA577VLQ
**                          MCXA577VPN
**
**     Version:             rev. 2.0, 2024-10-29
**     Build:               b251029
**
**     Abstract:
**         CMSIS Peripheral Access Layer for UDF
**
**     Copyright 1997-2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2025 NXP
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 1.0 (2024-03-26)
**         Initial version based on Rev1 DraftC RM
**     - rev. 2.0 (2024-10-29)
**         Change the device header file from single flat file to multiple files based on peripherals,
**         each peripheral with dedicated header file located in periphN folder.
**
** ###################################################################
*/

/*!
 * @file PERI_UDF.h
 * @version 2.0
 * @date 2024-10-29
 * @brief CMSIS Peripheral Access Layer for UDF
 *
 * CMSIS Peripheral Access Layer for UDF
 */

#if !defined(PERI_UDF_H_)
#define PERI_UDF_H_                              /**< Symbol preventing repeated inclusion */

#if (defined(CPU_MCXA286VLL) || defined(CPU_MCXA286VLQ) || defined(CPU_MCXA286VPN))
#include "MCXA286_COMMON.h"
#elif (defined(CPU_MCXA287VLL) || defined(CPU_MCXA287VLQ) || defined(CPU_MCXA287VPN))
#include "MCXA287_COMMON.h"
#elif (defined(CPU_MCXA456VLL) || defined(CPU_MCXA456VLQ) || defined(CPU_MCXA456VPN))
#include "MCXA456_COMMON.h"
#elif (defined(CPU_MCXA457VLL) || defined(CPU_MCXA457VLQ) || defined(CPU_MCXA457VPN))
#include "MCXA457_COMMON.h"
#elif (defined(CPU_MCXA536VLL) || defined(CPU_MCXA536VLQ) || defined(CPU_MCXA536VPN))
#include "MCXA536_COMMON.h"
#elif (defined(CPU_MCXA537VLL) || defined(CPU_MCXA537VLQ) || defined(CPU_MCXA537VPN))
#include "MCXA537_COMMON.h"
#elif (defined(CPU_MCXA556VPN))
#include "MCXA556_COMMON.h"
#elif (defined(CPU_MCXA566VLL) || defined(CPU_MCXA566VLQ) || defined(CPU_MCXA566VPN))
#include "MCXA566_COMMON.h"
#elif (defined(CPU_MCXA567VLL) || defined(CPU_MCXA567VLQ) || defined(CPU_MCXA567VPN))
#include "MCXA567_COMMON.h"
#elif (defined(CPU_MCXA577VLL) || defined(CPU_MCXA577VLQ) || defined(CPU_MCXA577VPN))
#include "MCXA577_COMMON.h"
#else
  #error "No valid CPU defined!"
#endif

/* ----------------------------------------------------------------------------
   -- Device Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup Peripheral_access_layer Device Peripheral Access Layer
 * @{
 */


/*
** Start of section using anonymous unions
*/

#if defined(__ARMCC_VERSION)
  #if (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic push
  #else
    #pragma push
    #pragma anon_unions
  #endif
#elif defined(__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined(__IAR_SYSTEMS_ICC__)
  #pragma language=extended
#else
  #error Not supported compiler type
#endif

/* ----------------------------------------------------------------------------
   -- UDF Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup UDF_Peripheral_Access_Layer UDF Peripheral Access Layer
 * @{
 */

/** UDF - Register Layout Typedef */
typedef struct {
  __IO uint32_t UDF_CTRL;                          /**< Control register, offset: 0x0 */
  __I  uint32_t UDF_STATUS;                        /**< Status register, offset: 0x4 */
  __O  uint32_t UDF_WR_DATA;                       /**< Data In Register, offset: 0x8 */
  __I  uint32_t UDF_RD_DATA;                       /**< Data Out Register, offset: 0xC */
} UDF_Type;

/* ----------------------------------------------------------------------------
   -- UDF Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup UDF_Register_Masks UDF Register Masks
 * @{
 */

/*! @name UDF_CTRL - Control register */
/*! @{ */

#define UDF_UDF_CTRL_salt_MASK                   (0xFFFFU)
#define UDF_UDF_CTRL_salt_SHIFT                  (0U)
/*! salt - Bits are internally XORed with i_custom */
#define UDF_UDF_CTRL_salt(x)                     (((uint32_t)(((uint32_t)(x)) << UDF_UDF_CTRL_salt_SHIFT)) & UDF_UDF_CTRL_salt_MASK)

#define UDF_UDF_CTRL_lock_MASK                   (0x70000U)
#define UDF_UDF_CTRL_lock_SHIFT                  (16U)
/*! lock - Lock access to UDF */
#define UDF_UDF_CTRL_lock(x)                     (((uint32_t)(((uint32_t)(x)) << UDF_UDF_CTRL_lock_SHIFT)) & UDF_UDF_CTRL_lock_MASK)

#define UDF_UDF_CTRL_reserved21_MASK             (0x380000U)
#define UDF_UDF_CTRL_reserved21_SHIFT            (19U)
/*! reserved21 - RFU */
#define UDF_UDF_CTRL_reserved21(x)               (((uint32_t)(((uint32_t)(x)) << UDF_UDF_CTRL_reserved21_SHIFT)) & UDF_UDF_CTRL_reserved21_MASK)

#define UDF_UDF_CTRL_udf_en_MASK                 (0x1C00000U)
#define UDF_UDF_CTRL_udf_en_SHIFT                (22U)
/*! udf_en - Enable the UDF block */
#define UDF_UDF_CTRL_udf_en(x)                   (((uint32_t)(((uint32_t)(x)) << UDF_UDF_CTRL_udf_en_SHIFT)) & UDF_UDF_CTRL_udf_en_MASK)

#define UDF_UDF_CTRL_reserved25_MASK             (0x2000000U)
#define UDF_UDF_CTRL_reserved25_SHIFT            (25U)
/*! reserved25 - RFU */
#define UDF_UDF_CTRL_reserved25(x)               (((uint32_t)(((uint32_t)(x)) << UDF_UDF_CTRL_reserved25_SHIFT)) & UDF_UDF_CTRL_reserved25_MASK)

#define UDF_UDF_CTRL_reserved27_MASK             (0xC000000U)
#define UDF_UDF_CTRL_reserved27_SHIFT            (26U)
/*! reserved27 - RFU */
#define UDF_UDF_CTRL_reserved27(x)               (((uint32_t)(((uint32_t)(x)) << UDF_UDF_CTRL_reserved27_SHIFT)) & UDF_UDF_CTRL_reserved27_MASK)

#define UDF_UDF_CTRL_flush_MASK                  (0x70000000U)
#define UDF_UDF_CTRL_flush_SHIFT                 (28U)
/*! flush - Flush UDF and return to reset state */
#define UDF_UDF_CTRL_flush(x)                    (((uint32_t)(((uint32_t)(x)) << UDF_UDF_CTRL_flush_SHIFT)) & UDF_UDF_CTRL_flush_MASK)

#define UDF_UDF_CTRL_reserved31_MASK             (0x80000000U)
#define UDF_UDF_CTRL_reserved31_SHIFT            (31U)
/*! reserved31 - reserved */
#define UDF_UDF_CTRL_reserved31(x)               (((uint32_t)(((uint32_t)(x)) << UDF_UDF_CTRL_reserved31_SHIFT)) & UDF_UDF_CTRL_reserved31_MASK)
/*! @} */

/*! @name UDF_STATUS - Status register */
/*! @{ */

#define UDF_UDF_STATUS_o_status_MASK             (0x1FU)
#define UDF_UDF_STATUS_o_status_SHIFT            (0U)
/*! o_status - Status bits
 *  0b00001..5'b00001 = Reset
 *  0b00010..5'b00010 = Init
 *  0b00100..5'b00100 = Warmup
 *  0b01000..5'b01000 = Ready
 *  0b10000..5'b10000 = Error
 */
#define UDF_UDF_STATUS_o_status(x)               (((uint32_t)(((uint32_t)(x)) << UDF_UDF_STATUS_o_status_SHIFT)) & UDF_UDF_STATUS_o_status_MASK)

#define UDF_UDF_STATUS_rsv_MASK                  (0x7FFFFFE0U)
#define UDF_UDF_STATUS_rsv_SHIFT                 (5U)
/*! rsv - RFU */
#define UDF_UDF_STATUS_rsv(x)                    (((uint32_t)(((uint32_t)(x)) << UDF_UDF_STATUS_rsv_SHIFT)) & UDF_UDF_STATUS_rsv_MASK)

#define UDF_UDF_STATUS_o_wait_MASK               (0x80000000U)
#define UDF_UDF_STATUS_o_wait_SHIFT              (31U)
/*! o_wait - Indicates UDF is processing data */
#define UDF_UDF_STATUS_o_wait(x)                 (((uint32_t)(((uint32_t)(x)) << UDF_UDF_STATUS_o_wait_SHIFT)) & UDF_UDF_STATUS_o_wait_MASK)
/*! @} */

/*! @name UDF_WR_DATA - Data In Register */
/*! @{ */

#define UDF_UDF_WR_DATA_i_dat_MASK               (0xFFFFFFFFU)
#define UDF_UDF_WR_DATA_i_dat_SHIFT              (0U)
#define UDF_UDF_WR_DATA_i_dat(x)                 (((uint32_t)(((uint32_t)(x)) << UDF_UDF_WR_DATA_i_dat_SHIFT)) & UDF_UDF_WR_DATA_i_dat_MASK)
/*! @} */

/*! @name UDF_RD_DATA - Data Out Register */
/*! @{ */

#define UDF_UDF_RD_DATA_o_dat_MASK               (0xFFFFFFFFU)
#define UDF_UDF_RD_DATA_o_dat_SHIFT              (0U)
#define UDF_UDF_RD_DATA_o_dat(x)                 (((uint32_t)(((uint32_t)(x)) << UDF_UDF_RD_DATA_o_dat_SHIFT)) & UDF_UDF_RD_DATA_o_dat_MASK)
/*! @} */


/*!
 * @}
 */ /* end of group UDF_Register_Masks */


/*!
 * @}
 */ /* end of group UDF_Peripheral_Access_Layer */


/*
** End of section using anonymous unions
*/

#if defined(__ARMCC_VERSION)
  #if (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic pop
  #else
    #pragma pop
  #endif
#elif defined(__GNUC__)
  /* leave anonymous unions enabled */
#elif defined(__IAR_SYSTEMS_ICC__)
  #pragma language=default
#else
  #error Not supported compiler type
#endif

/*!
 * @}
 */ /* end of group Peripheral_access_layer */


#endif  /* PERI_UDF_H_ */

