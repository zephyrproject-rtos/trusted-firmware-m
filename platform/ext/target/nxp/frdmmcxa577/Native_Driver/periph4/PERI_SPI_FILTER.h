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
**         CMSIS Peripheral Access Layer for SPI_FILTER
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
 * @file PERI_SPI_FILTER.h
 * @version 2.0
 * @date 2024-10-29
 * @brief CMSIS Peripheral Access Layer for SPI_FILTER
 *
 * CMSIS Peripheral Access Layer for SPI_FILTER
 */

#if !defined(PERI_SPI_FILTER_H_)
#define PERI_SPI_FILTER_H_                       /**< Symbol preventing repeated inclusion */

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
   -- SPI_FILTER Peripheral Access Layer
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SPI_FILTER_Peripheral_Access_Layer SPI_FILTER Peripheral Access Layer
 * @{
 */

/** SPI_FILTER - Register Layout Typedef */
typedef struct {
  __IO uint32_t CR;                                /**< Control, offset: 0x0 */
  __I  uint32_t ISR;                               /**< Interrupt Register, offset: 0x4 */
  __IO uint32_t IMR;                               /**< Interrupt Mask Register, offset: 0x8 */
  __I  uint32_t SR;                                /**< Port Status Register, offset: 0xC */
  __IO uint32_t TR;                                /**< Test Register, offset: 0x10 */
  __IO uint32_t P0FAR1;                            /**< PORT0 Filter Address Region 1, offset: 0x14 */
  __IO uint32_t P0FAR2;                            /**< PORT0 Filter Address Region 2, offset: 0x18 */
  __IO uint32_t P0FAR3;                            /**< PORT0 Filter Address Region 3, offset: 0x1C */
  __IO uint32_t P0FAR4;                            /**< PORT0 Filter Address Region 4, offset: 0x20 */
  __IO uint32_t P0FAR5;                            /**< PORT0 Filter Address Region 5, offset: 0x24 */
  __IO uint32_t P0FAR6;                            /**< PORT0 Filter Address Region 6, offset: 0x28 */
  __IO uint32_t P1FAR1;                            /**< PORT1 Filter Address Region 1, offset: 0x2C */
  __IO uint32_t P1FAR2;                            /**< PORT1 Filter Address Region 2, offset: 0x30 */
  __IO uint32_t P1FAR3;                            /**< PORT1 Filter Address Region 3, offset: 0x34 */
  __IO uint32_t P1FAR4;                            /**< PORT1 Filter Address Region 4, offset: 0x38 */
  __IO uint32_t P1FAR5;                            /**< PORT1 Filter Address Region 5, offset: 0x3C */
  __IO uint32_t P1FAR6;                            /**< PORT1 Filter Address Region 6, offset: 0x40 */
       uint8_t RESERVED_0[4];
  __IO uint32_t POPCODE0;                          /**< Programmable OP Code0, offset: 0x48 */
  __IO uint32_t POPCODE1;                          /**< Programmable OP Code1, offset: 0x4C */
  __IO uint32_t POPCODE2;                          /**< Programmable OP Code2, offset: 0x50 */
  __IO uint32_t POPCODE3;                          /**< Programmable OP Code3, offset: 0x54 */
  __IO uint32_t POPCODE4;                          /**< Programmable OP Code4, offset: 0x58 */
  __I  uint32_t P0BOC;                             /**< P0 Blocked Op Code, offset: 0x5C */
  __I  uint32_t P1BOC;                             /**< P1 Blocked Op Code, offset: 0x60 */
  __IO uint32_t P0MAM;                             /**< PORT0 Max Address Mask, offset: 0x64 */
  __IO uint32_t P1MAM;                             /**< PORT1 Max Address Mask, offset: 0x68 */
} SPI_FILTER_Type;

/* ----------------------------------------------------------------------------
   -- SPI_FILTER Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SPI_FILTER_Register_Masks SPI_FILTER Register Masks
 * @{
 */

/*! @name CR - Control */
/*! @{ */

#define SPI_FILTER_CR_P1_SINGLE_FLASH_MASK       (0x1U)
#define SPI_FILTER_CR_P1_SINGLE_FLASH_SHIFT      (0U)
/*! P1_SINGLE_FLASH - Flash memory organization
 *  0b0..Sets flash memory organization to Dual Flash mode.
 *  0b1..Sets flash memory organization to Single Flash mode.
 */
#define SPI_FILTER_CR_P1_SINGLE_FLASH(x)         (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P1_SINGLE_FLASH_SHIFT)) & SPI_FILTER_CR_P1_SINGLE_FLASH_MASK)

#define SPI_FILTER_CR_P1_SINGLE_FLASH_CHIP_SELECT_MASK (0x2U)
#define SPI_FILTER_CR_P1_SINGLE_FLASH_CHIP_SELECT_SHIFT (1U)
/*! P1_SINGLE_FLASH_CHIP_SELECT - Single Flash mode Chip Select
 *  0b0..Enables output to CS0 as the active chip.
 *  0b1..Enables output to CS1 as the active chip.
 */
#define SPI_FILTER_CR_P1_SINGLE_FLASH_CHIP_SELECT(x) (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P1_SINGLE_FLASH_CHIP_SELECT_SHIFT)) & SPI_FILTER_CR_P1_SINGLE_FLASH_CHIP_SELECT_MASK)

#define SPI_FILTER_CR_P1_SINGLE_FLASH_ALLOW_WRITE_MASK (0x4U)
#define SPI_FILTER_CR_P1_SINGLE_FLASH_ALLOW_WRITE_SHIFT (2U)
/*! P1_SINGLE_FLASH_ALLOW_WRITE - Single Flash Allow Write
 *  0b0..Normal operation. Writes are not allowed to unfiltered regions.
 *  0b1..Writes are allowed to unfiltered regions.
 */
#define SPI_FILTER_CR_P1_SINGLE_FLASH_ALLOW_WRITE(x) (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P1_SINGLE_FLASH_ALLOW_WRITE_SHIFT)) & SPI_FILTER_CR_P1_SINGLE_FLASH_ALLOW_WRITE_MASK)

#define SPI_FILTER_CR_P0_SINGLE_FLASH_MASK       (0x100U)
#define SPI_FILTER_CR_P0_SINGLE_FLASH_SHIFT      (8U)
/*! P0_SINGLE_FLASH - Flash memory organization
 *  0b0..Sets flash memory organization to Dual Flash mode.
 *  0b1..Sets flash memory organization to Single Flash mode.
 */
#define SPI_FILTER_CR_P0_SINGLE_FLASH(x)         (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P0_SINGLE_FLASH_SHIFT)) & SPI_FILTER_CR_P0_SINGLE_FLASH_MASK)

#define SPI_FILTER_CR_P0_SINGLE_FLASH_CHIP_SELECT_MASK (0x200U)
#define SPI_FILTER_CR_P0_SINGLE_FLASH_CHIP_SELECT_SHIFT (9U)
/*! P0_SINGLE_FLASH_CHIP_SELECT - Single Flash mode Chip Select
 *  0b0..Enables output to CS0 as the active chip.
 *  0b1..Enables output to CS1 as the active chip.
 */
#define SPI_FILTER_CR_P0_SINGLE_FLASH_CHIP_SELECT(x) (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P0_SINGLE_FLASH_CHIP_SELECT_SHIFT)) & SPI_FILTER_CR_P0_SINGLE_FLASH_CHIP_SELECT_MASK)

#define SPI_FILTER_CR_P0_SINGLE_FLASH_ALLOW_WRITE_MASK (0x400U)
#define SPI_FILTER_CR_P0_SINGLE_FLASH_ALLOW_WRITE_SHIFT (10U)
/*! P0_SINGLE_FLASH_ALLOW_WRITE - Single Flash Allow Write
 *  0b0..Normal operation. Writes are not allowed to unfiltered regions.
 *  0b1..Writes are allowed to unfiltered regions.
 */
#define SPI_FILTER_CR_P0_SINGLE_FLASH_ALLOW_WRITE(x) (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P0_SINGLE_FLASH_ALLOW_WRITE_SHIFT)) & SPI_FILTER_CR_P0_SINGLE_FLASH_ALLOW_WRITE_MASK)

#define SPI_FILTER_CR_P1_ACT_SEL_MASK            (0x10000U)
#define SPI_FILTER_CR_P1_ACT_SEL_SHIFT           (16U)
/*! P1_ACT_SEL - Active Chip Select for Flash memory
 *  0b0..Sets CS0 as the read location for the firmware image and sets CS1 as the write location for the FW image.
 *  0b1..Sets CS1 as the read location for the firmware image and sets CS0 as the write location for the FW image.
 */
#define SPI_FILTER_CR_P1_ACT_SEL(x)              (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P1_ACT_SEL_SHIFT)) & SPI_FILTER_CR_P1_ACT_SEL_MASK)

#define SPI_FILTER_CR_P1_WE_BYTE_MD_MASK         (0x20000U)
#define SPI_FILTER_CR_P1_WE_BYTE_MD_SHIFT        (17U)
/*! P1_WE_BYTE_MD - Write enable switch for address byte mode
 *  0b0..Write Enable command is not required to switch address byte mode.
 *  0b1..Write Enable command is required prior to switching address byte mode.
 */
#define SPI_FILTER_CR_P1_WE_BYTE_MD(x)           (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P1_WE_BYTE_MD_SHIFT)) & SPI_FILTER_CR_P1_WE_BYTE_MD_MASK)

#define SPI_FILTER_CR_P1_PERM_BYTE_MD_MASK       (0x40000U)
#define SPI_FILTER_CR_P1_PERM_BYTE_MD_SHIFT      (18U)
/*! P1_PERM_BYTE_MD - Address Byte change mode
 *  0b0..Normal Operation. Address byte mode can be changed as usual.
 *  0b1..Address byte mode cannot be changed.
 */
#define SPI_FILTER_CR_P1_PERM_BYTE_MD(x)         (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P1_PERM_BYTE_MD_SHIFT)) & SPI_FILTER_CR_P1_PERM_BYTE_MD_MASK)

#define SPI_FILTER_CR_P1_BYTE_SEL_MASK           (0x80000U)
#define SPI_FILTER_CR_P1_BYTE_SEL_SHIFT          (19U)
/*! P1_BYTE_SEL - Address Byte Select
 *  0b0..Sets the filter to 3-byte address mode.
 *  0b1..Sets the filter to 4-byte address mode.
 */
#define SPI_FILTER_CR_P1_BYTE_SEL(x)             (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P1_BYTE_SEL_SHIFT)) & SPI_FILTER_CR_P1_BYTE_SEL_MASK)

#define SPI_FILTER_CR_P1_BYTE_SEL_MD_MASK        (0x100000U)
#define SPI_FILTER_CR_P1_BYTE_SEL_MD_SHIFT       (20U)
/*! P1_BYTE_SEL_MD - Address Byte Select Mode
 *  0b0..Normal Operation. The Address byte mode is determined by the Op Code Command.
 *  0b1..Sets the Byte Mode to 3-byte and gives control of the function to the P1_BYTE_SEL in the P1 Filter
 *       Control Register. In this mode, any changes to P1_BYTE_SEL are latched and the internal state of P1_BYTE_SEL
 *       will retain this value when P1_BYTE_SEL_MD is cleared.
 */
#define SPI_FILTER_CR_P1_BYTE_SEL_MD(x)          (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P1_BYTE_SEL_MD_SHIFT)) & SPI_FILTER_CR_P1_BYTE_SEL_MD_MASK)

#define SPI_FILTER_CR_P1_DIRTY_CLR_MASK          (0x200000U)
#define SPI_FILTER_CR_P1_DIRTY_CLR_SHIFT         (21U)
/*! P1_DIRTY_CLR - Clear dirty state for P1 */
#define SPI_FILTER_CR_P1_DIRTY_CLR(x)            (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P1_DIRTY_CLR_SHIFT)) & SPI_FILTER_CR_P1_DIRTY_CLR_MASK)

#define SPI_FILTER_CR_P1_FLT_EN_MASK             (0x400000U)
#define SPI_FILTER_CR_P1_FLT_EN_SHIFT            (22U)
/*! P1_FLT_EN - Filter Enable bit
 *  0b0..Filter Disabled. All CS Disable outputs are asserted so that host access to the flash devices is
 *       disabled. CS# inputs to the filter are disabled so that the filter state machine is locked in its current state.
 *       All register controls are functional.
 *  0b1..Filter Enabled. CS# inputs to the filter are enabled and the filter state machine controls the CS disable
 *       outputs. This bit takes priority over the P1_BYP_EN bit in the Test Register. Setting this bit enables
 *       filtering and enables communication to flash devices regardless of the state of P1_BYP_EN. All other
 *       register controls are functional while filtering is enabled. The filter is enabled by default at CPLD power on.
 */
#define SPI_FILTER_CR_P1_FLT_EN(x)               (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P1_FLT_EN_SHIFT)) & SPI_FILTER_CR_P1_FLT_EN_MASK)

#define SPI_FILTER_CR_P1_BYTE_MD_RST_MASK        (0x800000U)
#define SPI_FILTER_CR_P1_BYTE_MD_RST_SHIFT       (23U)
/*! P1_BYTE_MD_RST
 *  0b0..3-byte mode after device reset.
 *  0b1..4-byte mode after device reset.
 */
#define SPI_FILTER_CR_P1_BYTE_MD_RST(x)          (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P1_BYTE_MD_RST_SHIFT)) & SPI_FILTER_CR_P1_BYTE_MD_RST_MASK)

#define SPI_FILTER_CR_P0_ACT_SEL_MASK            (0x1000000U)
#define SPI_FILTER_CR_P0_ACT_SEL_SHIFT           (24U)
/*! P0_ACT_SEL - Active Chip Select for Flash memory
 *  0b0..Sets CS0 as the read location for the firmware image and sets CS1 as the write location for the FW image.
 *  0b1..Sets CS1 as the read location for the firmware image and sets CS0 as the write location for the FW image.
 */
#define SPI_FILTER_CR_P0_ACT_SEL(x)              (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P0_ACT_SEL_SHIFT)) & SPI_FILTER_CR_P0_ACT_SEL_MASK)

#define SPI_FILTER_CR_P0_WE_BYTE_MD_MASK         (0x2000000U)
#define SPI_FILTER_CR_P0_WE_BYTE_MD_SHIFT        (25U)
/*! P0_WE_BYTE_MD - Write enable switch for address byte mode
 *  0b0..Write Enable command is not required to switch address byte mode
 *  0b1..Write Enable command is required prior to switching address byte mode
 */
#define SPI_FILTER_CR_P0_WE_BYTE_MD(x)           (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P0_WE_BYTE_MD_SHIFT)) & SPI_FILTER_CR_P0_WE_BYTE_MD_MASK)

#define SPI_FILTER_CR_P0_PERM_BYTE_MD_MASK       (0x4000000U)
#define SPI_FILTER_CR_P0_PERM_BYTE_MD_SHIFT      (26U)
/*! P0_PERM_BYTE_MD - Address Byte change mode
 *  0b0..Normal Operation. Address byte mode can be changed as usual.
 *  0b1..Address byte mode cannot be changed.
 */
#define SPI_FILTER_CR_P0_PERM_BYTE_MD(x)         (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P0_PERM_BYTE_MD_SHIFT)) & SPI_FILTER_CR_P0_PERM_BYTE_MD_MASK)

#define SPI_FILTER_CR_P0_BYTE_SEL_MASK           (0x8000000U)
#define SPI_FILTER_CR_P0_BYTE_SEL_SHIFT          (27U)
/*! P0_BYTE_SEL - Address Byte Select Mode for P0
 *  0b0..Sets the filter to 3-byte address mode
 *  0b1..Sets the filter to 4-byte address mode
 */
#define SPI_FILTER_CR_P0_BYTE_SEL(x)             (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P0_BYTE_SEL_SHIFT)) & SPI_FILTER_CR_P0_BYTE_SEL_MASK)

#define SPI_FILTER_CR_P0_BYTE_SEL_MD_MASK        (0x10000000U)
#define SPI_FILTER_CR_P0_BYTE_SEL_MD_SHIFT       (28U)
/*! P0_BYTE_SEL_MD - Address Byte Select Mode
 *  0b0..Normal Operation. The Address byte mode is determined by the Op Code Command
 *  0b1..Sets the Byte Mode to 3-byte and gives control of the function to the P0_BYTE_SEL in the P0 Filter
 *       Control Register. In this mode, any changes to P0_BYTE_SEL are latched and the internal state of BYTE_SEL will
 *       retain this value when P0_BYTE_SEL_MD is cleared
 */
#define SPI_FILTER_CR_P0_BYTE_SEL_MD(x)          (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P0_BYTE_SEL_MD_SHIFT)) & SPI_FILTER_CR_P0_BYTE_SEL_MD_MASK)

#define SPI_FILTER_CR_P0_DIRTY_CLR_MASK          (0x20000000U)
#define SPI_FILTER_CR_P0_DIRTY_CLR_SHIFT         (29U)
/*! P0_DIRTY_CLR - Clear dirty state for P0 */
#define SPI_FILTER_CR_P0_DIRTY_CLR(x)            (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P0_DIRTY_CLR_SHIFT)) & SPI_FILTER_CR_P0_DIRTY_CLR_MASK)

#define SPI_FILTER_CR_P0_FLT_EN_MASK             (0x40000000U)
#define SPI_FILTER_CR_P0_FLT_EN_SHIFT            (30U)
/*! P0_FLT_EN - Filter Enable bit
 *  0b0..Filter Disabled. All CS Disable outputs are asserted so that host access to the flash devices is
 *       disabled. CS# inputs to the filter are disabled so that the filter state machine is locked in its current state.
 *       All other register controls can be accessed, but have no impact on the functionality of the filter while
 *       the filter is disabled.
 *  0b1..Filter Enabled. CS# inputs to the filter are enabled and the filter state machine controls the CS disable
 *       outputs. CS# inputs to the filter are enabled so that the filter state machine is active. All other
 *       register controls are functional. The filter is enabled by default at CPLD power on.
 */
#define SPI_FILTER_CR_P0_FLT_EN(x)               (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P0_FLT_EN_SHIFT)) & SPI_FILTER_CR_P0_FLT_EN_MASK)

#define SPI_FILTER_CR_P0_BYTE_MD_RST_MASK        (0x80000000U)
#define SPI_FILTER_CR_P0_BYTE_MD_RST_SHIFT       (31U)
/*! P0_BYTE_MD_RST
 *  0b0..3-byte mode after device reset.
 *  0b1..4-byte mode after device reset.
 */
#define SPI_FILTER_CR_P0_BYTE_MD_RST(x)          (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_CR_P0_BYTE_MD_RST_SHIFT)) & SPI_FILTER_CR_P0_BYTE_MD_RST_MASK)
/*! @} */

/*! @name ISR - Interrupt Register */
/*! @{ */

#define SPI_FILTER_ISR_P0_F8_INT_MASK            (0x1000000U)
#define SPI_FILTER_ISR_P0_F8_INT_SHIFT           (24U)
/*! P0_F8_INT
 *  0b0..Indicates normal operation
 *  0b1..Indicates an F8 command has been detected
 */
#define SPI_FILTER_ISR_P0_F8_INT(x)              (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_ISR_P0_F8_INT_SHIFT)) & SPI_FILTER_ISR_P0_F8_INT_MASK)

#define SPI_FILTER_ISR_P1_F8_INT_MASK            (0x2000000U)
#define SPI_FILTER_ISR_P1_F8_INT_SHIFT           (25U)
/*! P1_F8_INT
 *  0b0..Indicates normal operation
 *  0b1..Indicates an F8 command has been detected
 */
#define SPI_FILTER_ISR_P1_F8_INT(x)              (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_ISR_P1_F8_INT_SHIFT)) & SPI_FILTER_ISR_P1_F8_INT_MASK)

#define SPI_FILTER_ISR_P0_DIRTY_INT_MASK         (0x4000000U)
#define SPI_FILTER_ISR_P0_DIRTY_INT_SHIFT        (26U)
/*! P0_DIRTY_INT
 *  0b0..Indicates normal operation
 *  0b1..Indicates that a write has occurred to the FW Code region of the inactive Flash and that flash is now considered corrupt.
 */
#define SPI_FILTER_ISR_P0_DIRTY_INT(x)           (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_ISR_P0_DIRTY_INT_SHIFT)) & SPI_FILTER_ISR_P0_DIRTY_INT_MASK)

#define SPI_FILTER_ISR_P1_DIRTY_INT_MASK         (0x8000000U)
#define SPI_FILTER_ISR_P1_DIRTY_INT_SHIFT        (27U)
/*! P1_DIRTY_INT
 *  0b0..Indicates normal operation
 *  0b1..indicates that a write has occurred to the FW Code region of the inactive Flash and that flash is now considered corrupt
 */
#define SPI_FILTER_ISR_P1_DIRTY_INT(x)           (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_ISR_P1_DIRTY_INT_SHIFT)) & SPI_FILTER_ISR_P1_DIRTY_INT_MASK)

#define SPI_FILTER_ISR_P0_BLK_INT_MASK           (0x10000000U)
#define SPI_FILTER_ISR_P0_BLK_INT_SHIFT          (28U)
/*! P0_BLK_INT
 *  0b0..Indicates normal operation
 *  0b1..Indicates that a blocked opcode has been detected
 */
#define SPI_FILTER_ISR_P0_BLK_INT(x)             (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_ISR_P0_BLK_INT_SHIFT)) & SPI_FILTER_ISR_P0_BLK_INT_MASK)

#define SPI_FILTER_ISR_P1_BLK_INT_MASK           (0x20000000U)
#define SPI_FILTER_ISR_P1_BLK_INT_SHIFT          (29U)
/*! P1_BLK_INT
 *  0b0..Indicates normal operation
 *  0b1..Indicates that a blocked opcode has been detected
 */
#define SPI_FILTER_ISR_P1_BLK_INT(x)             (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_ISR_P1_BLK_INT_SHIFT)) & SPI_FILTER_ISR_P1_BLK_INT_MASK)

#define SPI_FILTER_ISR_P0_BYTEMODE_INT_MASK      (0x40000000U)
#define SPI_FILTER_ISR_P0_BYTEMODE_INT_SHIFT     (30U)
/*! P0_BYTEMODE_INT
 *  0b0..Indicates normal operation
 *  0b1..Indicates that a Bytemode change has been detected (OpCode E9 or B7)
 */
#define SPI_FILTER_ISR_P0_BYTEMODE_INT(x)        (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_ISR_P0_BYTEMODE_INT_SHIFT)) & SPI_FILTER_ISR_P0_BYTEMODE_INT_MASK)

#define SPI_FILTER_ISR_P1_BYTEMODE_INT_MASK      (0x80000000U)
#define SPI_FILTER_ISR_P1_BYTEMODE_INT_SHIFT     (31U)
/*! P1_BYTEMODE_INT
 *  0b0..Indicates normal operation
 *  0b1..Indicates that a Bytemode change has been detected (OpCode E9 or B7)
 */
#define SPI_FILTER_ISR_P1_BYTEMODE_INT(x)        (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_ISR_P1_BYTEMODE_INT_SHIFT)) & SPI_FILTER_ISR_P1_BYTEMODE_INT_MASK)
/*! @} */

/*! @name IMR - Interrupt Mask Register */
/*! @{ */

#define SPI_FILTER_IMR_P0_F8_MASK_MASK           (0x1000000U)
#define SPI_FILTER_IMR_P0_F8_MASK_SHIFT          (24U)
/*! P0_F8_MASK
 *  0b0..Indicates P0_F8_INT interrupt is enabled.
 *  0b1..Indicates P0_F8_INT interrupt is disabled.
 */
#define SPI_FILTER_IMR_P0_F8_MASK(x)             (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_IMR_P0_F8_MASK_SHIFT)) & SPI_FILTER_IMR_P0_F8_MASK_MASK)

#define SPI_FILTER_IMR_P1_F8_MASK_MASK           (0x2000000U)
#define SPI_FILTER_IMR_P1_F8_MASK_SHIFT          (25U)
/*! P1_F8_MASK
 *  0b0..Indicates P1_F8_INT interrupt is enabled.
 *  0b1..Indicates P1_F8_INT interrupt is disabled.
 */
#define SPI_FILTER_IMR_P1_F8_MASK(x)             (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_IMR_P1_F8_MASK_SHIFT)) & SPI_FILTER_IMR_P1_F8_MASK_MASK)

#define SPI_FILTER_IMR_P0_DIRTY_MASK_MASK        (0x4000000U)
#define SPI_FILTER_IMR_P0_DIRTY_MASK_SHIFT       (26U)
/*! P0_DIRTY_MASK
 *  0b0..Indicates P0_DIRTY_INT interrupt is enabled.
 *  0b1..Indicates P0_DIRTY_INT interrupt is disabled.
 */
#define SPI_FILTER_IMR_P0_DIRTY_MASK(x)          (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_IMR_P0_DIRTY_MASK_SHIFT)) & SPI_FILTER_IMR_P0_DIRTY_MASK_MASK)

#define SPI_FILTER_IMR_P1_DIRTY_MASK_MASK        (0x8000000U)
#define SPI_FILTER_IMR_P1_DIRTY_MASK_SHIFT       (27U)
/*! P1_DIRTY_MASK
 *  0b0..Indicates P1_DIRTY_INT interrupt is enabled.
 *  0b1..Indicates P1_DIRTY_INT interrupt is disabled.
 */
#define SPI_FILTER_IMR_P1_DIRTY_MASK(x)          (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_IMR_P1_DIRTY_MASK_SHIFT)) & SPI_FILTER_IMR_P1_DIRTY_MASK_MASK)

#define SPI_FILTER_IMR_P0_BLK_MASK_MASK          (0x10000000U)
#define SPI_FILTER_IMR_P0_BLK_MASK_SHIFT         (28U)
/*! P0_BLK_MASK
 *  0b0..Indicates P0_BLK_INT is enabled.
 *  0b1..Indicates P0_BLK_INT is disabled.
 */
#define SPI_FILTER_IMR_P0_BLK_MASK(x)            (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_IMR_P0_BLK_MASK_SHIFT)) & SPI_FILTER_IMR_P0_BLK_MASK_MASK)

#define SPI_FILTER_IMR_P1_BLK_MASK_MASK          (0x20000000U)
#define SPI_FILTER_IMR_P1_BLK_MASK_SHIFT         (29U)
/*! P1_BLK_MASK
 *  0b0..Indicates P1_BLK_INT is enabled.
 *  0b1..Indicates P1_BLK_INT is disabled.
 */
#define SPI_FILTER_IMR_P1_BLK_MASK(x)            (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_IMR_P1_BLK_MASK_SHIFT)) & SPI_FILTER_IMR_P1_BLK_MASK_MASK)

#define SPI_FILTER_IMR_P0_BYTEMODE_MASK_MASK     (0x40000000U)
#define SPI_FILTER_IMR_P0_BYTEMODE_MASK_SHIFT    (30U)
/*! P0_BYTEMODE_MASK
 *  0b0..Indicates P0_BYTEMODE_INT is enabled.
 *  0b1..Indicates P0_BYTEMODE_INT is disabled.
 */
#define SPI_FILTER_IMR_P0_BYTEMODE_MASK(x)       (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_IMR_P0_BYTEMODE_MASK_SHIFT)) & SPI_FILTER_IMR_P0_BYTEMODE_MASK_MASK)

#define SPI_FILTER_IMR_P1_BYTEMODE_MASK_MASK     (0x80000000U)
#define SPI_FILTER_IMR_P1_BYTEMODE_MASK_SHIFT    (31U)
/*! P1_BYTEMODE_MASK
 *  0b0..Indicates P1_BYTEMODE_INT is enabled.
 *  0b1..Indicates P1_BYTEMODE_INT is disabled.
 */
#define SPI_FILTER_IMR_P1_BYTEMODE_MASK(x)       (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_IMR_P1_BYTEMODE_MASK_SHIFT)) & SPI_FILTER_IMR_P1_BYTEMODE_MASK_MASK)
/*! @} */

/*! @name SR - Port Status Register */
/*! @{ */

#define SPI_FILTER_SR_P1_DIRTY_MASK              (0x10000U)
#define SPI_FILTER_SR_P1_DIRTY_SHIFT             (16U)
/*! P1_DIRTY
 *  0b0..Indicates normal operation. No Writes have been directed to the inactive Flash
 *  0b1..Indicates that a write to the inactive Flash has been detected and the state of the flash should be considered "dirty".
 */
#define SPI_FILTER_SR_P1_DIRTY(x)                (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_SR_P1_DIRTY_SHIFT)) & SPI_FILTER_SR_P1_DIRTY_MASK)

#define SPI_FILTER_SR_P1_BYTE_MODE_MASK          (0x20000U)
#define SPI_FILTER_SR_P1_BYTE_MODE_SHIFT         (17U)
/*! P1_BYTE_MODE
 *  0b0..3-Byte Address Mode
 *  0b1..4-Byte Address Mode
 */
#define SPI_FILTER_SR_P1_BYTE_MODE(x)            (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_SR_P1_BYTE_MODE_SHIFT)) & SPI_FILTER_SR_P1_BYTE_MODE_MASK)

#define SPI_FILTER_SR_P1_WRITE_EN_MASK           (0x40000U)
#define SPI_FILTER_SR_P1_WRITE_EN_SHIFT          (18U)
/*! P1_WRITE_EN - Write enable command status
 *  0b0..Write enable command is not detected
 *  0b1..Write enable command is detected
 */
#define SPI_FILTER_SR_P1_WRITE_EN(x)             (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_SR_P1_WRITE_EN_SHIFT)) & SPI_FILTER_SR_P1_WRITE_EN_MASK)

#define SPI_FILTER_SR_P0_DIRTY_MASK              (0x1000000U)
#define SPI_FILTER_SR_P0_DIRTY_SHIFT             (24U)
/*! P0_DIRTY
 *  0b0..Indicates normal operation. No Writes have been directed to the inactive Flash
 *  0b1..Indicates that a write to the inactive Flash has been detected and the state of the flash should be considered "dirty".
 */
#define SPI_FILTER_SR_P0_DIRTY(x)                (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_SR_P0_DIRTY_SHIFT)) & SPI_FILTER_SR_P0_DIRTY_MASK)

#define SPI_FILTER_SR_P0_BYTE_MODE_MASK          (0x2000000U)
#define SPI_FILTER_SR_P0_BYTE_MODE_SHIFT         (25U)
/*! P0_BYTE_MODE
 *  0b0..3-Byte Address Mode
 *  0b1..4-Byte Address Mode
 */
#define SPI_FILTER_SR_P0_BYTE_MODE(x)            (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_SR_P0_BYTE_MODE_SHIFT)) & SPI_FILTER_SR_P0_BYTE_MODE_MASK)

#define SPI_FILTER_SR_P0_WRITE_EN_MASK           (0x4000000U)
#define SPI_FILTER_SR_P0_WRITE_EN_SHIFT          (26U)
/*! P0_WRITE_EN - Write enable command status
 *  0b0..Write enable command is not detected
 *  0b1..Write enable command is detected
 */
#define SPI_FILTER_SR_P0_WRITE_EN(x)             (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_SR_P0_WRITE_EN_SHIFT)) & SPI_FILTER_SR_P0_WRITE_EN_MASK)
/*! @} */

/*! @name TR - Test Register */
/*! @{ */

#define SPI_FILTER_TR_P0_BYP_SEL_MASK            (0x10000U)
#define SPI_FILTER_TR_P0_BYP_SEL_SHIFT           (16U)
/*! P0_BYP_SEL
 *  0b0..CS0
 *  0b1..CS1
 */
#define SPI_FILTER_TR_P0_BYP_SEL(x)              (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_TR_P0_BYP_SEL_SHIFT)) & SPI_FILTER_TR_P0_BYP_SEL_MASK)

#define SPI_FILTER_TR_P0_BYP_EN_MASK             (0x20000U)
#define SPI_FILTER_TR_P0_BYP_EN_SHIFT            (17U)
/*! P0_BYP_EN
 *  0b0..Filter Bypass Disabled
 *  0b1..Filter Bypass Enabled
 */
#define SPI_FILTER_TR_P0_BYP_EN(x)               (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_TR_P0_BYP_EN_SHIFT)) & SPI_FILTER_TR_P0_BYP_EN_MASK)

#define SPI_FILTER_TR_P1_BYP_SEL_MASK            (0x40000U)
#define SPI_FILTER_TR_P1_BYP_SEL_SHIFT           (18U)
/*! P1_BYP_SEL
 *  0b0..CS0
 *  0b1..CS1
 */
#define SPI_FILTER_TR_P1_BYP_SEL(x)              (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_TR_P1_BYP_SEL_SHIFT)) & SPI_FILTER_TR_P1_BYP_SEL_MASK)

#define SPI_FILTER_TR_P1_BYP_EN_MASK             (0x80000U)
#define SPI_FILTER_TR_P1_BYP_EN_SHIFT            (19U)
/*! P1_BYP_EN
 *  0b0..Filter Bypass Disabled
 *  0b1..Filter Bypass Enabled
 */
#define SPI_FILTER_TR_P1_BYP_EN(x)               (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_TR_P1_BYP_EN_SHIFT)) & SPI_FILTER_TR_P1_BYP_EN_MASK)

#define SPI_FILTER_TR_P0_FF_BYP_EN_MASK          (0x100000U)
#define SPI_FILTER_TR_P0_FF_BYP_EN_SHIFT         (20U)
/*! P0_FF_BYP_EN
 *  0b0..Bypass Neg FF for CS Disabled
 *  0b1..Bypass Neg FF for CS Enabled
 */
#define SPI_FILTER_TR_P0_FF_BYP_EN(x)            (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_TR_P0_FF_BYP_EN_SHIFT)) & SPI_FILTER_TR_P0_FF_BYP_EN_MASK)

#define SPI_FILTER_TR_P1_FF_BYP_EN_MASK          (0x200000U)
#define SPI_FILTER_TR_P1_FF_BYP_EN_SHIFT         (21U)
/*! P1_FF_BYP_EN
 *  0b0..Bypass Neg FF for CS Disabled
 *  0b1..Bypass Neg FF for CS Enabled
 */
#define SPI_FILTER_TR_P1_FF_BYP_EN(x)            (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_TR_P1_FF_BYP_EN_SHIFT)) & SPI_FILTER_TR_P1_FF_BYP_EN_MASK)

#define SPI_FILTER_TR_P0_MFG_ID_MASK             (0xF000000U)
#define SPI_FILTER_TR_P0_MFG_ID_SHIFT            (24U)
/*! P0_MFG_ID - Sets the Flash memory manufacturer
 *  0b0000..Macronix
 *  0b0001..Windbond
 *  0b0010..Micron
 */
#define SPI_FILTER_TR_P0_MFG_ID(x)               (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_TR_P0_MFG_ID_SHIFT)) & SPI_FILTER_TR_P0_MFG_ID_MASK)

#define SPI_FILTER_TR_P1_MFG_ID_MASK             (0xF0000000U)
#define SPI_FILTER_TR_P1_MFG_ID_SHIFT            (28U)
/*! P1_MFG_ID - Sets the Flash memory manufacturer
 *  0b0000..Macronix
 *  0b0001..Windbond
 *  0b0010..Micron
 */
#define SPI_FILTER_TR_P1_MFG_ID(x)               (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_TR_P1_MFG_ID_SHIFT)) & SPI_FILTER_TR_P1_MFG_ID_MASK)
/*! @} */

/*! @name P0FAR1 - PORT0 Filter Address Region 1 */
/*! @{ */

#define SPI_FILTER_P0FAR1_ADDRESS_LOWER_LSBS_MASK (0xFFU)
#define SPI_FILTER_P0FAR1_ADDRESS_LOWER_LSBS_SHIFT (0U)
/*! ADDRESS_LOWER_LSBS - Address Lower LSBs */
#define SPI_FILTER_P0FAR1_ADDRESS_LOWER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR1_ADDRESS_LOWER_LSBS_SHIFT)) & SPI_FILTER_P0FAR1_ADDRESS_LOWER_LSBS_MASK)

#define SPI_FILTER_P0FAR1_ADDRESS_LOWER_MSBS_MASK (0xFF00U)
#define SPI_FILTER_P0FAR1_ADDRESS_LOWER_MSBS_SHIFT (8U)
/*! ADDRESS_LOWER_MSBS - Address Lower MSBs */
#define SPI_FILTER_P0FAR1_ADDRESS_LOWER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR1_ADDRESS_LOWER_MSBS_SHIFT)) & SPI_FILTER_P0FAR1_ADDRESS_LOWER_MSBS_MASK)

#define SPI_FILTER_P0FAR1_ADDRESS_UPPER_LSBS_MASK (0xFF0000U)
#define SPI_FILTER_P0FAR1_ADDRESS_UPPER_LSBS_SHIFT (16U)
/*! ADDRESS_UPPER_LSBS - Address Upper LSBs */
#define SPI_FILTER_P0FAR1_ADDRESS_UPPER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR1_ADDRESS_UPPER_LSBS_SHIFT)) & SPI_FILTER_P0FAR1_ADDRESS_UPPER_LSBS_MASK)

#define SPI_FILTER_P0FAR1_ADDRESS_UPPER_MSBS_MASK (0xFF000000U)
#define SPI_FILTER_P0FAR1_ADDRESS_UPPER_MSBS_SHIFT (24U)
/*! ADDRESS_UPPER_MSBS - Address Upper MSBs */
#define SPI_FILTER_P0FAR1_ADDRESS_UPPER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR1_ADDRESS_UPPER_MSBS_SHIFT)) & SPI_FILTER_P0FAR1_ADDRESS_UPPER_MSBS_MASK)
/*! @} */

/*! @name P0FAR2 - PORT0 Filter Address Region 2 */
/*! @{ */

#define SPI_FILTER_P0FAR2_ADDRESS_LOWER_LSBS_MASK (0xFFU)
#define SPI_FILTER_P0FAR2_ADDRESS_LOWER_LSBS_SHIFT (0U)
/*! ADDRESS_LOWER_LSBS - Address Lower LSBs */
#define SPI_FILTER_P0FAR2_ADDRESS_LOWER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR2_ADDRESS_LOWER_LSBS_SHIFT)) & SPI_FILTER_P0FAR2_ADDRESS_LOWER_LSBS_MASK)

#define SPI_FILTER_P0FAR2_ADDRESS_LOWER_MSBS_MASK (0xFF00U)
#define SPI_FILTER_P0FAR2_ADDRESS_LOWER_MSBS_SHIFT (8U)
/*! ADDRESS_LOWER_MSBS - Address Lower MSBs */
#define SPI_FILTER_P0FAR2_ADDRESS_LOWER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR2_ADDRESS_LOWER_MSBS_SHIFT)) & SPI_FILTER_P0FAR2_ADDRESS_LOWER_MSBS_MASK)

#define SPI_FILTER_P0FAR2_ADDRESS_UPPER_LSBS_MASK (0xFF0000U)
#define SPI_FILTER_P0FAR2_ADDRESS_UPPER_LSBS_SHIFT (16U)
/*! ADDRESS_UPPER_LSBS - Address Upper LSBs */
#define SPI_FILTER_P0FAR2_ADDRESS_UPPER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR2_ADDRESS_UPPER_LSBS_SHIFT)) & SPI_FILTER_P0FAR2_ADDRESS_UPPER_LSBS_MASK)

#define SPI_FILTER_P0FAR2_ADDRESS_UPPER_MSBS_MASK (0xFF000000U)
#define SPI_FILTER_P0FAR2_ADDRESS_UPPER_MSBS_SHIFT (24U)
/*! ADDRESS_UPPER_MSBS - Address Upper MSBs */
#define SPI_FILTER_P0FAR2_ADDRESS_UPPER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR2_ADDRESS_UPPER_MSBS_SHIFT)) & SPI_FILTER_P0FAR2_ADDRESS_UPPER_MSBS_MASK)
/*! @} */

/*! @name P0FAR3 - PORT0 Filter Address Region 3 */
/*! @{ */

#define SPI_FILTER_P0FAR3_ADDRESS_LOWER_LSBS_MASK (0xFFU)
#define SPI_FILTER_P0FAR3_ADDRESS_LOWER_LSBS_SHIFT (0U)
/*! ADDRESS_LOWER_LSBS - Address Lower LSBs */
#define SPI_FILTER_P0FAR3_ADDRESS_LOWER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR3_ADDRESS_LOWER_LSBS_SHIFT)) & SPI_FILTER_P0FAR3_ADDRESS_LOWER_LSBS_MASK)

#define SPI_FILTER_P0FAR3_ADDRESS_LOWER_MSBS_MASK (0xFF00U)
#define SPI_FILTER_P0FAR3_ADDRESS_LOWER_MSBS_SHIFT (8U)
/*! ADDRESS_LOWER_MSBS - Address Lower MSBs */
#define SPI_FILTER_P0FAR3_ADDRESS_LOWER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR3_ADDRESS_LOWER_MSBS_SHIFT)) & SPI_FILTER_P0FAR3_ADDRESS_LOWER_MSBS_MASK)

#define SPI_FILTER_P0FAR3_ADDRESS_UPPER_LSBS_MASK (0xFF0000U)
#define SPI_FILTER_P0FAR3_ADDRESS_UPPER_LSBS_SHIFT (16U)
/*! ADDRESS_UPPER_LSBS - Address Upper LSBs */
#define SPI_FILTER_P0FAR3_ADDRESS_UPPER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR3_ADDRESS_UPPER_LSBS_SHIFT)) & SPI_FILTER_P0FAR3_ADDRESS_UPPER_LSBS_MASK)

#define SPI_FILTER_P0FAR3_ADDRESS_UPPER_MSBS_MASK (0xFF000000U)
#define SPI_FILTER_P0FAR3_ADDRESS_UPPER_MSBS_SHIFT (24U)
/*! ADDRESS_UPPER_MSBS - Address Upper MSBs */
#define SPI_FILTER_P0FAR3_ADDRESS_UPPER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR3_ADDRESS_UPPER_MSBS_SHIFT)) & SPI_FILTER_P0FAR3_ADDRESS_UPPER_MSBS_MASK)
/*! @} */

/*! @name P0FAR4 - PORT0 Filter Address Region 4 */
/*! @{ */

#define SPI_FILTER_P0FAR4_ADDRESS_LOWER_LSBS_MASK (0xFFU)
#define SPI_FILTER_P0FAR4_ADDRESS_LOWER_LSBS_SHIFT (0U)
/*! ADDRESS_LOWER_LSBS - Address Lower LSBs */
#define SPI_FILTER_P0FAR4_ADDRESS_LOWER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR4_ADDRESS_LOWER_LSBS_SHIFT)) & SPI_FILTER_P0FAR4_ADDRESS_LOWER_LSBS_MASK)

#define SPI_FILTER_P0FAR4_ADDRESS_LOWER_MSBS_MASK (0xFF00U)
#define SPI_FILTER_P0FAR4_ADDRESS_LOWER_MSBS_SHIFT (8U)
/*! ADDRESS_LOWER_MSBS - Address Lower MSBs */
#define SPI_FILTER_P0FAR4_ADDRESS_LOWER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR4_ADDRESS_LOWER_MSBS_SHIFT)) & SPI_FILTER_P0FAR4_ADDRESS_LOWER_MSBS_MASK)

#define SPI_FILTER_P0FAR4_ADDRESS_UPPER_LSBS_MASK (0xFF0000U)
#define SPI_FILTER_P0FAR4_ADDRESS_UPPER_LSBS_SHIFT (16U)
/*! ADDRESS_UPPER_LSBS - Address Upper LSBs */
#define SPI_FILTER_P0FAR4_ADDRESS_UPPER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR4_ADDRESS_UPPER_LSBS_SHIFT)) & SPI_FILTER_P0FAR4_ADDRESS_UPPER_LSBS_MASK)

#define SPI_FILTER_P0FAR4_ADDRESS_UPPER_MSBS_MASK (0xFF000000U)
#define SPI_FILTER_P0FAR4_ADDRESS_UPPER_MSBS_SHIFT (24U)
/*! ADDRESS_UPPER_MSBS - Address Upper MSBs */
#define SPI_FILTER_P0FAR4_ADDRESS_UPPER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR4_ADDRESS_UPPER_MSBS_SHIFT)) & SPI_FILTER_P0FAR4_ADDRESS_UPPER_MSBS_MASK)
/*! @} */

/*! @name P0FAR5 - PORT0 Filter Address Region 5 */
/*! @{ */

#define SPI_FILTER_P0FAR5_ADDRESS_LOWER_LSBS_MASK (0xFFU)
#define SPI_FILTER_P0FAR5_ADDRESS_LOWER_LSBS_SHIFT (0U)
/*! ADDRESS_LOWER_LSBS - Address Lower LSBs */
#define SPI_FILTER_P0FAR5_ADDRESS_LOWER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR5_ADDRESS_LOWER_LSBS_SHIFT)) & SPI_FILTER_P0FAR5_ADDRESS_LOWER_LSBS_MASK)

#define SPI_FILTER_P0FAR5_ADDRESS_LOWER_MSBS_MASK (0xFF00U)
#define SPI_FILTER_P0FAR5_ADDRESS_LOWER_MSBS_SHIFT (8U)
/*! ADDRESS_LOWER_MSBS - Address Lower MSBs */
#define SPI_FILTER_P0FAR5_ADDRESS_LOWER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR5_ADDRESS_LOWER_MSBS_SHIFT)) & SPI_FILTER_P0FAR5_ADDRESS_LOWER_MSBS_MASK)

#define SPI_FILTER_P0FAR5_ADDRESS_UPPER_LSBS_MASK (0xFF0000U)
#define SPI_FILTER_P0FAR5_ADDRESS_UPPER_LSBS_SHIFT (16U)
/*! ADDRESS_UPPER_LSBS - Address Upper LSBs */
#define SPI_FILTER_P0FAR5_ADDRESS_UPPER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR5_ADDRESS_UPPER_LSBS_SHIFT)) & SPI_FILTER_P0FAR5_ADDRESS_UPPER_LSBS_MASK)

#define SPI_FILTER_P0FAR5_ADDRESS_UPPER_MSBS_MASK (0xFF000000U)
#define SPI_FILTER_P0FAR5_ADDRESS_UPPER_MSBS_SHIFT (24U)
/*! ADDRESS_UPPER_MSBS - Address Upper MSBs */
#define SPI_FILTER_P0FAR5_ADDRESS_UPPER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR5_ADDRESS_UPPER_MSBS_SHIFT)) & SPI_FILTER_P0FAR5_ADDRESS_UPPER_MSBS_MASK)
/*! @} */

/*! @name P0FAR6 - PORT0 Filter Address Region 6 */
/*! @{ */

#define SPI_FILTER_P0FAR6_ADDRESS_LOWER_LSBS_MASK (0xFFU)
#define SPI_FILTER_P0FAR6_ADDRESS_LOWER_LSBS_SHIFT (0U)
/*! ADDRESS_LOWER_LSBS - Address Lower LSBs */
#define SPI_FILTER_P0FAR6_ADDRESS_LOWER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR6_ADDRESS_LOWER_LSBS_SHIFT)) & SPI_FILTER_P0FAR6_ADDRESS_LOWER_LSBS_MASK)

#define SPI_FILTER_P0FAR6_ADDRESS_LOWER_MSBS_MASK (0xFF00U)
#define SPI_FILTER_P0FAR6_ADDRESS_LOWER_MSBS_SHIFT (8U)
/*! ADDRESS_LOWER_MSBS - Address Lower MSBs */
#define SPI_FILTER_P0FAR6_ADDRESS_LOWER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR6_ADDRESS_LOWER_MSBS_SHIFT)) & SPI_FILTER_P0FAR6_ADDRESS_LOWER_MSBS_MASK)

#define SPI_FILTER_P0FAR6_ADDRESS_UPPER_LSBS_MASK (0xFF0000U)
#define SPI_FILTER_P0FAR6_ADDRESS_UPPER_LSBS_SHIFT (16U)
/*! ADDRESS_UPPER_LSBS - Address Upper LSBs */
#define SPI_FILTER_P0FAR6_ADDRESS_UPPER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR6_ADDRESS_UPPER_LSBS_SHIFT)) & SPI_FILTER_P0FAR6_ADDRESS_UPPER_LSBS_MASK)

#define SPI_FILTER_P0FAR6_ADDRESS_UPPER_MSBS_MASK (0xFF000000U)
#define SPI_FILTER_P0FAR6_ADDRESS_UPPER_MSBS_SHIFT (24U)
/*! ADDRESS_UPPER_MSBS - Address Upper MSBs */
#define SPI_FILTER_P0FAR6_ADDRESS_UPPER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0FAR6_ADDRESS_UPPER_MSBS_SHIFT)) & SPI_FILTER_P0FAR6_ADDRESS_UPPER_MSBS_MASK)
/*! @} */

/*! @name P1FAR1 - PORT1 Filter Address Region 1 */
/*! @{ */

#define SPI_FILTER_P1FAR1_ADDRESS_LOWER_LSBS_MASK (0xFFU)
#define SPI_FILTER_P1FAR1_ADDRESS_LOWER_LSBS_SHIFT (0U)
/*! ADDRESS_LOWER_LSBS - Address Lower LSBs */
#define SPI_FILTER_P1FAR1_ADDRESS_LOWER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR1_ADDRESS_LOWER_LSBS_SHIFT)) & SPI_FILTER_P1FAR1_ADDRESS_LOWER_LSBS_MASK)

#define SPI_FILTER_P1FAR1_ADDRESS_LOWER_MSBS_MASK (0xFF00U)
#define SPI_FILTER_P1FAR1_ADDRESS_LOWER_MSBS_SHIFT (8U)
/*! ADDRESS_LOWER_MSBS - Address Lower MSBs */
#define SPI_FILTER_P1FAR1_ADDRESS_LOWER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR1_ADDRESS_LOWER_MSBS_SHIFT)) & SPI_FILTER_P1FAR1_ADDRESS_LOWER_MSBS_MASK)

#define SPI_FILTER_P1FAR1_ADDRESS_UPPER_LSBS_MASK (0xFF0000U)
#define SPI_FILTER_P1FAR1_ADDRESS_UPPER_LSBS_SHIFT (16U)
/*! ADDRESS_UPPER_LSBS - Address Upper LSBs */
#define SPI_FILTER_P1FAR1_ADDRESS_UPPER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR1_ADDRESS_UPPER_LSBS_SHIFT)) & SPI_FILTER_P1FAR1_ADDRESS_UPPER_LSBS_MASK)

#define SPI_FILTER_P1FAR1_ADDRESS_UPPER_MSBS_MASK (0xFF000000U)
#define SPI_FILTER_P1FAR1_ADDRESS_UPPER_MSBS_SHIFT (24U)
/*! ADDRESS_UPPER_MSBS - Address Upper MSBs */
#define SPI_FILTER_P1FAR1_ADDRESS_UPPER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR1_ADDRESS_UPPER_MSBS_SHIFT)) & SPI_FILTER_P1FAR1_ADDRESS_UPPER_MSBS_MASK)
/*! @} */

/*! @name P1FAR2 - PORT1 Filter Address Region 2 */
/*! @{ */

#define SPI_FILTER_P1FAR2_ADDRESS_LOWER_LSBS_MASK (0xFFU)
#define SPI_FILTER_P1FAR2_ADDRESS_LOWER_LSBS_SHIFT (0U)
/*! ADDRESS_LOWER_LSBS - Address Lower LSBs */
#define SPI_FILTER_P1FAR2_ADDRESS_LOWER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR2_ADDRESS_LOWER_LSBS_SHIFT)) & SPI_FILTER_P1FAR2_ADDRESS_LOWER_LSBS_MASK)

#define SPI_FILTER_P1FAR2_ADDRESS_LOWER_MSBS_MASK (0xFF00U)
#define SPI_FILTER_P1FAR2_ADDRESS_LOWER_MSBS_SHIFT (8U)
/*! ADDRESS_LOWER_MSBS - Address Lower MSBs */
#define SPI_FILTER_P1FAR2_ADDRESS_LOWER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR2_ADDRESS_LOWER_MSBS_SHIFT)) & SPI_FILTER_P1FAR2_ADDRESS_LOWER_MSBS_MASK)

#define SPI_FILTER_P1FAR2_ADDRESS_UPPER_LSBS_MASK (0xFF0000U)
#define SPI_FILTER_P1FAR2_ADDRESS_UPPER_LSBS_SHIFT (16U)
/*! ADDRESS_UPPER_LSBS - Address Upper LSBs */
#define SPI_FILTER_P1FAR2_ADDRESS_UPPER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR2_ADDRESS_UPPER_LSBS_SHIFT)) & SPI_FILTER_P1FAR2_ADDRESS_UPPER_LSBS_MASK)

#define SPI_FILTER_P1FAR2_ADDRESS_UPPER_MSBS_MASK (0xFF000000U)
#define SPI_FILTER_P1FAR2_ADDRESS_UPPER_MSBS_SHIFT (24U)
/*! ADDRESS_UPPER_MSBS - Address Upper MSBs */
#define SPI_FILTER_P1FAR2_ADDRESS_UPPER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR2_ADDRESS_UPPER_MSBS_SHIFT)) & SPI_FILTER_P1FAR2_ADDRESS_UPPER_MSBS_MASK)
/*! @} */

/*! @name P1FAR3 - PORT1 Filter Address Region 3 */
/*! @{ */

#define SPI_FILTER_P1FAR3_ADDRESS_LOWER_LSBS_MASK (0xFFU)
#define SPI_FILTER_P1FAR3_ADDRESS_LOWER_LSBS_SHIFT (0U)
/*! ADDRESS_LOWER_LSBS - Address Lower LSBs */
#define SPI_FILTER_P1FAR3_ADDRESS_LOWER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR3_ADDRESS_LOWER_LSBS_SHIFT)) & SPI_FILTER_P1FAR3_ADDRESS_LOWER_LSBS_MASK)

#define SPI_FILTER_P1FAR3_ADDRESS_LOWER_MSBS_MASK (0xFF00U)
#define SPI_FILTER_P1FAR3_ADDRESS_LOWER_MSBS_SHIFT (8U)
/*! ADDRESS_LOWER_MSBS - Address Lower MSBs */
#define SPI_FILTER_P1FAR3_ADDRESS_LOWER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR3_ADDRESS_LOWER_MSBS_SHIFT)) & SPI_FILTER_P1FAR3_ADDRESS_LOWER_MSBS_MASK)

#define SPI_FILTER_P1FAR3_ADDRESS_UPPER_LSBS_MASK (0xFF0000U)
#define SPI_FILTER_P1FAR3_ADDRESS_UPPER_LSBS_SHIFT (16U)
/*! ADDRESS_UPPER_LSBS - Address Upper LSBs */
#define SPI_FILTER_P1FAR3_ADDRESS_UPPER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR3_ADDRESS_UPPER_LSBS_SHIFT)) & SPI_FILTER_P1FAR3_ADDRESS_UPPER_LSBS_MASK)

#define SPI_FILTER_P1FAR3_ADDRESS_UPPER_MSBS_MASK (0xFF000000U)
#define SPI_FILTER_P1FAR3_ADDRESS_UPPER_MSBS_SHIFT (24U)
/*! ADDRESS_UPPER_MSBS - Address Upper MSBs */
#define SPI_FILTER_P1FAR3_ADDRESS_UPPER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR3_ADDRESS_UPPER_MSBS_SHIFT)) & SPI_FILTER_P1FAR3_ADDRESS_UPPER_MSBS_MASK)
/*! @} */

/*! @name P1FAR4 - PORT1 Filter Address Region 4 */
/*! @{ */

#define SPI_FILTER_P1FAR4_ADDRESS_LOWER_LSBS_MASK (0xFFU)
#define SPI_FILTER_P1FAR4_ADDRESS_LOWER_LSBS_SHIFT (0U)
/*! ADDRESS_LOWER_LSBS - Address Lower LSBs */
#define SPI_FILTER_P1FAR4_ADDRESS_LOWER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR4_ADDRESS_LOWER_LSBS_SHIFT)) & SPI_FILTER_P1FAR4_ADDRESS_LOWER_LSBS_MASK)

#define SPI_FILTER_P1FAR4_ADDRESS_LOWER_MSBS_MASK (0xFF00U)
#define SPI_FILTER_P1FAR4_ADDRESS_LOWER_MSBS_SHIFT (8U)
/*! ADDRESS_LOWER_MSBS - Address Lower MSBs */
#define SPI_FILTER_P1FAR4_ADDRESS_LOWER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR4_ADDRESS_LOWER_MSBS_SHIFT)) & SPI_FILTER_P1FAR4_ADDRESS_LOWER_MSBS_MASK)

#define SPI_FILTER_P1FAR4_ADDRESS_UPPER_LSBS_MASK (0xFF0000U)
#define SPI_FILTER_P1FAR4_ADDRESS_UPPER_LSBS_SHIFT (16U)
/*! ADDRESS_UPPER_LSBS - Address Upper LSBs */
#define SPI_FILTER_P1FAR4_ADDRESS_UPPER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR4_ADDRESS_UPPER_LSBS_SHIFT)) & SPI_FILTER_P1FAR4_ADDRESS_UPPER_LSBS_MASK)

#define SPI_FILTER_P1FAR4_ADDRESS_UPPER_MSBS_MASK (0xFF000000U)
#define SPI_FILTER_P1FAR4_ADDRESS_UPPER_MSBS_SHIFT (24U)
/*! ADDRESS_UPPER_MSBS - Address Upper MSBs */
#define SPI_FILTER_P1FAR4_ADDRESS_UPPER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR4_ADDRESS_UPPER_MSBS_SHIFT)) & SPI_FILTER_P1FAR4_ADDRESS_UPPER_MSBS_MASK)
/*! @} */

/*! @name P1FAR5 - PORT1 Filter Address Region 5 */
/*! @{ */

#define SPI_FILTER_P1FAR5_ADDRESS_LOWER_LSBS_MASK (0xFFU)
#define SPI_FILTER_P1FAR5_ADDRESS_LOWER_LSBS_SHIFT (0U)
/*! ADDRESS_LOWER_LSBS - Address Lower LSBs */
#define SPI_FILTER_P1FAR5_ADDRESS_LOWER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR5_ADDRESS_LOWER_LSBS_SHIFT)) & SPI_FILTER_P1FAR5_ADDRESS_LOWER_LSBS_MASK)

#define SPI_FILTER_P1FAR5_ADDRESS_LOWER_MSBS_MASK (0xFF00U)
#define SPI_FILTER_P1FAR5_ADDRESS_LOWER_MSBS_SHIFT (8U)
/*! ADDRESS_LOWER_MSBS - Address Lower MSBs */
#define SPI_FILTER_P1FAR5_ADDRESS_LOWER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR5_ADDRESS_LOWER_MSBS_SHIFT)) & SPI_FILTER_P1FAR5_ADDRESS_LOWER_MSBS_MASK)

#define SPI_FILTER_P1FAR5_ADDRESS_UPPER_LSBS_MASK (0xFF0000U)
#define SPI_FILTER_P1FAR5_ADDRESS_UPPER_LSBS_SHIFT (16U)
/*! ADDRESS_UPPER_LSBS - Address Upper LSBs */
#define SPI_FILTER_P1FAR5_ADDRESS_UPPER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR5_ADDRESS_UPPER_LSBS_SHIFT)) & SPI_FILTER_P1FAR5_ADDRESS_UPPER_LSBS_MASK)

#define SPI_FILTER_P1FAR5_ADDRESS_UPPER_MSBS_MASK (0xFF000000U)
#define SPI_FILTER_P1FAR5_ADDRESS_UPPER_MSBS_SHIFT (24U)
/*! ADDRESS_UPPER_MSBS - Address Upper MSBs */
#define SPI_FILTER_P1FAR5_ADDRESS_UPPER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR5_ADDRESS_UPPER_MSBS_SHIFT)) & SPI_FILTER_P1FAR5_ADDRESS_UPPER_MSBS_MASK)
/*! @} */

/*! @name P1FAR6 - PORT1 Filter Address Region 6 */
/*! @{ */

#define SPI_FILTER_P1FAR6_ADDRESS_LOWER_LSBS_MASK (0xFFU)
#define SPI_FILTER_P1FAR6_ADDRESS_LOWER_LSBS_SHIFT (0U)
/*! ADDRESS_LOWER_LSBS - Address Lower LSBs */
#define SPI_FILTER_P1FAR6_ADDRESS_LOWER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR6_ADDRESS_LOWER_LSBS_SHIFT)) & SPI_FILTER_P1FAR6_ADDRESS_LOWER_LSBS_MASK)

#define SPI_FILTER_P1FAR6_ADDRESS_LOWER_MSBS_MASK (0xFF00U)
#define SPI_FILTER_P1FAR6_ADDRESS_LOWER_MSBS_SHIFT (8U)
/*! ADDRESS_LOWER_MSBS - Address Lower MSBs */
#define SPI_FILTER_P1FAR6_ADDRESS_LOWER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR6_ADDRESS_LOWER_MSBS_SHIFT)) & SPI_FILTER_P1FAR6_ADDRESS_LOWER_MSBS_MASK)

#define SPI_FILTER_P1FAR6_ADDRESS_UPPER_LSBS_MASK (0xFF0000U)
#define SPI_FILTER_P1FAR6_ADDRESS_UPPER_LSBS_SHIFT (16U)
/*! ADDRESS_UPPER_LSBS - Address Upper LSBs */
#define SPI_FILTER_P1FAR6_ADDRESS_UPPER_LSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR6_ADDRESS_UPPER_LSBS_SHIFT)) & SPI_FILTER_P1FAR6_ADDRESS_UPPER_LSBS_MASK)

#define SPI_FILTER_P1FAR6_ADDRESS_UPPER_MSBS_MASK (0xFF000000U)
#define SPI_FILTER_P1FAR6_ADDRESS_UPPER_MSBS_SHIFT (24U)
/*! ADDRESS_UPPER_MSBS - Address Upper MSBs */
#define SPI_FILTER_P1FAR6_ADDRESS_UPPER_MSBS(x)  (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1FAR6_ADDRESS_UPPER_MSBS_SHIFT)) & SPI_FILTER_P1FAR6_ADDRESS_UPPER_MSBS_MASK)
/*! @} */

/*! @name POPCODE0 - Programmable OP Code0 */
/*! @{ */

#define SPI_FILTER_POPCODE0_FILTER_STATE0_MASK   (0x1F0000U)
#define SPI_FILTER_POPCODE0_FILTER_STATE0_SHIFT  (16U)
/*! FILTER_STATE0 - Programmable Filter state 0 */
#define SPI_FILTER_POPCODE0_FILTER_STATE0(x)     (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_POPCODE0_FILTER_STATE0_SHIFT)) & SPI_FILTER_POPCODE0_FILTER_STATE0_MASK)

#define SPI_FILTER_POPCODE0_FILTER_BIT0_MASK     (0x200000U)
#define SPI_FILTER_POPCODE0_FILTER_BIT0_SHIFT    (21U)
/*! FILTER_BIT0
 *  0b0..Filter after 7th bit for all Commands Op Codes that need to be filtered.
 *  0b1..Filter after 8th bit for all Read/Write Op Codes that are filtered based on the address.
 */
#define SPI_FILTER_POPCODE0_FILTER_BIT0(x)       (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_POPCODE0_FILTER_BIT0_SHIFT)) & SPI_FILTER_POPCODE0_FILTER_BIT0_MASK)

#define SPI_FILTER_POPCODE0_PRG_OPCODE0_MASK     (0xFF000000U)
#define SPI_FILTER_POPCODE0_PRG_OPCODE0_SHIFT    (24U)
/*! PRG_OPCODE0 - Programmable OP Code 0 */
#define SPI_FILTER_POPCODE0_PRG_OPCODE0(x)       (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_POPCODE0_PRG_OPCODE0_SHIFT)) & SPI_FILTER_POPCODE0_PRG_OPCODE0_MASK)
/*! @} */

/*! @name POPCODE1 - Programmable OP Code1 */
/*! @{ */

#define SPI_FILTER_POPCODE1_FILTER_STATE1_MASK   (0x1F0000U)
#define SPI_FILTER_POPCODE1_FILTER_STATE1_SHIFT  (16U)
/*! FILTER_STATE1 - Programmable Filter state 1 */
#define SPI_FILTER_POPCODE1_FILTER_STATE1(x)     (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_POPCODE1_FILTER_STATE1_SHIFT)) & SPI_FILTER_POPCODE1_FILTER_STATE1_MASK)

#define SPI_FILTER_POPCODE1_FILTER_BIT1_MASK     (0x200000U)
#define SPI_FILTER_POPCODE1_FILTER_BIT1_SHIFT    (21U)
/*! FILTER_BIT1
 *  0b0..Filter after 7th bit for all Commands Op Codes that need to be filtered.
 *  0b1..Filter after 8th bit for all Read/Write Op Codes that are filtered based on the address.
 */
#define SPI_FILTER_POPCODE1_FILTER_BIT1(x)       (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_POPCODE1_FILTER_BIT1_SHIFT)) & SPI_FILTER_POPCODE1_FILTER_BIT1_MASK)

#define SPI_FILTER_POPCODE1_PRG_OPCODE1_MASK     (0xFF000000U)
#define SPI_FILTER_POPCODE1_PRG_OPCODE1_SHIFT    (24U)
/*! PRG_OPCODE1 - Programmable OP Code 1 */
#define SPI_FILTER_POPCODE1_PRG_OPCODE1(x)       (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_POPCODE1_PRG_OPCODE1_SHIFT)) & SPI_FILTER_POPCODE1_PRG_OPCODE1_MASK)
/*! @} */

/*! @name POPCODE2 - Programmable OP Code2 */
/*! @{ */

#define SPI_FILTER_POPCODE2_FILTER_STATE2_MASK   (0x1F0000U)
#define SPI_FILTER_POPCODE2_FILTER_STATE2_SHIFT  (16U)
/*! FILTER_STATE2 - Programmable Filter state 2 */
#define SPI_FILTER_POPCODE2_FILTER_STATE2(x)     (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_POPCODE2_FILTER_STATE2_SHIFT)) & SPI_FILTER_POPCODE2_FILTER_STATE2_MASK)

#define SPI_FILTER_POPCODE2_FILTER_BIT2_MASK     (0x200000U)
#define SPI_FILTER_POPCODE2_FILTER_BIT2_SHIFT    (21U)
/*! FILTER_BIT2
 *  0b0..Filter after 7th bit for all Commands Op Codes that need to be filtered.
 *  0b1..Filter after 8th bit for all Read/Write Op Codes that are filtered based on the address.
 */
#define SPI_FILTER_POPCODE2_FILTER_BIT2(x)       (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_POPCODE2_FILTER_BIT2_SHIFT)) & SPI_FILTER_POPCODE2_FILTER_BIT2_MASK)

#define SPI_FILTER_POPCODE2_PRG_OPCODE2_MASK     (0xFF000000U)
#define SPI_FILTER_POPCODE2_PRG_OPCODE2_SHIFT    (24U)
/*! PRG_OPCODE2 - Programmable OP Code 0 */
#define SPI_FILTER_POPCODE2_PRG_OPCODE2(x)       (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_POPCODE2_PRG_OPCODE2_SHIFT)) & SPI_FILTER_POPCODE2_PRG_OPCODE2_MASK)
/*! @} */

/*! @name POPCODE3 - Programmable OP Code3 */
/*! @{ */

#define SPI_FILTER_POPCODE3_FILTER_STATE3_MASK   (0x1F0000U)
#define SPI_FILTER_POPCODE3_FILTER_STATE3_SHIFT  (16U)
/*! FILTER_STATE3 - Programmable Filter state 3 */
#define SPI_FILTER_POPCODE3_FILTER_STATE3(x)     (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_POPCODE3_FILTER_STATE3_SHIFT)) & SPI_FILTER_POPCODE3_FILTER_STATE3_MASK)

#define SPI_FILTER_POPCODE3_FILTER_BIT3_MASK     (0x200000U)
#define SPI_FILTER_POPCODE3_FILTER_BIT3_SHIFT    (21U)
/*! FILTER_BIT3
 *  0b0..Filter after 7th bit for all Commands Op Codes that need to be filtered.
 *  0b1..Filter after 8th bit for all Read/Write Op Codes that are filtered based on the address.
 */
#define SPI_FILTER_POPCODE3_FILTER_BIT3(x)       (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_POPCODE3_FILTER_BIT3_SHIFT)) & SPI_FILTER_POPCODE3_FILTER_BIT3_MASK)

#define SPI_FILTER_POPCODE3_PRG_OPCODE3_MASK     (0xFF000000U)
#define SPI_FILTER_POPCODE3_PRG_OPCODE3_SHIFT    (24U)
/*! PRG_OPCODE3 - Programmable OP Code 3 */
#define SPI_FILTER_POPCODE3_PRG_OPCODE3(x)       (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_POPCODE3_PRG_OPCODE3_SHIFT)) & SPI_FILTER_POPCODE3_PRG_OPCODE3_MASK)
/*! @} */

/*! @name POPCODE4 - Programmable OP Code4 */
/*! @{ */

#define SPI_FILTER_POPCODE4_FILTER_STATE4_MASK   (0x1F0000U)
#define SPI_FILTER_POPCODE4_FILTER_STATE4_SHIFT  (16U)
/*! FILTER_STATE4 - Programmable Filter state 4 */
#define SPI_FILTER_POPCODE4_FILTER_STATE4(x)     (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_POPCODE4_FILTER_STATE4_SHIFT)) & SPI_FILTER_POPCODE4_FILTER_STATE4_MASK)

#define SPI_FILTER_POPCODE4_FILTER_BIT4_MASK     (0x200000U)
#define SPI_FILTER_POPCODE4_FILTER_BIT4_SHIFT    (21U)
/*! FILTER_BIT4
 *  0b0..Filter after 7th bit for all Commands Op Codes that need to be filtered.
 *  0b1..Filter after 8th bit for all Read/Write Op Codes that are filtered based on the address.
 */
#define SPI_FILTER_POPCODE4_FILTER_BIT4(x)       (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_POPCODE4_FILTER_BIT4_SHIFT)) & SPI_FILTER_POPCODE4_FILTER_BIT4_MASK)

#define SPI_FILTER_POPCODE4_PRG_OPCODE4_MASK     (0xFF000000U)
#define SPI_FILTER_POPCODE4_PRG_OPCODE4_SHIFT    (24U)
/*! PRG_OPCODE4 - Programmable OP Code 4 */
#define SPI_FILTER_POPCODE4_PRG_OPCODE4(x)       (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_POPCODE4_PRG_OPCODE4_SHIFT)) & SPI_FILTER_POPCODE4_PRG_OPCODE4_MASK)
/*! @} */

/*! @name P0BOC - P0 Blocked Op Code */
/*! @{ */

#define SPI_FILTER_P0BOC_P0BOC_MASK              (0xFF000000U)
#define SPI_FILTER_P0BOC_P0BOC_SHIFT             (24U)
/*! P0BOC - PORT0 Blocked Op Code */
#define SPI_FILTER_P0BOC_P0BOC(x)                (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0BOC_P0BOC_SHIFT)) & SPI_FILTER_P0BOC_P0BOC_MASK)
/*! @} */

/*! @name P1BOC - P1 Blocked Op Code */
/*! @{ */

#define SPI_FILTER_P1BOC_P1BOC_MASK              (0xFF000000U)
#define SPI_FILTER_P1BOC_P1BOC_SHIFT             (24U)
/*! P1BOC - PORT1 Blocked Op Code */
#define SPI_FILTER_P1BOC_P1BOC(x)                (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1BOC_P1BOC_SHIFT)) & SPI_FILTER_P1BOC_P1BOC_MASK)
/*! @} */

/*! @name P0MAM - PORT0 Max Address Mask */
/*! @{ */

#define SPI_FILTER_P0MAM_P0MAML_MASK             (0xFF0000U)
#define SPI_FILTER_P0MAM_P0MAML_SHIFT            (16U)
#define SPI_FILTER_P0MAM_P0MAML(x)               (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0MAM_P0MAML_SHIFT)) & SPI_FILTER_P0MAM_P0MAML_MASK)

#define SPI_FILTER_P0MAM_P0MAMM_MASK             (0xFF000000U)
#define SPI_FILTER_P0MAM_P0MAMM_SHIFT            (24U)
#define SPI_FILTER_P0MAM_P0MAMM(x)               (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P0MAM_P0MAMM_SHIFT)) & SPI_FILTER_P0MAM_P0MAMM_MASK)
/*! @} */

/*! @name P1MAM - PORT1 Max Address Mask */
/*! @{ */

#define SPI_FILTER_P1MAM_P1MAML_MASK             (0xFF0000U)
#define SPI_FILTER_P1MAM_P1MAML_SHIFT            (16U)
#define SPI_FILTER_P1MAM_P1MAML(x)               (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1MAM_P1MAML_SHIFT)) & SPI_FILTER_P1MAM_P1MAML_MASK)

#define SPI_FILTER_P1MAM_P1MAMM_MASK             (0xFF000000U)
#define SPI_FILTER_P1MAM_P1MAMM_SHIFT            (24U)
/*! P1MAMM - PORT1 Max Address Mask MSB */
#define SPI_FILTER_P1MAM_P1MAMM(x)               (((uint32_t)(((uint32_t)(x)) << SPI_FILTER_P1MAM_P1MAMM_SHIFT)) & SPI_FILTER_P1MAM_P1MAMM_MASK)
/*! @} */


/*!
 * @}
 */ /* end of group SPI_FILTER_Register_Masks */


/*!
 * @}
 */ /* end of group SPI_FILTER_Peripheral_Access_Layer */


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


#endif  /* PERI_SPI_FILTER_H_ */

