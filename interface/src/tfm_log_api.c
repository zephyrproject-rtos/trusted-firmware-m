/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_log_defs.h"
#include "tfm_ns_lock.h"

enum tfm_log_err tfm_log_retrieve(uint32_t size,
                                  uint8_t *buffer,
                                  uint32_t *log_size)
{
    return tfm_ns_lock_svc_dispatch(SVC_TFM_LOG_RETRIEVE,
                                    (uint32_t)size,
                                    (uint32_t)buffer,
                                    (uint32_t)log_size,
                                    0);
}
