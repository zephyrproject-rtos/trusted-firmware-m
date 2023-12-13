/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSS_BRINGUP_HELPERS_HAL_H__
#define __RSS_BRINGUP_HELPERS_HAL_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t rss_bringup_helpers_hal_get_pins_offset(void);

void *rss_bringup_helpers_hal_get_vm0_exec_address(void);
void *rss_bringup_helpers_hal_get_qpsi_exec_address(void);
void *rss_bringup_helpers_hal_get_side_band_exec_address(void);

#ifdef __cplusplus
}
#endif

#endif /* __RSS_BRINGUP_HELPERS_HAL_H__ */
