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
#include "secure_fw/spm/spm_partition_defs.h"

__tfm_secure_gateway_attributes__
enum psa_sst_err_t tfm_sst_veneer_create(int32_t client_id,
                                         uint32_t asset_uuid,
                                         const struct tfm_sst_token_t *s_token)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_create,
                         client_id, asset_uuid, s_token, 0);
}

__tfm_secure_gateway_attributes__
enum psa_sst_err_t tfm_sst_veneer_get_info(int32_t client_id,
                                          uint32_t asset_uuid,
                                          const struct tfm_sst_token_t *s_token,
                                          struct psa_sst_asset_info_t *info)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_get_info,
                         client_id, asset_uuid, s_token, info);
}

__tfm_secure_gateway_attributes__
enum psa_sst_err_t tfm_sst_veneer_get_attributes(int32_t client_id,
                                                 uint32_t asset_uuid,
                                          const struct tfm_sst_token_t *s_token,
                                          struct psa_sst_asset_attrs_t *attrs)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_get_attributes,
                         client_id, asset_uuid, s_token, attrs);
}

__tfm_secure_gateway_attributes__
enum psa_sst_err_t tfm_sst_veneer_set_attributes(int32_t client_id,
                                                 uint32_t asset_uuid,
                                      const struct tfm_sst_token_t *s_token,
                                      const struct psa_sst_asset_attrs_t *attrs)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_set_attributes,
                         client_id, asset_uuid, s_token, attrs);
}

__tfm_secure_gateway_attributes__
enum psa_sst_err_t tfm_sst_veneer_read(int32_t client_id,
                                       uint32_t asset_uuid,
                                       const struct tfm_sst_token_t *s_token,
                                       struct tfm_sst_buf_t *data)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_read, client_id,
                         asset_uuid, s_token, data);
}

__tfm_secure_gateway_attributes__
enum psa_sst_err_t tfm_sst_veneer_write(int32_t client_id,
                                        uint32_t asset_uuid,
                                        const struct tfm_sst_token_t *s_token,
                                        struct tfm_sst_buf_t *data)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_write, client_id,
                         asset_uuid, s_token, data);
}

__tfm_secure_gateway_attributes__
enum psa_sst_err_t tfm_sst_veneer_delete(int32_t client_id,
                                         uint32_t asset_uuid,
                                         const struct tfm_sst_token_t *s_token)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_delete, client_id,
                         asset_uuid, s_token, 0);
}
