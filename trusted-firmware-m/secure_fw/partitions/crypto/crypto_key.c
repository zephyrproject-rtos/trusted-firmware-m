/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "tfm_mbedcrypto_include.h"

#include "tfm_crypto_api.h"
#include "tfm_crypto_defs.h"
#include "tfm_crypto_private.h"

#ifndef TFM_CRYPTO_KEY_MODULE_DISABLED
#ifdef CRYPTO_KEY_ID_ENCODES_OWNER
#ifndef TFM_CRYPTO_MAX_KEY_HANDLES
#define TFM_CRYPTO_MAX_KEY_HANDLES (32)
#endif

struct tfm_crypto_handle_owner_s {
    int32_t owner;           /*!< Owner of the allocated handle */
    psa_key_id_t key;        /*!< Allocated key */
    uint8_t in_use;          /*!< Flag to indicate if this in use */
};

static struct tfm_crypto_handle_owner_s
                                 handle_owner[TFM_CRYPTO_MAX_KEY_HANDLES] = {0};

static void set_handle_owner(uint8_t idx, int32_t client_id,
                             psa_key_id_t key_handle)
{
    /* Skip checking idx */

    handle_owner[idx].owner = client_id;
    handle_owner[idx].key = key_handle;
    handle_owner[idx].in_use = TFM_CRYPTO_IN_USE;
}

static void clean_handle_owner(uint8_t idx)
{
    /* Skip checking idx */

    handle_owner[idx].owner = TFM_INVALID_CLIENT_ID;
    handle_owner[idx].key = (psa_key_id_t)0;
    handle_owner[idx].in_use = TFM_CRYPTO_NOT_IN_USE;
}

static psa_status_t find_empty_handle_owner_slot(uint8_t *idx)
{
    uint8_t i;

    for (i = 0; i < TFM_CRYPTO_MAX_KEY_HANDLES; i++) {
        if (handle_owner[i].in_use == TFM_CRYPTO_NOT_IN_USE) {
            *idx = i;
            return PSA_SUCCESS;
        }
    }

    return PSA_ERROR_INSUFFICIENT_MEMORY;
}

/*
 * Check that the requested handle belongs to the requesting partition
 *
 * Argument idx is optional. It points to the buffer to hold the internal
 * index corresponding to the input handle. Valid only on PSA_SUCCESS.
 * It is filled only if the input pointer is not NULL.
 *
 * Return values as described in \ref psa_status_t
 */
