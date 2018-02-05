/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "tfm_log_defs.h"
#include "tfm_secure_api.h"

enum tfm_log_err log_core_retrieve(uint32_t size,
                                   uint8_t *buffer,
                                   uint32_t *log_size)
{
    (*(uint32_t*)((void*)buffer)) = 0xDEADBEEF;

    return TFM_LOG_ERR_SUCCESS;
}

enum tfm_log_err log_core_init(void)
{
    return TFM_LOG_ERR_SUCCESS;
}

enum tfm_log_err log_core_add_line(struct tfm_log_line *line)
{
    if (tfm_core_validate_secure_caller() != TFM_SUCCESS) {
        return TFM_LOG_ERR_FAILURE;
    }

    return TFM_LOG_ERR_SUCCESS;
}
