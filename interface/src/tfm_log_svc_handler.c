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
                                      int32_t start,
                                      uint8_t* buffer,
                                      struct tfm_log_info *info)
{
    return tfm_log_veneer_retrieve(size, start, buffer, info);
}

enum tfm_log_err tfm_log_svc_get_info(struct tfm_log_info *info)
{
    return tfm_log_veneer_get_info(info);
}

enum tfm_log_err tfm_log_svc_delete_items(uint32_t num_items,
                                          uint32_t *rem_items)
{
    return tfm_log_veneer_delete_items(num_items, rem_items);
}
