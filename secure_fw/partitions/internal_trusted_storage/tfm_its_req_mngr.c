/*
 * Copyright (c) 2019-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_its_req_mngr.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "psa/storage_common.h"
#include "tfm_internal_trusted_storage.h"
#include "its_utils.h"

#ifdef TFM_PSA_API
#include "psa/framework_feature.h"
#include "psa/service.h"
#include "psa_manifest/tfm_internal_trusted_storage.h"
#include "tfm_its_defs.h"
#if PSA_FRAMEWORK_HAS_MM_IOVEC != 1
#include "flash/its_flash.h"
#endif /* PSA_FRAMEWORK_HAS_MM_IOVEC != 1 */
#else
#include <stdbool.h>
#include "tfm_secure_api.h"
#include "tfm_api.h"
#endif

#if defined(TFM_PSA_API) && PSA_FRAMEWORK_HAS_MM_IOVEC != 1
#ifndef ITS_BUF_SIZE
/* By default, set the ITS buffer size to the max asset size so that all
 * requests can be handled in one iteration.
 */
#define ITS_BUF_SIZE ITS_MAX_ASSET_SIZE
#endif

/* Buffer to store asset data from the caller.
 * Note: size must be aligned to the max flash program unit to meet the
 * alignment requirement of the filesystem.
 */
static uint8_t asset_data[ITS_UTILS_ALIGN(ITS_BUF_SIZE,
                                          ITS_FLASH_MAX_ALIGNMENT)];
#endif

#ifndef TFM_PSA_API
/**
 * \brief Indicates whether ITS has been initialised.
 */
static bool its_is_init = false;

