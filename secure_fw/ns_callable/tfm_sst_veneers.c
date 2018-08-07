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
enum psa_sst_err_t tfm_sst_veneer_create(uint32_t asset_uuid,
                                         const struct tfm_sst_token_t *s_token)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_create,
                         asset_uuid, s_token, 0, 0);
}

__tfm_secure_gateway_attributes__
enum psa_sst_err_t tfm_sst_veneer_get_info(uint32_t asset_uuid,
                                          const struct tfm_sst_token_t *s_token,
                                          struct psa_sst_asset_info_t *info)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_get_info,
                         asset_uuid, s_token, info, 0);
}

__tfm_secure_gateway_attributes__
enum psa_sst_err_t tfm_sst_veneer_get_attributes(uint32_t asset_uuid,
                                          const struct tfm_sst_token_t *s_token,
                                          struct psa_sst_asset_attrs_t *attrs)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_get_attributes,
                         asset_uuid, s_token, attrs, 0);
}

__tfm_secure_gateway_attributes__
enum psa_sst_err_t tfm_sst_veneer_set_attributes(uint32_t asset_uuid,
                                      const struct tfm_sst_token_t *s_token,
                                      const struct psa_sst_asset_attrs_t *attrs)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_set_attributes,
                         asset_uuid, s_token, attrs, 0);
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
enum psa_sst_err_t tfm_sst_veneer_write(uint32_t asset_uuid,
                                        const struct tfm_sst_token_t *s_token,
                                        struct tfm_sst_buf_t *data)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_write,
                         asset_uuid, s_token, data, 0);
}

__tfm_secure_gateway_attributes__
enum psa_sst_err_t tfm_sst_veneer_delete(uint32_t asset_uuid,
                                         const struct tfm_sst_token_t *s_token)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, sst_am_delete,
                         asset_uuid, s_token, 0, 0);
}
