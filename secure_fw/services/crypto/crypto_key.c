/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
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

#if (TFM_CRYPTO_KEY_MODULE_DISABLED == 0)
static struct tfm_crypto_handle_owner_s
                                 handle_owner[TFM_CRYPTO_MAX_KEY_HANDLES] = {0};
#endif

/**
 * \brief Open a handle to the hardware unique key (HUK).
 *
 * \note As persistent keys are not yet supported by TF-M Crypto, this function
 *       allocates an empty volatile key to get a valid key handle.
 *
 * \param[in]  lifetime  The lifetime of the key
 * \param[out] handle    On success, a handle to the HUK
 *
 * \return Return values as described in \ref psa_status_t
 */
static psa_status_t tfm_crypto_open_huk(psa_key_lifetime_t lifetime,
                                        psa_key_handle_t *key_handle)
{
    psa_status_t status;
    int32_t partition_id;
    uint32_t i;
    psa_key_policy_t huk_policy = PSA_KEY_POLICY_INIT;

    /* The HUK has a persistent lifetime */
    if (lifetime != PSA_KEY_LIFETIME_PERSISTENT) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    for (i = 0; i < TFM_CRYPTO_MAX_KEY_HANDLES; i++) {
        if (handle_owner[i].in_use == TFM_CRYPTO_NOT_IN_USE) {
            break;
        }
    }

    if (i == TFM_CRYPTO_MAX_KEY_HANDLES) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Allocate a transient key to get a valid key handle */
    status = psa_allocate_key(key_handle);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* The HUK can only be used to derive other keys */
    huk_policy.usage = PSA_KEY_USAGE_DERIVE;
    huk_policy.alg = TFM_CRYPTO_ALG_HUK_DERIVATION;
    status = psa_set_key_policy(*key_handle, &huk_policy);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Import zero data to the HUK handle to prevent further modification */
    status = psa_import_key(*key_handle, PSA_KEY_TYPE_RAW_DATA, NULL, 0);

    if (status == PSA_SUCCESS) {
        handle_owner[i].owner = partition_id;
        handle_owner[i].handle = *key_handle;
        handle_owner[i].in_use = TFM_CRYPTO_IN_USE;
    }

    return status;
}

/*!
 * \defgroup public Public functions
 *
 */

/*!@{*/
psa_status_t tfm_crypto_check_handle_owner(psa_key_handle_t handle,
                                           uint32_t *index)
{
#if (TFM_CRYPTO_KEY_MODULE_DISABLED != 0)
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

psa_status_t tfm_crypto_allocate_key(psa_invec in_vec[],
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
        (out_vec[0].len != sizeof(psa_key_handle_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    psa_key_handle_t *key_handle = out_vec[0].base;
    uint32_t i = 0;
    int32_t partition_id = 0;
    bool empty_found = false;
    psa_status_t status;

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

    status = psa_allocate_key(key_handle);

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
#if (TFM_CRYPTO_KEY_MODULE_DISABLED != 0)
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

    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_key_lifetime_t lifetime = iov->lifetime;
    psa_key_id_t id = *((psa_key_id_t *)in_vec[1].base);
    psa_key_handle_t *key_handle = out_vec[0].base;

    /* FIXME: Persistent key APIs are not supported in general, so use a
     * specific implementation to open a handle to the HUK.
     */
    if (id == TFM_CRYPTO_KEY_ID_HUK) {
        return tfm_crypto_open_huk(lifetime, key_handle);
    } else {
        return PSA_ERROR_NOT_SUPPORTED;
    }
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_close_key(psa_invec in_vec[],
                                  size_t in_len,
                                  psa_outvec out_vec[],
                                  size_t out_len)
{
#if (TFM_CRYPTO_KEY_MODULE_DISABLED != 0)
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

psa_status_t tfm_crypto_import_key(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
#if (TFM_CRYPTO_KEY_MODULE_DISABLED != 0)
    return PSA_ERROR_NOT_SUPPORTED;
#else
    (void)out_vec;

    if ((in_len != 2) || (out_len != 0)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    psa_key_type_t type = iov->type;
    const uint8_t *data = in_vec[1].base;
    size_t data_length = in_vec[1].len;
    psa_status_t status = tfm_crypto_check_handle_owner(key, NULL);

    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_import_key(key, type, data, data_length);
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_destroy_key(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
#if (TFM_CRYPTO_KEY_MODULE_DISABLED != 0)
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

psa_status_t tfm_crypto_get_key_information(psa_invec in_vec[],
                                            size_t in_len,
                                            psa_outvec out_vec[],
                                            size_t out_len)
{
#if (TFM_CRYPTO_KEY_MODULE_DISABLED != 0)
    return PSA_ERROR_NOT_SUPPORTED;
#else
    if ((in_len != 1) || (out_len != 2)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(psa_key_type_t)) ||
        (out_vec[1].len != sizeof(size_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    psa_key_type_t *type = out_vec[0].base;
    size_t *bits = out_vec[1].base;

    return psa_get_key_information(key, type, bits);
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_export_key(psa_invec in_vec[],
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
#if (TFM_CRYPTO_KEY_MODULE_DISABLED != 0)
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
#if (TFM_CRYPTO_KEY_MODULE_DISABLED != 0)
    return PSA_ERROR_NOT_SUPPORTED;
#else
    (void)out_vec;

    if ((in_len != 3) || (out_len != 0)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(psa_key_handle_t)) ||
        (in_vec[2].len != sizeof(psa_key_policy_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t source_handle = iov->key_handle;
    psa_key_handle_t target_handle = *((psa_key_handle_t *)in_vec[1].base);
    const psa_key_policy_t *policy = in_vec[2].base;

    return psa_copy_key(source_handle, target_handle, policy);
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_set_key_policy(psa_invec in_vec[],
                                       size_t in_len,
                                       psa_outvec out_vec[],
                                       size_t out_len)
{
#if (TFM_CRYPTO_KEY_MODULE_DISABLED != 0)
    return PSA_ERROR_NOT_SUPPORTED;
#else
    (void)out_vec;

    if ((in_len != 2) || (out_len != 0)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(psa_key_policy_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    const psa_key_policy_t *policy = in_vec[1].base;
    psa_status_t status = tfm_crypto_check_handle_owner(key, NULL);

    if (status == PSA_SUCCESS) {
        return psa_set_key_policy(key, policy);
    } else {
        return status;
    }
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_get_key_policy(psa_invec in_vec[],
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
        (out_vec[0].len != sizeof(psa_key_policy_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    psa_key_policy_t *policy = out_vec[0].base;

    return psa_get_key_policy(key, policy);
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_get_key_lifetime(psa_invec in_vec[],
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
        (out_vec[0].len != sizeof(psa_key_lifetime_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    psa_key_lifetime_t *lifetime = out_vec[0].base;

    return psa_get_key_lifetime(key, lifetime);
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}
/*!@}*/