static psa_status_t check_handle_owner(psa_key_id_t key, uint8_t *idx)
{
    int32_t client_id = 0;
    uint8_t i = 0;
    psa_status_t status;

    status = tfm_crypto_get_caller_id(&client_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    for (i = 0; i < TFM_CRYPTO_MAX_KEY_HANDLES; i++) {
        if (handle_owner[i].in_use && handle_owner[i].key == key) {
            if (handle_owner[i].owner == client_id) {
                if (idx) {
                    *idx = i;
                }
                return PSA_SUCCESS;
            } else {
                return PSA_ERROR_NOT_PERMITTED;
            }
        }
    }

    return PSA_ERROR_INVALID_HANDLE;
}

static void encoded_key_id_make(psa_key_id_t key, uint8_t slot_idx,
                                mbedtls_svc_key_id_t *encoded_key)
{
    /* Skip checking encoded_key */
    *encoded_key = mbedtls_svc_key_id_make(handle_owner[slot_idx].owner, key);
}
#else /* CRYPTO_KEY_ID_ENCODES_OWNER */
#define set_handle_owner(idx, client_id, key_handle)        do {} while (0)
#define clean_handle_owner(idx)                             do {} while (0)

static inline psa_status_t find_empty_handle_owner_slot(uint8_t *idx)
{
    *idx = 0;

    return PSA_SUCCESS;
}

static inline psa_status_t check_handle_owner(psa_key_id_t key, uint8_t *idx)
{
    (void)key;

    if (idx) {
        *idx = 0;
    }

    return PSA_SUCCESS;
}

static inline void encoded_key_id_make(psa_key_id_t key, uint8_t slot_idx,
                                       mbedtls_svc_key_id_t *encoded_key)
{
    (void)slot_idx;

    /* Skip checking encoded_key */
    *encoded_key = mbedtls_svc_key_id_make(TFM_INVALID_CLIENT_ID, key);
}
#endif /* CRYPTO_KEY_ID_ENCODES_OWNER */
#endif /* !TFM_CRYPTO_KEY_MODULE_DISABLED */

/*!
 * \defgroup public Public functions
 *
 */
/*!@{*/
psa_status_t tfm_crypto_key_attributes_from_client(
                    const struct psa_client_key_attributes_s *client_key_attr,
                    int32_t client_id,
                    psa_key_attributes_t *key_attributes)
{
    psa_core_key_attributes_t *core;

    if (client_key_attr == NULL || key_attributes == NULL) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    *key_attributes = psa_key_attributes_init();
    core = &(key_attributes->MBEDTLS_PRIVATE(core));

    /* Copy core key attributes from the client core key attributes */
    core->MBEDTLS_PRIVATE(type) = client_key_attr->type;
    core->MBEDTLS_PRIVATE(lifetime) = client_key_attr->lifetime;
    core->MBEDTLS_PRIVATE(policy).MBEDTLS_PRIVATE(usage) =
                                                     client_key_attr->usage;
    core->MBEDTLS_PRIVATE(policy).MBEDTLS_PRIVATE(alg) =
                                                     client_key_attr->alg;
    core->MBEDTLS_PRIVATE(bits) = client_key_attr->bits;

    /* Use the client key id as the key_id and its partition id as the owner */
#ifdef CRYPTO_KEY_ID_ENCODES_OWNER
    core->MBEDTLS_PRIVATE(id).MBEDTLS_PRIVATE(key_id) = client_key_attr->id;
    core->MBEDTLS_PRIVATE(id).MBEDTLS_PRIVATE(owner) = client_id;
#else
    core->MBEDTLS_PRIVATE(id) = client_key_attr->id;
#endif

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_key_attributes_to_client(
                        const psa_key_attributes_t *key_attributes,
                        struct psa_client_key_attributes_s *client_key_attr)
{
    if (client_key_attr == NULL || key_attributes == NULL) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    struct psa_client_key_attributes_s v = PSA_CLIENT_KEY_ATTRIBUTES_INIT;
    *client_key_attr = v;
    psa_core_key_attributes_t core = key_attributes->MBEDTLS_PRIVATE(core);

    /* Copy core key attributes from the client core key attributes */
    client_key_attr->type = core.MBEDTLS_PRIVATE(type);
    client_key_attr->lifetime = core.MBEDTLS_PRIVATE(lifetime);
    client_key_attr->usage = core.MBEDTLS_PRIVATE(policy).MBEDTLS_PRIVATE(usage);
    client_key_attr->alg = core.MBEDTLS_PRIVATE(policy).MBEDTLS_PRIVATE(alg);
    client_key_attr->bits = core.MBEDTLS_PRIVATE(bits);

    /* Return the key_id as the client key id, do not return the owner */
#ifdef CRYPTO_KEY_ID_ENCODES_OWNER
    client_key_attr->id = core.MBEDTLS_PRIVATE(id).MBEDTLS_PRIVATE(key_id);
#else
    client_key_attr->id = core.MBEDTLS_PRIVATE(id);
#endif

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_check_handle_owner(psa_key_id_t key)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    return check_handle_owner(key, NULL);
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_encode_id_and_owner(psa_key_id_t key_id,
                                            mbedtls_svc_key_id_t *enc_key_ptr)
{
    int32_t partition_id = 0;
    psa_status_t status = tfm_crypto_get_caller_id(&partition_id);

    if (status != PSA_SUCCESS) {
        return status;
    }

    /* If Null Pointer, return PSA_ERROR_PROGRAMMER_ERROR */
    if (enc_key_ptr == NULL) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    /* Use the client key id as the key_id and its partition id as the owner */
    *enc_key_ptr = mbedtls_svc_key_id_make(partition_id, key_id);

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_check_key_storage(uint32_t *index)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    return find_empty_handle_owner_slot((uint8_t *)index);
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_set_key_storage(uint32_t index,
                                        psa_key_id_t key_handle)
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

    set_handle_owner(index, partition_id, key_handle);

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

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 2, 3, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(struct psa_client_key_attributes_s)) ||
        (out_vec[0].len != sizeof(psa_key_id_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct psa_client_key_attributes_s *client_key_attr = in_vec[1].base;
    const uint8_t *data = in_vec[2].base;
    size_t data_length = in_vec[2].len;
    psa_key_id_t *psa_key = out_vec[0].base;

    psa_status_t status;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t i = 0;
    mbedtls_svc_key_id_t encoded_key;
    int32_t partition_id = 0;

    status = find_empty_handle_owner_slot(&i);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = tfm_crypto_key_attributes_from_client(client_key_attr,
                                                   partition_id,
                                                   &key_attributes);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_import_key(&key_attributes, data, data_length, &encoded_key);
    /* Update the imported key id */
#ifdef CRYPTO_KEY_ID_ENCODES_OWNER
    *psa_key = encoded_key.MBEDTLS_PRIVATE(key_id);
#else
    *psa_key = (psa_key_id_t)encoded_key;
#endif

    if (status == PSA_SUCCESS) {
        set_handle_owner(i, partition_id, *psa_key);
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

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 2, 2, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(psa_key_id_t)) ||
        (out_vec[0].len != sizeof(psa_key_id_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    psa_key_id_t client_key_id = *((psa_key_id_t *)in_vec[1].base);
    psa_key_id_t *key = out_vec[0].base;
    psa_status_t status;
    mbedtls_svc_key_id_t encoded_key;
    int32_t partition_id;
    uint8_t i;

    status = find_empty_handle_owner_slot(&i);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Use the client key id as the key_id and its partition id as the owner */
    encoded_key = mbedtls_svc_key_id_make(partition_id, client_key_id);

    status = psa_open_key(encoded_key, &encoded_key);
#ifdef CRYPTO_KEY_ID_ENCODES_OWNER
    *key = encoded_key.MBEDTLS_PRIVATE(key_id);
#else
    *key = (psa_key_id_t)encoded_key;
#endif

    if (status == PSA_SUCCESS) {
        set_handle_owner(i, partition_id, *key);
    }

    return status;
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

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 0, 0);

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_id_t key = iov->key_id;
    uint8_t index;
    mbedtls_svc_key_id_t encoded_key;
    psa_status_t status;

    status = check_handle_owner(key, &index);
    if (status != PSA_SUCCESS) {
        return status;
    }

    encoded_key_id_make(key, index, &encoded_key);

    status = psa_close_key(encoded_key);
    if (status == PSA_SUCCESS) {
        clean_handle_owner(index);
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

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 0, 0);

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_key_id_t key = iov->key_id;
    uint8_t index;
    mbedtls_svc_key_id_t encoded_key;
    psa_status_t status;

    status = check_handle_owner(key, &index);
    if (status != PSA_SUCCESS) {
        return status;
    }

    encoded_key_id_make(key, index, &encoded_key);

    status = psa_destroy_key(encoded_key);
    if (status == PSA_SUCCESS) {
        clean_handle_owner(index);
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

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(struct psa_client_key_attributes_s))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_id_t key = iov->key_id;
    struct psa_client_key_attributes_s *client_key_attr = out_vec[0].base;
    psa_status_t status;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_svc_key_id_t encoded_key;
    uint8_t index;

    status = check_handle_owner(key, &index);
    if (status != PSA_SUCCESS) {
        return status;
    }

    encoded_key_id_make(key, index, &encoded_key);

    status = psa_get_key_attributes(encoded_key, &key_attributes);
    if (status == PSA_SUCCESS) {
        status = tfm_crypto_key_attributes_to_client(&key_attributes,
                                                     client_key_attr);
    }

    return status;
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

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(struct psa_client_key_attributes_s))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    struct psa_client_key_attributes_s *client_key_attr = out_vec[0].base;
    psa_status_t status;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    int32_t partition_id;

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = tfm_crypto_key_attributes_from_client(client_key_attr,
                                                   partition_id,
                                                   &key_attributes);
    if (status != PSA_SUCCESS) {
        return status;
    }

    psa_reset_key_attributes(&key_attributes);

    return tfm_crypto_key_attributes_to_client(&key_attributes,
                                               client_key_attr);
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

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 0, 1);

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_id_t key = iov->key_id;
    uint8_t *data = out_vec[0].base;
    size_t data_size = out_vec[0].len;
    mbedtls_svc_key_id_t encoded_key;
    psa_status_t status;
    uint8_t index;

    status = check_handle_owner(key, &index);
    if (status != PSA_SUCCESS) {
        return status;
    }

    encoded_key_id_make(key, index, &encoded_key);

    return psa_export_key(encoded_key, data, data_size,
                          &(out_vec[0].len));
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

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 0, 1);

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_key_id_t key = iov->key_id;
    uint8_t *data = out_vec[0].base;
    size_t data_size = out_vec[0].len;
    mbedtls_svc_key_id_t encoded_key;
    psa_status_t status;
    uint8_t index;

    status = check_handle_owner(key, &index);
    if (status != PSA_SUCCESS) {
        return status;
    }

    encoded_key_id_make(key, index, &encoded_key);

    return psa_export_public_key(encoded_key, data, data_size,
                                 &(out_vec[0].len));
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_purge_key(psa_invec in_vec[],
                                  size_t in_len,
                                  psa_outvec out_vec[],
                                  size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    (void)out_vec;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 0, 0);

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_key_id_t key = iov->key_id;
    mbedtls_svc_key_id_t encoded_key;
    psa_status_t status;
    uint8_t index;

    status = check_handle_owner(key, &index);
    if (status != PSA_SUCCESS) {
        return status;
    }

    encoded_key_id_make(key, index, &encoded_key);

    status = psa_purge_key(encoded_key);
    if (status == PSA_SUCCESS) {
        clean_handle_owner(index);
    }

    return status;
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

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 2, 2, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(psa_key_id_t)) ||
        (in_vec[1].len != sizeof(struct psa_client_key_attributes_s))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_id_t source_key_id = iov->key_id;
    psa_key_id_t *target_key_id = out_vec[0].base;
    const struct psa_client_key_attributes_s *client_key_attr = in_vec[1].base;
    psa_status_t status;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t source_idx = 0, target_idx = 0;
    int32_t partition_id = 0;
    mbedtls_svc_key_id_t target_key;
    mbedtls_svc_key_id_t encoded_key;

    status = find_empty_handle_owner_slot(&target_idx);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = tfm_crypto_key_attributes_from_client(client_key_attr,
                                                   partition_id,
                                                   &key_attributes);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = check_handle_owner(source_key_id, &source_idx);
    if (status != PSA_SUCCESS) {
        return status;
    }

    encoded_key_id_make(source_key_id, source_idx, &encoded_key);

    status = psa_copy_key(encoded_key, &key_attributes, &target_key);
#ifdef CRYPTO_KEY_ID_ENCODES_OWNER
    *target_key_id = target_key.MBEDTLS_PRIVATE(key_id);
#else
    *target_key_id = (psa_key_id_t)target_key;
#endif
    if (status == PSA_SUCCESS) {
        set_handle_owner(target_idx, partition_id, *target_key_id);
    }

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_generate_key(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 2, 2, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(struct psa_client_key_attributes_s)) ||
        (out_vec[0].len != sizeof(psa_key_id_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    psa_key_id_t *key_handle = out_vec[0].base;
    const struct psa_client_key_attributes_s *client_key_attr = in_vec[1].base;
    psa_status_t status;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint8_t i = 0;
    int32_t partition_id = 0;
    mbedtls_svc_key_id_t encoded_key;

    status = find_empty_handle_owner_slot(&i);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = tfm_crypto_key_attributes_from_client(client_key_attr,
                                                   partition_id,
                                                   &key_attributes);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_generate_key(&key_attributes, &encoded_key);
#ifdef CRYPTO_KEY_ID_ENCODES_OWNER
    *key_handle = encoded_key.MBEDTLS_PRIVATE(key_id);
#else
    *key_handle = (psa_key_id_t)encoded_key;
#endif

    if (status == PSA_SUCCESS) {
        set_handle_owner(i, partition_id, *key_handle);
    }

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}
/*!@}*/
