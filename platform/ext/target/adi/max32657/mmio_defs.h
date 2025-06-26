/*
 * Copyright (c) 2021-2022, Arm Limited. All rights reserved.
 * Copyright (C) 2024 Analog Devices, Inc.
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
#define HANDLE_ATTR_PRIV_POS            1U
#define HANDLE_ATTR_PRIV_MASK           (0x1UL << HANDLE_ATTR_PRIV_POS)
#define HANDLE_ATTR_NS_POS              0U
#define HANDLE_ATTR_NS_MASK             (0x1UL << HANDLE_ATTR_NS_POS)

/* Allowed named MMIO of this platform */
const uintptr_t partition_named_mmio_list[] = {
    (uintptr_t)TFM_PERIPHERAL_STD_UART,
};


#ifdef __cplusplus
}
#endif

#endif /* __MMIO_DEFS_H__ */
