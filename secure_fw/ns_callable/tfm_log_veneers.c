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
                                         int32_t start,
                                         uint8_t *buffer,
                                         struct tfm_log_info *info)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_AUDIT_LOG_ID, log_core_retrieve,
                         size, start, buffer, info);
}

__tfm_secure_gateway_attributes__
enum tfm_log_err tfm_log_veneer_add_line(struct tfm_log_line *line)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_AUDIT_LOG_ID, log_core_add_line,
                         line, 0, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_log_err tfm_log_veneer_get_info(struct tfm_log_info *info)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_AUDIT_LOG_ID, log_core_get_info,
                         info, 0, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_log_err tfm_log_veneer_delete_items(uint32_t num_items,
                                             uint32_t *rem_items)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_AUDIT_LOG_ID, log_core_delete_items,
                         num_items, rem_items, 0, 0);
}
