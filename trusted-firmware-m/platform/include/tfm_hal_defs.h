/*
 * Copyright (c) 2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_HAL_DEFS_H__
#define __TFM_HAL_DEFS_H__

#include <limits.h>
#include <stdint.h>

enum tfm_hal_status_t {
    TFM_HAL_ERROR_MEM_FAULT = SCHAR_MIN,
    TFM_HAL_ERROR_MAX_VALUE,
    TFM_HAL_ERROR_BAD_STATE,
    TFM_HAL_ERROR_NOT_SUPPORTED,
    TFM_HAL_ERROR_INVALID_INPUT,
    TFM_HAL_ERROR_NOT_INIT,
    TFM_HAL_ERROR_GENERIC,
    TFM_HAL_SUCCESS = 0
};

#endif /* __TFM_HAL_DEFS_H__ */
