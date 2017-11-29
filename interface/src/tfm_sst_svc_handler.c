/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "tfm_ns_svc.h"
#include "tfm_sst_veneers.h"
#include "tfm_id_mngr.h"

/* SVC function implementations */
enum tfm_sst_err_t tfm_sst_svc_get_handle(uint16_t asset_uuid,
                                          uint32_t* hdl)
{
    uint32_t app_id;

    app_id = tfm_sst_get_cur_id();

    return tfm_sst_veneer_get_handle(app_id, asset_uuid, hdl);
}

enum tfm_sst_err_t tfm_sst_svc_create(uint16_t asset_uuid)
{
    uint32_t app_id;

    app_id = tfm_sst_get_cur_id();

    return tfm_sst_veneer_create(app_id, asset_uuid);
}

enum tfm_sst_err_t tfm_sst_svc_get_attributes(uint32_t asset_handle,
                                        struct tfm_sst_attribs_t* attrib_struct)
{
    uint32_t app_id;

    app_id = tfm_sst_get_cur_id();

    return tfm_sst_veneer_get_attributes(app_id, asset_handle, attrib_struct);
}

enum tfm_sst_err_t tfm_sst_svc_read(uint32_t asset_handle,
                                    struct tfm_sst_buf_t* data)
{
    uint32_t app_id;

    app_id = tfm_sst_get_cur_id();

    return tfm_sst_veneer_read(app_id, asset_handle, data);
}

enum tfm_sst_err_t tfm_sst_svc_write(uint32_t asset_handle,
                                     struct tfm_sst_buf_t* data)
{
    uint32_t app_id;

    app_id = tfm_sst_get_cur_id();

    return tfm_sst_veneer_write(app_id, asset_handle, data);
}

enum tfm_sst_err_t tfm_sst_svc_delete(uint32_t asset_handle)
{
    uint32_t app_id;

    app_id = tfm_sst_get_cur_id();

    return tfm_sst_veneer_delete(app_id, asset_handle);
}
