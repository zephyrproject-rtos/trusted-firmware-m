/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_sst_defs.h"
#include "tfm_ns_lock.h"

enum psa_sst_err_t psa_sst_create(uint32_t asset_uuid, const uint8_t* token,
                                  uint32_t token_size)
{
    struct tfm_sst_token_t s_token;

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_CREATE,
                                    asset_uuid,
                                    (uint32_t)&s_token,
                                    0,
                                    0);
}

enum psa_sst_err_t psa_sst_get_info(uint32_t asset_uuid,
                                    const uint8_t* token,
                                    uint32_t token_size,
                                    struct psa_sst_asset_info_t *info)
{
    struct tfm_sst_token_t s_token;

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_GET_INFO,
                                    asset_uuid,
                                    (uint32_t)&s_token,
                                    (uint32_t)info,
                                    0);
}

enum psa_sst_err_t psa_sst_get_attributes(uint32_t asset_uuid,
                                          const uint8_t* token,
                                          uint32_t token_size,
                                          struct psa_sst_asset_attrs_t *attrs)
{
    struct tfm_sst_token_t s_token;

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_GET_ATTRIBUTES,
                                    asset_uuid,
                                    (uint32_t)&s_token,
                                    (uint32_t)attrs,
                                    0);
}

enum psa_sst_err_t psa_sst_set_attributes(uint32_t asset_uuid,
                                      const uint8_t* token,
                                      uint32_t token_size,
                                      const struct psa_sst_asset_attrs_t *attrs)
{
    struct tfm_sst_token_t s_token;

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_SET_ATTRIBUTES,
                                    asset_uuid,
                                    (uint32_t)&s_token,
                                    (uint32_t)attrs,
                                    0);
}

enum psa_sst_err_t psa_sst_read(uint32_t asset_uuid,
                                const uint8_t* token,
                                uint32_t token_size,
                                uint32_t size,
                                uint32_t offset,
                                uint8_t *data)
{
    struct tfm_sst_token_t s_token;
    struct tfm_sst_buf_t   s_data;

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

    /* Pack buffer information in the buffer structure */
    s_data.size = size;
    s_data.offset = offset;
    s_data.data = data;

    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_READ,
                                    asset_uuid,
                                    (uint32_t)&s_token,
                                    (uint32_t)&s_data,
                                    0);
}

enum psa_sst_err_t psa_sst_write(uint32_t asset_uuid,
                                 const uint8_t* token,
                                 uint32_t token_size,
                                 uint32_t size,
                                 uint32_t offset,
                                 const uint8_t *data)
{
    struct tfm_sst_token_t s_token;
    struct tfm_sst_buf_t   s_data;

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

    /* Pack buffer information in the buffer structure */
    s_data.size = size;
    s_data.offset = offset;
    s_data.data = (uint8_t *)data;

    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_WRITE,
                                    asset_uuid,
                                    (uint32_t)&s_token,
                                    (uint32_t)&s_data,
                                    0);
}

enum psa_sst_err_t psa_sst_delete(uint32_t asset_uuid,
                                  const uint8_t* token,
                                  uint32_t token_size)
{
    struct tfm_sst_token_t s_token;

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

    return tfm_ns_lock_svc_dispatch(SVC_TFM_SST_DELETE,
                                    asset_uuid,
                                    (uint32_t)&s_token,
                                    0,
                                    0);
}
