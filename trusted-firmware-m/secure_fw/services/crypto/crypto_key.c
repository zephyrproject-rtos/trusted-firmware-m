/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>

/* FixMe: Use PSA_ERROR_CONNECTION_REFUSED when performing parameter
 *        integrity checks but this will have to be revised
 *        when the full set of error codes mandated by PSA FF
 *        is available.
 */
#include "tfm_mbedcrypto_include.h"

#include "tfm_crypto_api.h"
#include "tfm_crypto_defs.h"
#include <stdbool.h>

#ifndef TFM_CRYPTO_MAX_KEY_HANDLES
#define TFM_CRYPTO_MAX_KEY_HANDLES (16)
#endif
struct tfm_crypto_handle_owner_s {
    int32_t owner;           /*!< Owner of the allocated handle */
    psa_key_handle_t handle; /*!< Allocated handle */
    uint8_t in_use;          /*!< Flag to indicate if this in use */
};

#ifndef TFM_CRYPTO_KEY_MODULE_DISABLED
static struct tfm_crypto_handle_owner_s
                                 handle_owner[TFM_CRYPTO_MAX_KEY_HANDLES] = {0};
#endif

/*!
 * \defgroup public Public functions
 *
 */

/*!@{*/
psa_status_t tfm_crypto_check_handle_owner(psa_key_handle_t handle,
                                           uint32_t *index)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    int32_t partition_id = 0;
    uint32_t i = 0;
    psa_status_t status;

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    for (i = 0; i < TFM_CRYPTO_MAX_KEY_HANDLES; i++) {
        if (handle_owner[i].in_use && handle_owner[i].handle == handle) {
            if (handle_owner[i].owner == partition_id) {
                if (index != NULL) {
                    *index = i;
                }
                return PSA_SUCCESS;
            } else {
                return PSA_ERROR_NOT_PERMITTED;
            }
        }
    }

    return PSA_ERROR_INVALID_HANDLE;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_check_key_storage(uint32_t *index)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    uint32_t i;

    for (i = 0; i < TFM_CRYPTO_MAX_KEY_HANDLES; i++) {
        if (handle_owner[i].in_use == TFM_CRYPTO_NOT_IN_USE) {
            *index = i;
            return PSA_SUCCESS;
        }
    }

    return PSA_ERROR_INSUFFICIENT_MEMORY;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_set_key_storage(uint32_t index,
                                        psa_key_handle_t key_handle)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    int32_t partition_id;

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    handle_owner[index].owner = partition_id;
    handle_owner[index].handle = key_handle;
    handle_owner[index].in_use = TFM_CRYPTO_IN_USE;

    return PSA_SUCCESS;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_set_key_domain_parameters(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    /* FixMe: To be implemented */
    return PSA_ERROR_NOT_SUPPORTED;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_get_key_domain_parameters(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    /* FixMe: To be implemented */
    return PSA_ERROR_NOT_SUPPORTED;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_import_key(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else

    if ((in_len != 3) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(psa_key_attributes_t)) ||
        (out_vec[0].len != sizeof(psa_key_handle_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const psa_key_attributes_t *key_attributes = in_vec[1].base;
    const uint8_t *data = in_vec[2].base;
    size_t data_length = in_vec[2].len;
    psa_key_handle_t *key_handle = out_vec[0].base;
    psa_status_t status;
    uint32_t i = 0;
    int32_t partition_id = 0;
    bool empty_found = false;

    for (i = 0; i < TFM_CRYPTO_MAX_KEY_HANDLES; i++) {
        if (handle_owner[i].in_use == TFM_CRYPTO_NOT_IN_USE) {
            empty_found = true;
            break;
        }
    }

    if (!empty_found) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status =  psa_import_key(key_attributes, data, data_length, key_handle);

    if (status == PSA_SUCCESS) {
        handle_owner[i].owner = partition_id;
        handle_owner[i].handle = *key_handle;
        handle_owner[i].in_use = TFM_CRYPTO_IN_USE;
    }

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_open_key(psa_invec in_vec[],
                                 size_t in_len,
                                 psa_outvec out_vec[],
                                 size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    if ((in_len != 2) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(psa_key_id_t)) ||
        (out_vec[0].len != sizeof(psa_key_handle_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    return PSA_ERROR_NOT_SUPPORTED;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_close_key(psa_invec in_vec[],
                                  size_t in_len,
                                  psa_outvec out_vec[],
                                  size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    (void)out_vec;

    if ((in_len != 1) || (out_len != 0)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    uint32_t index;
    psa_status_t status = tfm_crypto_check_handle_owner(key, &index);

    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_close_key(key);

    if (status == PSA_SUCCESS) {
        handle_owner[index].owner = 0;
        handle_owner[index].handle = 0;
        handle_owner[index].in_use = TFM_CRYPTO_NOT_IN_USE;
    }

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_destroy_key(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    (void)out_vec;

    if ((in_len != 1) || (out_len != 0)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    uint32_t index;
    psa_status_t status = tfm_crypto_check_handle_owner(key, &index);

    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_destroy_key(key);

    if (status == PSA_SUCCESS) {
        handle_owner[index].owner = 0;
        handle_owner[index].handle = 0;
        handle_owner[index].in_use = TFM_CRYPTO_NOT_IN_USE;
    }

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_get_key_attributes(psa_invec in_vec[],
                                           size_t in_len,
                                           psa_outvec out_vec[],
                                           size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(psa_key_attributes_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    psa_key_attributes_t *key_attributes = out_vec[0].base;

    return psa_get_key_attributes(key, key_attributes);
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_reset_key_attributes(psa_invec in_vec[],
                                             size_t in_len,
                                             psa_outvec out_vec[],
                                             size_t out_len)
{
#if (TFM_CRYPTO_KEY_MODULE_DISABLED != 0)
    return PSA_ERROR_NOT_SUPPORTED;
#else
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(psa_key_attributes_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    psa_key_attributes_t *key_attributes = out_vec[0].base;

    psa_reset_key_attributes(key_attributes);

    /* psa_reset_key_attributes() doesn't report any error */
    return PSA_SUCCESS;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_export_key(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    uint8_t *data = out_vec[0].base;
    size_t data_size = out_vec[0].len;

    return psa_export_key(key, data, data_size, &(out_vec[0].len));
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_export_public_key(psa_invec in_vec[],
                                          size_t in_len,
                                          psa_outvec out_vec[],
                                          size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    uint8_t *data = out_vec[0].base;
    size_t data_size = out_vec[0].len;

    return psa_export_public_key(key, data, data_size, &(out_vec[0].len));
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_copy_key(psa_invec in_vec[],
                                 size_t in_len,
                                 psa_outvec out_vec[],
                                 size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else

    if ((in_len != 2) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(psa_key_handle_t)) ||
        (in_vec[1].len != sizeof(psa_key_attributes_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t source_handle = iov->key_handle;
    psa_key_handle_t *target_handle = out_vec[0].base;
    const psa_key_attributes_t *key_attributes = in_vec[1].base;
    psa_status_t status;
    uint32_t i = 0;
    int32_t partition_id = 0;
    bool empty_found = false;

    for (i = 0; i < TFM_CRYPTO_MAX_KEY_HANDLES; i++) {
        if (handle_owner[i].in_use == TFM_CRYPTO_NOT_IN_USE) {
            empty_found = true;
            break;
        }
    }

    if (!empty_found) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_copy_key(source_handle, key_attributes, target_handle);

    if (status == PSA_SUCCESS) {
        handle_owner[i].owner = partition_id;
        handle_owner[i].handle = *target_handle;
        handle_owner[i].in_use = TFM_CRYPTO_IN_USE;
    }

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t tfm_crypto_generate_key(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    if ((in_len != 2) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(psa_key_attributes_t)) ||
        (out_vec[0].len != sizeof(psa_key_handle_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    psa_key_handle_t *key_handle = out_vec[0].base;
    const psa_key_attributes_t *key_attributes = in_vec[1].base;
    psa_status_t status;
    uint32_t i = 0;
    int32_t partition_id = 0;
    bool empty_found = false;

    for (i = 0; i < TFM_CRYPTO_MAX_KEY_HANDLES; i++) {
        if (handle_owner[i].in_use == TFM_CRYPTO_NOT_IN_USE) {
            empty_found = true;
            break;
        }
    }

    if (!empty_found) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_generate_key(key_attributes, key_handle);

    if (status == PSA_SUCCESS) {
        handle_owner[i].owner = partition_id;
        handle_owner[i].handle = *key_handle;
        handle_owner[i].in_use = TFM_CRYPTO_IN_USE;
    }

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}
/*!@}*/
