/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**
 * \file sam_reg_map.h
 * \brief Register map of Arm Security Alarm Manager (SAM).
 */

#ifndef __SAM_REG_MAP_H__
#define __SAM_REG_MAP_H__

#include <stdint.h>
#include "tfm_hal_device_header.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * \brief SAM register map.
 */
struct sam_reg_map_t {
    __IM uint32_t sambc;
        /**< Offset: 0x000 (R/ ) SAM Build Configuration register */
    __IM uint32_t sames[2];
        /**< Offset: 0x004 (R/ ) SAM Event Status registers */
    __OM uint32_t samecl[2];
        /**< Offset: 0x00C ( /W) SAM Event Clear registers */
    __IOM uint32_t samem[2];
        /**< Offset: 0x014 (R/W) SAM Export Mask registers */
    __IOM uint32_t samim[2];
        /**< Offset: 0x01C (R/W) SAM Input Mask registers */
    __IOM uint32_t samrrls[8];
        /**< Offset: 0x024 (R/W) SAM Response Routing Logic Setup registers */
    __IOM uint32_t samec[8];
        /**< Offset: 0x044 (R/W) SAM Event Counter registers */
    __IOM uint32_t samectiv;
        /**< Offset: 0x064 (R/W) SAM Event Counters Timer Initial Value register */
    __IOM uint32_t samwdciv;
        /**< Offset: 0x068 (R/W) SAM Watchdog Counter Initial Value register */
    __IOM uint32_t samrl;
        /**< Offset: 0x06C (R/W) SAM Registers Lock register */
    __IOM uint32_t samicv;
        /**< Offset: 0x070 (R/W) SAM Integrity Check Value register */
    __IOM uint32_t samcdres;
        /**< Offset: 0x074 (R/W) SAM Processor DCLS Reported Errors Status registers */
    __IOM uint32_t samrres[3];
        /**< Offset: 0x078 (R/W) SAM Processor RAS Reported Errors Status registers */
    __IOM uint32_t vmpwca[4];
        /**< Offset: 0x084 (R/W) VM<n> Partial Write Captured Address registers */
    __IOM uint32_t vmsceeca[4];
        /**< Offset: 0x094 (R/W) VM<n> Single Corrected ECC Error Captured Address registers */
    __IOM uint32_t vmdueeca[4];
        /**< Offset: 0x0A4 (R/W) VM<n> Double Uncorrected ECC Error Captured Address registers */
    __IOM uint32_t tramsceeca;
        /**< Offset: 0x0B4 (R/W) TRAM Single Corrected ECC Error Captured Address register */
    __IOM uint32_t tramdueeca;
        /**< Offset: 0x0B8 (R/W) TRAM Double Uncorrected ECC Error Captured Address register */
    const volatile uint32_t _reserved0[0x3C5];
        /**< Offset: 0x0BC (RAZ/WI) Reserved */
    __IM uint32_t pidr4;
        /**< Offset: 0xFD0 (R/ ) Peripheral ID 4 register */
    const volatile uint32_t _reserved1[0x3];
        /**< Offset: 0xFD4 (RAZ/WI) Reserved */
    __IM uint32_t pidr[4];
        /**< Offset: 0xFE0 (R/ ) Peripheral ID 0-3 registers */
    __IM uint32_t cidr[4];
        /**< Offset: 0xFF0 (R/ ) Component ID 0-3 registers */
};
#ifdef __cplusplus
}
#endif

#endif /* __SAM_REG_MAP_H__ */
