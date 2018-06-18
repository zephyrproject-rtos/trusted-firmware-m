/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_sst_defs.h"
#include "tfm_ns_lock.h"

enum tfm_sst_err_t tfm_sst_create(uint32_t asset_uuid)
{
    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_CREATE,
                                    asset_uuid,
                                    0,
                                    0,
                                    0);
}

enum tfm_sst_err_t tfm_sst_get_info(uint32_t asset_uuid,
                                    struct tfm_sst_asset_info_t *info)
{
    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_GET_INFO,
                                    asset_uuid,
                                    (uint32_t)info,
                                    0,
                                    0);
}

enum tfm_sst_err_t tfm_sst_get_attributes(uint32_t asset_uuid,
                                          struct tfm_sst_asset_attrs_t *attrs)
{
    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_GET_ATTRIBUTES,
                                    asset_uuid,
                                    (uint32_t)attrs,
                                    0,
                                    0);
}

enum tfm_sst_err_t tfm_sst_set_attributes(uint32_t asset_uuid,
                                      const struct tfm_sst_asset_attrs_t *attrs)
{
    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_SET_ATTRIBUTES,
                                    asset_uuid,
                                    (uint32_t)attrs,
                                    0,
                                    0);
}

enum tfm_sst_err_t tfm_sst_read(uint32_t asset_uuid,
                                struct tfm_sst_buf_t* data)
{
    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_READ,
                                    asset_uuid,
                                    (uint32_t)data,
                                    0,
                                    0);
}

enum tfm_sst_err_t tfm_sst_write(uint32_t asset_uuid,
                                 struct tfm_sst_buf_t* data)
{
    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_WRITE,
                                    asset_uuid,
                                    (uint32_t)data,
                                    0,
                                    0);
}

enum tfm_sst_err_t tfm_sst_delete(uint32_t asset_uuid)
{
    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_DELETE,
                                    asset_uuid,
                                    0,
                                    0,
                                    0);
}
