/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_sst_defs.h"
#include "tfm_ns_lock.h"

enum tfm_sst_err_t tfm_sst_get_handle(uint16_t asset_uuid, uint32_t* hdl)
{
    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_GET_HANDLE,
                                    (uint32_t)asset_uuid,
                                    (uint32_t)hdl,
                                    0,
                                    0);
}

enum tfm_sst_err_t tfm_sst_create(uint16_t asset_uuid)
{
    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_CREATE,
                                    (uint32_t) asset_uuid,
                                    0,
                                    0,
                                    0);
}

enum tfm_sst_err_t tfm_sst_get_info(uint32_t asset_handle,
                                    struct tfm_sst_asset_info_t *info)
{
    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_GET_INFO,
                                    asset_handle,
                                    (uint32_t)info,
                                    0,
                                    0);
}

enum tfm_sst_err_t tfm_sst_read(uint32_t asset_handle, struct tfm_sst_buf_t* data)
{
    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_READ,
                                    (uint32_t)asset_handle,
                                    (uint32_t)data,
                                    0,
                                    0);
}

enum tfm_sst_err_t tfm_sst_write(uint32_t asset_handle, struct tfm_sst_buf_t* data)
{
    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_WRITE,
                                    (uint32_t)asset_handle,
                                    (uint32_t)data,
                                    0,
                                    0);
}

enum tfm_sst_err_t tfm_sst_delete(uint32_t asset_handle)
{
    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_DELETE,
                                    (uint32_t)asset_handle,
                                    0,
                                    0,
                                    0);
}