psa_status_t tfm_its_set_req(psa_invec *in_vec, size_t in_len,
                             psa_outvec *out_vec, size_t out_len)
{
    psa_storage_uid_t uid;
    uint8_t *p_data;
    size_t data_length;
    psa_storage_create_flags_t create_flags;
    struct its_asset_info asset_info;
    int32_t client_id;

    (void)out_vec;

    if (!its_is_init) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    if ((in_len != 3) || (out_len != 0)) {
        /* The number of arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    if (in_vec[0].len != sizeof(uid) ||
        in_vec[2].len != sizeof(create_flags)) {
        /* The size of one of the arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    uid = *((psa_storage_uid_t *)in_vec[0].base);

    p_data = (uint8_t *)in_vec[1].base;
    data_length = in_vec[1].len;

    create_flags = *(psa_storage_create_flags_t *)in_vec[2].base;

    /* Get the caller's client ID */
    if (tfm_core_get_caller_client_id(&client_id) != (int32_t)TFM_SUCCESS) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    asset_info.uid = uid;
    asset_info.client_id = client_id;
    asset_info.create_flags = create_flags;

    return tfm_its_set(&asset_info, p_data, data_length,
                       data_length, 0);
}

psa_status_t tfm_its_get_req(psa_invec *in_vec, size_t in_len,
                             psa_outvec *out_vec, size_t out_len)
{
    psa_storage_uid_t uid;
    struct its_asset_info asset_info;
    uint8_t *p_data;
    size_t data_offset;
    size_t data_size;
    size_t *p_data_length;
    int32_t client_id;

    if (!its_is_init) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    if ((in_len != 2) || (out_len != 1)) {
        /* The number of arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    if (in_vec[0].len != sizeof(uid) ||
        in_vec[1].len != sizeof(data_offset)) {
        /* The size of one of the arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    uid = *((psa_storage_uid_t *)in_vec[0].base);

    data_offset = *(size_t *)in_vec[1].base;

    p_data = (uint8_t *)out_vec[0].base;
    data_size = out_vec[0].len;

    p_data_length = &out_vec[0].len;

    /* Get the caller's client ID */
    if (tfm_core_get_caller_client_id(&client_id) != (int32_t)TFM_SUCCESS) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    asset_info.uid = uid;
    asset_info.client_id = client_id;

    return tfm_its_get(&asset_info, p_data, data_size,
                       data_offset, p_data_length, true);
}

psa_status_t tfm_its_get_info_req(psa_invec *in_vec, size_t in_len,
                                  psa_outvec *out_vec, size_t out_len)
{
    psa_storage_uid_t uid;
    struct psa_storage_info_t *p_info;
    int32_t client_id;

    if (!its_is_init) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    if ((in_len != 1) || (out_len != 1)) {
        /* The number of arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    if (in_vec[0].len != sizeof(uid) ||
        out_vec[0].len != sizeof(*p_info)) {
        /* The size of one of the arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    uid = *((psa_storage_uid_t *)in_vec[0].base);

    p_info = (struct psa_storage_info_t *)out_vec[0].base;

    /* Get the caller's client ID */
    if (tfm_core_get_caller_client_id(&client_id) != (int32_t)TFM_SUCCESS) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    return tfm_its_get_info(client_id, uid, p_info);
}

psa_status_t tfm_its_remove_req(psa_invec *in_vec, size_t in_len,
                                psa_outvec *out_vec, size_t out_len)
{
    psa_storage_uid_t uid;
    int32_t client_id;

    (void)out_vec;

    if (!its_is_init) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    if ((in_len != 1) || (out_len != 0)) {
        /* The number of arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    if (in_vec[0].len != sizeof(uid)) {
        /* The input argument size is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    uid = *((psa_storage_uid_t *)in_vec[0].base);

    /* Get the caller's client ID */
    if (tfm_core_get_caller_client_id(&client_id) != (int32_t)TFM_SUCCESS) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    return tfm_its_remove(client_id, uid);
}

#else /* !defined(TFM_PSA_API) */
static psa_status_t tfm_its_set_req(const psa_msg_t *msg)
{
    psa_status_t status;
    psa_storage_uid_t uid;
    uint8_t *data_buf;
    size_t size_remaining;
    size_t offset;
    psa_storage_create_flags_t create_flags;
    struct its_asset_info asset_info;
    size_t num;

    if (msg->in_size[0] != sizeof(uid) ||
        msg->in_size[2] != sizeof(create_flags)) {
        /* The size of one of the arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    num = psa_read(msg->handle, 0, &uid, sizeof(uid));
    if (num != sizeof(uid)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    num = psa_read(msg->handle, 2, &create_flags, sizeof(create_flags));
    if (num != sizeof(create_flags)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    asset_info.uid = uid;
    asset_info.client_id = msg->client_id;
    asset_info.create_flags = create_flags;

    size_remaining = msg->in_size[1];
    offset = 0;

#if PSA_FRAMEWORK_HAS_MM_IOVEC == 1
    if (size_remaining != 0) {
        data_buf = (uint8_t *)psa_map_invec(msg->handle, 1);
    } else {
        /* zero-size asset is supported */
        data_buf = NULL;
    }

    status = tfm_its_set(&asset_info, data_buf, size_remaining,
                         size_remaining, offset);
#else
    data_buf = asset_data;
    do {
        num = psa_read(msg->handle, 1, asset_data,
                       ITS_UTILS_MIN(size_remaining, sizeof(asset_data)));

        status = tfm_its_set(&asset_info, data_buf, size_remaining,
                             num, offset);
        if (status != PSA_SUCCESS) {
            return status;
        }

        size_remaining -= num;
        offset += num;
    } while (size_remaining);
#endif

    return status;
}

static psa_status_t tfm_its_get_req(const psa_msg_t *msg)
{
    psa_status_t status;
    psa_storage_uid_t uid;
    uint8_t *data_buf;
    size_t size_to_read;
    size_t data_offset;
    size_t out_size;
    size_t size_read;
    size_t num;
    struct its_asset_info asset_info;
    bool first_get;

    if (msg->in_size[0] != sizeof(uid) ||
        msg->in_size[1] != sizeof(data_offset)) {
        /* The size of one of the arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    num = psa_read(msg->handle, 0, &uid, sizeof(uid));
    if (num != sizeof(uid)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    num = psa_read(msg->handle, 1, &data_offset, sizeof(data_offset));
    if (num != sizeof(data_offset)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    asset_info.uid = uid;
    asset_info.client_id = msg->client_id;
    out_size = msg->out_size[0];
    first_get = true;

#if PSA_FRAMEWORK_HAS_MM_IOVEC == 1
    size_to_read = msg->out_size[0];
    if (size_to_read != 0) {
        data_buf = (uint8_t *)psa_map_outvec(msg->handle, 0);
    } else {
        data_buf = NULL;
    }

    status = tfm_its_get(&asset_info, data_buf, size_to_read,
                         data_offset, &size_read, first_get);
    if (status == PSA_SUCCESS && size_to_read != 0) {
        /* Unmap to update caller’s outvec with the number of bytes written  */
        psa_unmap_outvec(msg->handle, 0, size_read);
    }
#else
    /* Fill in the outvec unless no data left */
    data_buf = asset_data;
    do {
        size_to_read = ITS_UTILS_MIN(out_size, sizeof(asset_data));
        status = tfm_its_get(&asset_info, data_buf, size_to_read,
                             data_offset, &size_read, first_get);
        if (status != PSA_SUCCESS) {
            return status;
        }
        if (size_read == 0) {
            /* No more data */
            return PSA_SUCCESS;
        }

        psa_write(msg->handle, 0, data_buf, size_read);

        first_get = false;
        out_size -= size_read;
        data_offset += size_read;
    } while (out_size > 0);
#endif

    return status;
}

static psa_status_t tfm_its_get_info_req(const psa_msg_t *msg)
{
    psa_status_t status;
    psa_storage_uid_t uid;
    struct psa_storage_info_t info;
    size_t num;

    if (msg->in_size[0] != sizeof(uid) ||
        msg->out_size[0] != sizeof(info)) {
        /* The size of one of the arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    num = psa_read(msg->handle, 0, &uid, sizeof(uid));
    if (num != sizeof(uid)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    status = tfm_its_get_info(msg->client_id, uid, &info);
    if (status == PSA_SUCCESS) {
        psa_write(msg->handle, 0, &info, sizeof(info));
    }

    return status;
}

static psa_status_t tfm_its_remove_req(const psa_msg_t *msg)
{
    psa_storage_uid_t uid;
    size_t num;

    if (msg->in_size[0] != sizeof(uid)) {
        /* The input argument size is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    num = psa_read(msg->handle, 0, &uid, sizeof(uid));
    if (num != sizeof(uid)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    return tfm_its_remove(msg->client_id, uid);
}

#endif /* !defined(TFM_PSA_API) */

psa_status_t tfm_its_entry(void)
{
#ifdef TFM_PSA_API
    return tfm_its_init();
#else
    if (tfm_its_init() != PSA_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }
    its_is_init = true;
    return PSA_SUCCESS;
#endif
}

#ifdef TFM_PSA_API
psa_status_t tfm_internal_trusted_storage_service_sfn(const psa_msg_t *msg)
{
    switch (msg->type) {
    case TFM_ITS_SET:
        return tfm_its_set_req(msg);
    case TFM_ITS_GET:
        return tfm_its_get_req(msg);
    case TFM_ITS_GET_INFO:
        return tfm_its_get_info_req(msg);
    case TFM_ITS_REMOVE:
        return tfm_its_remove_req(msg);
    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }

    return PSA_ERROR_GENERIC_ERROR;
}
#endif /* TFM_PSA_API */
