/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __MMIO_DEFS_H__
#define __MMIO_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "tfm_peripherals_def.h"

/* Boundary handle binding macros. */
#define HANDLE_PER_ATTR_BITS (0x4)
#define HANDLE_ATTR_PRIV_MASK ((1 << HANDLE_PER_ATTR_BITS) - 1)

/* Allowed named MMIO of this platform */
const uintptr_t partition_named_mmio_list[] = {
    (uintptr_t)TFM_PERIPHERAL_DCNF,
    (uintptr_t)TFM_PERIPHERAL_FPU,
    (uintptr_t)TFM_PERIPHERAL_OSCILLATORS,
    (uintptr_t)TFM_PERIPHERAL_REGULATORS,
    (uintptr_t)TFM_PERIPHERAL_CLOCK,
    (uintptr_t)TFM_PERIPHERAL_POWER,
    (uintptr_t)TFM_PERIPHERAL_RESET,
    (uintptr_t)TFM_PERIPHERAL_SPIM0,
    (uintptr_t)TFM_PERIPHERAL_SPIS0,
    (uintptr_t)TFM_PERIPHERAL_TWIM0,
    (uintptr_t)TFM_PERIPHERAL_TWIS0,
    (uintptr_t)TFM_PERIPHERAL_UARTE0,
    (uintptr_t)TFM_PERIPHERAL_SPIM1,
    (uintptr_t)TFM_PERIPHERAL_SPIS1,
    (uintptr_t)TFM_PERIPHERAL_TWIM1,
    (uintptr_t)TFM_PERIPHERAL_TWIS1,
    (uintptr_t)TFM_PERIPHERAL_UARTE1,
    (uintptr_t)TFM_PERIPHERAL_SPIM4,
    (uintptr_t)TFM_PERIPHERAL_SPIM2,
    (uintptr_t)TFM_PERIPHERAL_SPIS2,
    (uintptr_t)TFM_PERIPHERAL_TWIM2,
    (uintptr_t)TFM_PERIPHERAL_TWIS2,
    (uintptr_t)TFM_PERIPHERAL_UARTE2,
    (uintptr_t)TFM_PERIPHERAL_SPIM3,
    (uintptr_t)TFM_PERIPHERAL_SPIS3,
    (uintptr_t)TFM_PERIPHERAL_TWIM3,
    (uintptr_t)TFM_PERIPHERAL_TWIS3,
    (uintptr_t)TFM_PERIPHERAL_UARTE3,
    (uintptr_t)TFM_PERIPHERAL_SAADC,
    (uintptr_t)TFM_PERIPHERAL_TIMER0,
    (uintptr_t)TFM_PERIPHERAL_TIMER1,
    (uintptr_t)TFM_PERIPHERAL_TIMER2,
    (uintptr_t)TFM_PERIPHERAL_RTC0,
    (uintptr_t)TFM_PERIPHERAL_RTC1,
    (uintptr_t)TFM_PERIPHERAL_DPPIC,
    (uintptr_t)TFM_PERIPHERAL_WDT0,
    (uintptr_t)TFM_PERIPHERAL_WDT1,
    (uintptr_t)TFM_PERIPHERAL_COMP,
    (uintptr_t)TFM_PERIPHERAL_LPCOMP,
    (uintptr_t)TFM_PERIPHERAL_EGU0,
    (uintptr_t)TFM_PERIPHERAL_EGU1,
    (uintptr_t)TFM_PERIPHERAL_EGU2,
    (uintptr_t)TFM_PERIPHERAL_EGU3,
    (uintptr_t)TFM_PERIPHERAL_EGU4,
    (uintptr_t)TFM_PERIPHERAL_EGU5,
    (uintptr_t)TFM_PERIPHERAL_PWM0,
    (uintptr_t)TFM_PERIPHERAL_PWM1,
    (uintptr_t)TFM_PERIPHERAL_PWM2,
    (uintptr_t)TFM_PERIPHERAL_PWM3,
    (uintptr_t)TFM_PERIPHERAL_PDM0,
    (uintptr_t)TFM_PERIPHERAL_I2S0,
    (uintptr_t)TFM_PERIPHERAL_IPC,
    (uintptr_t)TFM_PERIPHERAL_QSPI,
    (uintptr_t)TFM_PERIPHERAL_NFCT,
    (uintptr_t)TFM_PERIPHERAL_MUTEX,
    (uintptr_t)TFM_PERIPHERAL_QDEC0,
    (uintptr_t)TFM_PERIPHERAL_QDEC1,
    (uintptr_t)TFM_PERIPHERAL_USBD,
    (uintptr_t)TFM_PERIPHERAL_USBREGULATOR,
    (uintptr_t)TFM_PERIPHERAL_NVMC,
    (uintptr_t)TFM_PERIPHERAL_P0,
    (uintptr_t)TFM_PERIPHERAL_P1,
    (uintptr_t)TFM_PERIPHERAL_VMC,
};

#ifdef __cplusplus
}
#endif

#endif /* __MMIO_DEFS_H__ */
