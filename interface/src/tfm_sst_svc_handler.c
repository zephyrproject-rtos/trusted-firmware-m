/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "tfm_ns_svc.h"
#include "tfm_sst_veneers.h"
#include "tfm_id_mngr.h"

/* SVC function implementations */
enum psa_sst_err_t tfm_sst_svc_create(uint32_t asset_uuid,
                                      const struct tfm_sst_token_t *s_token)
{
    int32_t client_id;

    client_id = tfm_sst_get_cur_id();

    return tfm_sst_veneer_create(client_id, asset_uuid, s_token);
}

enum psa_sst_err_t tfm_sst_svc_get_info(uint32_t asset_uuid,
                                        const struct tfm_sst_token_t *s_token,
                                        struct psa_sst_asset_info_t *info)
{
    int32_t client_id;

    client_id = tfm_sst_get_cur_id();

    return tfm_sst_veneer_get_info(client_id, asset_uuid, s_token, info);
}

enum psa_sst_err_t tfm_sst_svc_get_attributes(uint32_t asset_uuid,
                                          const struct tfm_sst_token_t *s_token,
                                          struct psa_sst_asset_attrs_t *attrs)
{
    int32_t client_id;

    client_id = tfm_sst_get_cur_id();

    return tfm_sst_veneer_get_attributes(client_id, asset_uuid,
                                         s_token, attrs);
}

enum psa_sst_err_t tfm_sst_svc_set_attributes(uint32_t asset_uuid,
                                      const struct tfm_sst_token_t *s_token,
                                      const struct psa_sst_asset_attrs_t *attrs)
{
    int32_t client_id;

    client_id = tfm_sst_get_cur_id();

    return tfm_sst_veneer_set_attributes(client_id, asset_uuid,
                                         s_token, attrs);
}

enum psa_sst_err_t tfm_sst_svc_read(uint32_t asset_uuid,
                                    const struct tfm_sst_token_t *s_token,
                                    struct tfm_sst_buf_t* data)
{
    int32_t client_id;

    client_id = tfm_sst_get_cur_id();

    return tfm_sst_veneer_read(client_id, asset_uuid, s_token, data);
}

enum psa_sst_err_t tfm_sst_svc_write(uint32_t asset_uuid,
                                     const struct tfm_sst_token_t *s_token,
                                     struct tfm_sst_buf_t* data)
{
    int32_t client_id;

    client_id = tfm_sst_get_cur_id();

    return tfm_sst_veneer_write(client_id, asset_uuid, s_token, data);
}

enum psa_sst_err_t tfm_sst_svc_delete(uint32_t asset_uuid,
                                      const struct tfm_sst_token_t *s_token)
{
    int32_t client_id;

    client_id = tfm_sst_get_cur_id();

    return tfm_sst_veneer_delete(client_id, asset_uuid, s_token);
}
