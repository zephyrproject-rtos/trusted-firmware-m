/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**
 * \file    gpio_pl061_drv.h
 * \brief   Driver for ARM Primecell GPIO Element
 *          As described in DDI0190
 *          https://developer.arm.com/documentation/ddi0190/latest/
 */

#ifndef GPIO_PL061_DRV_H_
#define GPIO_PL061_DRV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "tfm_hal_device_header.h"

typedef __PACKED_STRUCT {
    __IOM   uint32_t gpiodata[256];     /*!< Offset: 0x000 (R/W) Data Register Array */
    __IOM   uint32_t gpiodir;           /*!< Offset: 0x400 (R/W) Direction Register */
    __IOM   uint32_t gpiois;            /*!< Offset: 0x404 (R/W) Interrupt Sense Register */
    __IOM   uint32_t gpioibe;           /*!< Offset: 0x408 (R/W) Interrupt Both Edges Register */
    __IOM   uint32_t gpioiev;           /*!< Offset: 0x40C (R/W) Interrupt Event Register */
    __IOM   uint32_t gpioie;            /*!< Offset: 0x410 (R/W) Interrupt Event Register */
    __IM    uint32_t gpioris;           /*!< Offset: 0x414 (R/O) Raw Interrupt Status Register */
    __IM    uint32_t gpiomis;           /*!< Offset: 0x418 (R/O) Masked Interrupt Status Register */
    __OM    uint32_t gpioic;            /*!< Offset: 0x41C (R/W) Interrupt Clear Register */
    __IOM   uint32_t gpioafsel;         /*!< Offset: 0x420 (R/W) Mode Control Select Register */
    const   uint32_t reserved[751];
    __IM    uint32_t gpioperiphid[4];   /*!< Offset: 0xFE0 (R/O) Peripheral Identification */
    __IM    uint32_t gpiocellid[4];     /*!< Offset: 0xFF0 (R/O) Primecell Identification */
} pl061_regblk_t;

/**
 * \brief           Set all GPIO to given state for every HIGH bit in mask
 *
 * \param[in] pdev  Pointer to the PL061 register block
 * \param[in] mask  Bitmask for writes to be masked
 * \param[in] pins  Bitmask for state of pins
 */
__STATIC_FORCEINLINE
void pl061_set_gpio(pl061_regblk_t * pdev, uint8_t mask, uint8_t pins)
{
    /* DDI0190: 3.3.1: Masked by address; Never write HIGH to INPUT */
    pdev->gpiodata[mask] = pins & pdev->gpiodir;
}

/**
 * \brief           Set GPIO to HIGH state for every HIGH bit in mask
 *
 * \param[in] pdev  Pointer to the PL061 register block
 * \param[in] mask  Bitmask for writes to be masked
 */
__STATIC_FORCEINLINE
void pl061_set_high(pl061_regblk_t * pdev, uint8_t mask)
{
    /* DDI0190: 3.3.1: Masked by address; Never write HIGH to INPUT */
    pdev->gpiodata[mask] = UINT8_MAX & pdev->gpiodir;
}

/**
 * \brief           Set the GPIO to LOW for every HIGH bit in mask
 *
 * \param[in] pdev  Pointer to the PL061 register block
 * \param[in] mask  Bitmask for writes to be masked
 */
__STATIC_FORCEINLINE
void gpio_set_low(pl061_regblk_t * pdev, uint8_t mask)
{
    /* DDI0190: 3.3.1: Masked by address */
    pdev->gpiodata[mask] = 0u;
}

/**
 * \brief           Get the GPIO state for every HIGH bit in mask
 *
 * \param[in] pdev  Pointer to the PL061 register block
 * \param[in] mask  Bitmask for reads to be masked
 * \return uint8_t  Masked state of GPIO
 */
__STATIC_FORCEINLINE
uint8_t pl061_get_gpio(pl061_regblk_t * pdev, uint8_t mask)
{
    /* DDI0190: 3.3.1: Masked by address */
    return pdev->gpiodata[mask] & UINT8_MAX; /* only 8 bit wide */
}

/**
 * \brief           Set the GPIO direction to INPUT for every HIGH bit in mask
 *
 * \param[in] pdev  Pointer to the PL061 register block
 * \param[in] mask  Bitmask for GPIO to be configured as INPUT
 */
__STATIC_FORCEINLINE
void pl061_set_input(pl061_regblk_t * pdev, uint8_t mask)
{
    pdev->gpiodir &= ~mask;
}

/**
 * \brief           Set GPIO direction to OUTPUT for every HIGH bit in mask
 *
 * \param[in] pdev  Pointer to the PL061 register block
 * \param[in] mask  Bitmask for GPIO to be configured as OUTPUT
 */
__STATIC_FORCEINLINE
void pl061_set_output(pl061_regblk_t * pdev, uint8_t mask)
{
    pdev->gpiodir |= mask;
}

/**
 * \brief       Get peripheral ID from hardware
 *
 * \param pdev  Pointer to the PL061 register block
 * \return      uint32_t
 */
uint32_t pl061_get_perifid(pl061_regblk_t * pdev);

/**
 * \brief           Get primecell ID from hardware
 *
 * \param[in] pdev  Pointer to the PL061 register block
 * \return          uint32_t - 0xB105_F00D
 */
uint32_t pl061_get_cellid(pl061_regblk_t * pdev);

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PL061_DRV_H_ */
