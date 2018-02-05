/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_ns_svc.h"
#include "tfm_log_veneers.h"

/* SVC function implementations */
enum tfm_log_err tfm_log_svc_retrieve(uint32_t size,
                                      uint8_t* buffer,
                                      uint32_t *log_size)
{
    return tfm_log_veneer_retrieve(size, buffer, log_size);
}
