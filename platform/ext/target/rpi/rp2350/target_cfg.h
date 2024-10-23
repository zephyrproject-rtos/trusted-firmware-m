/*
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 */

#ifndef __TARGET_CFG_H__
#define __TARGET_CFG_H__

#include <stdint.h>

#define TFM_DRIVER_STDIO    driver_usart0
#define NS_DRIVER_STDIO     driver_usart0

/**
 * \brief Defines the word offsets of Slave Peripheral Protection Controller
 *        Registers
 */
typedef enum
{
    PPC_SP_DO_NOT_CONFIGURE = -1,
} ppc_bank_t;

typedef enum
{
    AC_LOCK = 0x0,
    AC_FORCE_CORE_NS = 0x4,
    AC_CFGRESET = 0x8,
    AC_GPIO_NSMASK0 = 0xC,
    AC_GPIO_NSMASK1 = 0x10,
    AC_ROM = 0x14,
    AC_XIP_MAIN = 0x18,
    AC_SRAM0 = 0x1C,
    AC_SRAM1 = 0x20,
    AC_SRAM2 = 0x24,
    AC_SRAM3 = 0x28,
    AC_SRAM4 = 0x2C,
    AC_SRAM5 = 0x30,
    AC_SRAM6 = 0x34,
    AC_SRAM7 = 0x38,
    AC_SRAM8 = 0x3C,
    AC_SRAM9 = 0x40,
    AC_DMA = 0x44,
    AC_USBCTRL = 0x48,
    AC_PIO0 = 0x4C,
    AC_PIO1 = 0x50,
    AC_PIO2 = 0x54,
    AC_CORESIGHT_TRACE = 0x58,
    AC_CORESIGHT_PERIPH = 0x5C,
    AC_SYSINFO = 0x60,
    AC_RESETS = 0x64,
    AC_IO_BANK0 = 0x68,
    AC_IO_BANK1 = 0x6C,
    AC_PADS_BANK0 = 0x70,
    AC_PADS_QSPI = 0x74,
    AC_BUSCTRL = 0x78,
    AC_ADC0 = 0x7C,
    AC_HSTX = 0x80,
    AC_I2C0 = 0x84,
    AC_I2C1 = 0x88,
    AC_PWM = 0x8C,
    AC_SPI0 = 0x90,
    AC_SPI1 = 0x94,
    AC_TIMER0 = 0x98,
    AC_TIMER1 = 0x9C,
    AC_UART0 = 0xA0,
    AC_UART1 = 0xA4,
    AC_OTP = 0xA8,
    AC_TBMAN = 0xAC,
    AC_POWMAN = 0xB0,
    AC_TRNG = 0xB4,
    AC_SHA256 = 0xB8,
    AC_SYSCFG = 0xBC,
    AC_CLOCKS_BANK_DEFAULT = 0xC0,
    AC_XOSC = 0xC4,
    AC_ROSC = 0xC8,
    AC_PLL_SYS = 0xCC,
    AC_PLL_USB = 0xD0,
    AC_TICKS = 0xD4,
    AC_WATCHDOG = 0xD8,
    AC_RSM = 0xDC,
    AC_XIP_CTRL = 0xE0,
    AC_XIP_QMI = 0xE4,
    AC_XIP_AUX = 0xE8,
    AC_DO_NOT_CONFIGURE = 0xFFFF,
} access_ctrl_reg_offset;

/**
 * \brief Initialize SAU.
 */
void sau_and_idau_cfg(void);

/**
 * \brief Configure access control for bus endpoints.
 */
enum tfm_plat_err_t bus_filter_cfg(void);

/**
 * \brief Configure DMA channels' security
 */
enum tfm_plat_err_t dma_security_config(void);

/**
 * \brief Configure bus endpoint to be secure privileged accessible for core0
 */
void access_ctrl_configure_to_secure_privileged(access_ctrl_reg_offset offset);


/**
 * \brief Configure bus endpoint to be secure unprivileged accessible for core0
 */
void access_ctrl_configure_to_secure_unprivileged(access_ctrl_reg_offset offset);

#endif /* __TARGET_CFG_H__ */
