/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PLATFORM_ERROR_CODES_H__
#define __PLATFORM_ERROR_CODES_H__

#include <limits.h>

#ifdef TFM_UNIQUE_ERROR_CODES
#include "error_codes_mapping.h"
#else
#define TFM_PLAT_ERROR_BASE 0x1u
#endif /* TFM_UNIQUE_ERROR_CODES */

enum tfm_plat_err_t {
    TFM_PLAT_ERR_SUCCESS = 0,
    /* Generic errors */
    TFM_PLAT_ERR_SYSTEM_ERR = TFM_PLAT_ERROR_BASE,
    TFM_PLAT_ERR_MAX_VALUE,
    TFM_PLAT_ERR_INVALID_INPUT,
    TFM_PLAT_ERR_UNSUPPORTED,
    TFM_PLAT_ERR_NOT_PERMITTED,
    /* Following entry is only to ensure the error code of int size */
    TFM_PLAT_ERR_FORCE_UINT_SIZE = UINT_MAX
};

#endif /* __PLATFORM_ERROR_CODES_H__ */
