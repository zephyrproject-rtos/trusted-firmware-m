/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PLAT_DEFS_H__
#define __TFM_PLAT_DEFS_H__
/**
 * \note The interfaces defined in this file must be implemented for each
 *       target.
 */

#include <stdint.h>
#include <limits.h>

enum tfm_plat_errno_t {
    TFM_PLAT_ERRNO_SUCCESS = 0,
    TFM_PLAT_ERRNO_SYSTEM_ERR,
    /* Following entry is only to ensure the error code of int size */
    TFM_PLAT_ERRNO_FORCE_INT_SIZE = INT_MAX
};

#endif /* __TFM_PLAT_DEFS_H__ */
