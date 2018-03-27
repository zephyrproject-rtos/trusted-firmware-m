/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_sst_veneers.h"
#include "secure_fw/services/secure_storage/sst_asset_management.h"
#include "tfm_secure_api.h"
#include "tfm_api.h"
#include "secure_fw/spm/spm_api.h"

__tfm_secure_gateway_attributes__
enum tfm_sst_err_t tfm_sst_veneer_get_handle(uint32_t app_id,
                                             uint16_t asset_uuid,
                                             uint32_t *hdl)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_get_handle,
                             app_id, asset_uuid, hdl, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_sst_err_t tfm_sst_veneer_create(uint32_t app_id, uint16_t asset_uuid)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_create,
                             app_id, asset_uuid, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_sst_err_t tfm_sst_veneer_get_attributes(
                                             uint32_t app_id,
                                             uint32_t asset_handle,
                                             struct tfm_sst_attribs_t *attrib)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_get_attributes,
                             app_id, asset_handle, attrib, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_sst_err_t tfm_sst_veneer_read(uint32_t app_id, uint32_t asset_handle,
                                       struct tfm_sst_buf_t *data)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_read, app_id,
                             asset_handle, data, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_sst_err_t tfm_sst_veneer_write(uint32_t app_id, uint32_t asset_handle,
                                        struct tfm_sst_buf_t *data)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_write, app_id,
                             asset_handle, data, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_sst_err_t tfm_sst_veneer_delete(uint32_t app_id, uint32_t asset_handle)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_delete, app_id,
                             asset_handle, 0, 0);
}

