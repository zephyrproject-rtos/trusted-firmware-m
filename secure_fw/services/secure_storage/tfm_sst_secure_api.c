/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa_sst_api.h"
#include "tfm_sst_veneers.h"
#include "secure_fw/services/secure_storage/sst_asset_management.h"

enum psa_sst_err_t psa_sst_create(uint32_t asset_uuid, const uint8_t *token,
                                  uint32_t token_size)
{
    struct tfm_sst_token_t s_token;

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

   /* FIXME: Currently, TF-M framework does not provide any mechanism to
    *        identify the secure partition. So, the same partition ID
    *        (S_CLIENT_ID) is used for all the calls.
    */
    return tfm_sst_veneer_create(S_CLIENT_ID, asset_uuid, &s_token);

}

enum psa_sst_err_t psa_sst_get_info(uint32_t asset_uuid,
                                    const uint8_t *token,
                                    uint32_t token_size,
                                    struct psa_sst_asset_info_t *info)
{
    struct tfm_sst_token_t s_token;

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

   /* FIXME: Currently, TF-M framework does not provide any mechanism to
    *        identify the secure partition. So, the same partition ID
    *        (S_CLIENT_ID) is used for all the calls.
    */
    return tfm_sst_veneer_get_info(S_CLIENT_ID, asset_uuid,
                                   &s_token, info);
}

enum psa_sst_err_t psa_sst_get_attributes(uint32_t asset_uuid,
                                          const uint8_t *token,
                                          uint32_t token_size,
                                          struct psa_sst_asset_attrs_t *attrs)
{
    struct tfm_sst_token_t s_token;

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

   /* FIXME: Currently, TF-M framework does not provide any mechanism to
    *        identify the secure partition. So, the same partition ID
    *        (S_CLIENT_ID) is used for all the calls.
    */
    return tfm_sst_veneer_get_attributes(S_CLIENT_ID, asset_uuid,
                                         &s_token, attrs);
}

enum psa_sst_err_t psa_sst_set_attributes(uint32_t asset_uuid,
                                      const uint8_t *token,
                                      uint32_t token_size,
                                      const struct psa_sst_asset_attrs_t *attrs)
{
    struct tfm_sst_token_t s_token;

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

   /* FIXME: Currently, TF-M framework does not provide any mechanism to
    *        identify the secure partition. So, the same partition ID
    *        (S_CLIENT_ID) is used for all the calls.
    */
    return tfm_sst_veneer_set_attributes(S_CLIENT_ID, asset_uuid,
                                         &s_token, attrs);
}

enum psa_sst_err_t psa_sst_read(uint32_t asset_uuid,
                                const uint8_t *token,
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

    return tfm_sst_veneer_read(S_CLIENT_ID, asset_uuid, &s_token, &s_data);
}

enum psa_sst_err_t psa_sst_reference_read(int32_t  client_id,
                                          uint32_t asset_uuid,
                                          const uint8_t *token,
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

    return tfm_sst_veneer_read(client_id, asset_uuid, &s_token, &s_data);
}

enum psa_sst_err_t psa_sst_write(uint32_t asset_uuid,
                                 const uint8_t *token,
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

   /* FIXME: Currently, TF-M framework does not provide any mechanism to
    *        identify the secure partition. So, the same partition ID
    *        (S_CLIENT_ID) is used for all the calls.
    */
    return tfm_sst_veneer_write(S_CLIENT_ID, asset_uuid, &s_token, &s_data);
}

enum psa_sst_err_t psa_sst_delete(uint32_t asset_uuid,
                                  const uint8_t *token,
                                  uint32_t token_size)
{
    struct tfm_sst_token_t s_token;

    /* Pack the token information in the token structure */
    s_token.token = token;
    s_token.token_size = token_size;

   /* FIXME: Currently, TF-M framework does not provide any mechanism to
    *        identify the secure partition. So, the same partition ID
    *        (S_CLIENT_ID) is used for all the calls.
    */
    return tfm_sst_veneer_delete(S_CLIENT_ID, asset_uuid, &s_token);
}
