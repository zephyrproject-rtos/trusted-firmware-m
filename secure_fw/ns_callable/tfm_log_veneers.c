/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_log_veneers.h"
#include "secure_fw/services/audit_logging/log_core.h"
#include "tfm_secure_api.h"
#include "tfm_api.h"
#include "spm_partition_defs.h"

__tfm_secure_gateway_attributes__
enum tfm_log_err tfm_log_veneer_retrieve(uint32_t size,
                                         uint8_t *buffer,
                                         uint32_t *log_size)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_AUDIT_LOG_ID, log_core_retrieve,
                         size, buffer, log_size, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_log_err tfm_log_veneer_add_line(struct tfm_log_line *line)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_AUDIT_LOG_ID, log_core_add_line,
                         line, 0, 0, 0);
}
