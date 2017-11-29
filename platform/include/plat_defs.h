/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PLAT_DEFS_H__
#define __PLAT_DEFS_H__
/**
 * \note The interfaces defined in this file must be implemented for each
 *       target.
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <limits.h>

enum tfm_plat_errno_t {
    TFM_PLAT_ERRNO_SUCCESS = 0,
    TFM_PLAT_ERRNO_SYSTEM_ERR,
    /* Following entry is only to ensure the error code of int size */
    TFM_PLAT_ERRNO_FORCE_INT_SIZE = INT_MAX
};

#ifdef __cplusplus
}
#endif

#endif /* __PLAT_DEFS_H__ */
