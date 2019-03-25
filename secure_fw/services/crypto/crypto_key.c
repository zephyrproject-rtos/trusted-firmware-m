/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include <stddef.h>

#include "tfm_crypto_api.h"
#include "psa_crypto.h"
#include "tfm_crypto_defs.h"
#include "secure_fw/core/tfm_memory_utils.h"

/**
 * \brief This is the default value of maximum number of simultaneous
 *        key stores supported.
 */
#ifndef TFM_CRYPTO_KEY_STORAGE_NUM
#define TFM_CRYPTO_KEY_STORAGE_NUM (4)
#endif

/**
 * \brief This is the default value of the maximum supported key length
 *        in bytes.
 */
#ifndef TFM_CRYPTO_MAX_KEY_LENGTH
#define TFM_CRYPTO_MAX_KEY_LENGTH (64)
#endif

struct tfm_crypto_key_storage_s {
    uint8_t in_use;                 /*!< Indicates if the key store is in use */
    psa_key_type_t type;            /*!< Type of the key stored */
    psa_key_policy_t policy;        /*!< Policy of the key stored */
    psa_key_lifetime_t lifetime;    /*!< Lifetime of the key stored */
    size_t data_length;             /*!< Length of the key stored */
    uint8_t data[TFM_CRYPTO_MAX_KEY_LENGTH]; /*!< Buffer containining the key */
};

static struct tfm_crypto_key_storage_s
                                key_storage[TFM_CRYPTO_KEY_STORAGE_NUM] = {{0}};

/**
 * \brief Get a pointer to the key store for the provided key slot.
 *
 * \param[in] key  Key slot
 *
 * \return Pointer to key store or NULL if key is not a valid key slot
 */
static struct tfm_crypto_key_storage_s *get_key_store(psa_key_slot_t key)
{
    if (key == 0 || key > TFM_CRYPTO_KEY_STORAGE_NUM) {
        return NULL;
    }

    return &key_storage[key - 1];
}

/**
 * \brief Check that the key type is supported and that key_length is a
 *        supported key length for that key type.
 *
 * \param[in] type        Key type
 * \param[in] key_length  Key data length in bytes
 *
 * \return True if the key type is supported and key_length is a supported
 *         key length for that key type, false otherwise
 */
static bool key_type_is_supported(psa_key_type_t type, size_t key_length)
{
    if (key_length > TFM_CRYPTO_MAX_KEY_LENGTH) {
        return false;
    }

    switch (type) {
    case PSA_KEY_TYPE_RAW_DATA:
    case PSA_KEY_TYPE_HMAC:
    case PSA_KEY_TYPE_DERIVE:
        return true; /* No further restictions on these key types */
    case PSA_KEY_TYPE_AES:
    case PSA_KEY_TYPE_CAMELLIA:
        return (key_length == 16 || key_length == 24 || key_length == 32);
    case PSA_KEY_TYPE_DES:
        return (key_length == 8 || key_length == 16 || key_length == 24);
    case PSA_KEY_TYPE_ARC4:
        return key_length >= 1;
    default:
        return false; /* Other key types are not supported */
    }
}

/*!
 * \defgroup public Public functions
 *
 */

/*!@{*/
psa_status_t tfm_crypto_init_key(void)
{
    /* Clear the contents of the local key_storage */
    (void)tfm_memset(key_storage, 0, sizeof(key_storage));
    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_get_key(psa_key_slot_t key,
                                psa_key_usage_t usage,
                                psa_algorithm_t alg,
                                uint8_t *data,
                                size_t data_size,
                                size_t *data_length)
{
    struct tfm_crypto_key_storage_s *key_store;
    size_t i;

    key_store = get_key_store(key);
    if (key_store == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_store->in_use == TFM_CRYPTO_NOT_IN_USE) {
        return PSA_ERROR_EMPTY_SLOT;
    }

    /* Check that usage is permitted for this key */
    if ((usage & key_store->policy.usage) != usage) {
        return PSA_ERROR_NOT_PERMITTED;
    }

    /* Check that alg is compatible with this key */
    if (alg != 0 && alg != key_store->policy.alg) {
        return PSA_ERROR_NOT_PERMITTED;
    }

    if (key_store->data_length > data_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    for (i = 0; i < key_store->data_length; i++) {
        data[i] = key_store->data[i];
    }

    *data_length = key_store->data_length;

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_import_key(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
    struct tfm_crypto_key_storage_s *key_store = NULL;
    size_t i;

    if ((in_len != 3) || (out_len != 0)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(psa_key_slot_t)) ||
        (in_vec[1].len != sizeof(psa_key_type_t))) {
        return PSA_CONNECTION_REFUSED;
    }

    psa_key_slot_t key = *((psa_key_slot_t *)in_vec[0].base);
    psa_key_type_t type = *((psa_key_type_t *)in_vec[1].base);
    const uint8_t *data = in_vec[2].base;
    size_t data_length = in_vec[2].len;

    key_store = get_key_store(key);
    if (key_store == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_store->in_use != TFM_CRYPTO_NOT_IN_USE) {
        return PSA_ERROR_OCCUPIED_SLOT;
    }

    if (!key_type_is_supported(type, data_length)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    key_store->in_use = TFM_CRYPTO_IN_USE;
    key_store->type = type;

    for (i=0; i<data_length; i++) {
        key_store->data[i] = data[i];
    }

    key_store->data_length = data_length;

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_destroy_key(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
    struct tfm_crypto_key_storage_s *key_store = NULL;
    uint32_t i;

    if ((in_len != 1) || (out_len != 0)) {
        return PSA_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(psa_key_slot_t)) {
        return PSA_CONNECTION_REFUSED;
    }

    psa_key_slot_t key = *((psa_key_slot_t *)in_vec[0].base);

    key_store = get_key_store(key);
    if (key_store == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    volatile uint8_t *p_mem = (uint8_t *)key_store;
    uint32_t size_mem = sizeof(struct tfm_crypto_key_storage_s);

    /* memset the key_storage */
    for (i=0; i<size_mem; i++) {
        p_mem[i] = 0;
    }

    /* Set default values */
    key_store->in_use = TFM_CRYPTO_NOT_IN_USE;

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_get_key_information(psa_invec in_vec[],
                                            size_t in_len,
                                            psa_outvec out_vec[],
                                            size_t out_len)
{
    struct tfm_crypto_key_storage_s *key_store = NULL;

    if ((in_len != 1) || (out_len != 2)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(psa_key_slot_t)) ||
        (out_vec[0].len != sizeof(psa_key_type_t)) ||
        (out_vec[1].len != sizeof(size_t))) {
        return PSA_CONNECTION_REFUSED;
    }

    psa_key_slot_t key = *((psa_key_slot_t *)in_vec[0].base);
    psa_key_type_t *type = out_vec[0].base;
    size_t *bits = out_vec[1].base;

    /* Initialise output parameters contents to zero */
    *type = (psa_key_type_t) 0;
    *bits = (size_t) 0;

    key_store = get_key_store(key);
    if (key_store == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_store->in_use == TFM_CRYPTO_NOT_IN_USE) {
        return PSA_ERROR_EMPTY_SLOT;
    }

    /* Get basic metadata */
    *type = key_store->type;
    *bits = PSA_BYTES_TO_BITS(key_store->data_length);

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_export_key(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(psa_key_slot_t)) {
        return PSA_CONNECTION_REFUSED;
    }

    psa_key_slot_t key = *((psa_key_slot_t *)in_vec[0].base);
    uint8_t *data = out_vec[0].base;
    size_t data_size = out_vec[0].len;

    return tfm_crypto_get_key(key, PSA_KEY_USAGE_EXPORT, 0, data, data_size,
                              &(out_vec[0].len));
}

psa_status_t tfm_crypto_export_public_key(psa_invec in_vec[],
                                          size_t in_len,
                                          psa_outvec out_vec[],
                                          size_t out_len)
{
    (void)in_vec;
    (void)in_len;
    (void)out_vec;
    (void)out_len;

    /* FIXME: This API is not supported yet */
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t tfm_crypto_key_policy_init(psa_invec in_vec[],
                                        size_t in_len,
                                        psa_outvec out_vec[],
                                        size_t out_len)
{
    if ((in_len != 0) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if (out_vec[0].len != sizeof(psa_key_policy_t)) {
        return PSA_CONNECTION_REFUSED;
    }

    psa_key_policy_t *policy = out_vec[0].base;

    policy->usage = 0;
    policy->alg = 0;

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_key_policy_set_usage(psa_invec in_vec[],
                                             size_t in_len,
                                             psa_outvec out_vec[],
                                             size_t out_len)
{
    if ((in_len != 2) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((out_vec[0].len != sizeof(psa_key_policy_t)) ||
        (in_vec[0].len != sizeof(psa_key_usage_t))  ||
        (in_vec[1].len != sizeof(psa_algorithm_t))) {
        return PSA_CONNECTION_REFUSED;
    }

    psa_key_policy_t *policy = out_vec[0].base;
    psa_key_usage_t usage = *((psa_key_usage_t *)in_vec[0].base);
    psa_algorithm_t alg = *((psa_algorithm_t *)in_vec[1].base);

    policy->usage = usage;
    policy->alg = alg;

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_key_policy_get_usage(psa_invec in_vec[],
                                             size_t in_len,
                                             psa_outvec out_vec[],
                                             size_t out_len)
{
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(psa_key_policy_t)) ||
        (out_vec[0].len != sizeof(psa_key_usage_t))) {
        return PSA_CONNECTION_REFUSED;
    }

    const psa_key_policy_t *policy = in_vec[0].base;
    psa_key_usage_t *usage = out_vec[0].base;

    *usage = policy->usage;

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_key_policy_get_algorithm(psa_invec in_vec[],
                                                 size_t in_len,
                                                 psa_outvec out_vec[],
                                                 size_t out_len)
{
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(psa_key_policy_t)) ||
        (out_vec[0].len != sizeof(psa_algorithm_t))) {
        return PSA_CONNECTION_REFUSED;
    }

    const psa_key_policy_t *policy = in_vec[0].base;
    psa_algorithm_t *alg = out_vec[0].base;

    *alg = policy->alg;

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_set_key_policy(psa_invec in_vec[],
                                       size_t in_len,
                                       psa_outvec out_vec[],
                                       size_t out_len)
{
    struct tfm_crypto_key_storage_s *key_store = NULL;

    if ((in_len != 2) || (out_len != 0)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(psa_key_slot_t)) ||
        (in_vec[1].len != sizeof(psa_key_policy_t))) {
        return PSA_CONNECTION_REFUSED;
    }

    psa_key_slot_t key = *((psa_key_slot_t *)in_vec[0].base);
    const psa_key_policy_t *policy = in_vec[1].base;

    /* Check that the policy is valid */
    if (policy->usage & ~(PSA_KEY_USAGE_EXPORT
                          | PSA_KEY_USAGE_ENCRYPT
                          | PSA_KEY_USAGE_DECRYPT
                          | PSA_KEY_USAGE_SIGN
                          | PSA_KEY_USAGE_VERIFY
                          | PSA_KEY_USAGE_DERIVE)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    key_store = get_key_store(key);
    if (key_store == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Changing the policy of an occupied slot is not permitted as
     * this is a requirement of the PSA Crypto API
     */
    if (key_store->in_use != TFM_CRYPTO_NOT_IN_USE) {
        return PSA_ERROR_OCCUPIED_SLOT;
    }

    key_store->policy = *policy;

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_get_key_policy(psa_invec in_vec[],
                                       size_t in_len,
                                       psa_outvec out_vec[],
                                       size_t out_len)
{
    struct tfm_crypto_key_storage_s *key_store = NULL;

    if ((in_len != 1) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(psa_key_slot_t)) ||
        (out_vec[0].len != sizeof(psa_key_policy_t))) {
        return PSA_CONNECTION_REFUSED;
    }

    psa_key_slot_t key = *((psa_key_slot_t *)in_vec[0].base);
    psa_key_policy_t *policy = out_vec[0].base;

    key_store = get_key_store(key);
    if (key_store == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    *policy = key_store->policy;

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_set_key_lifetime(psa_invec in_vec[],
                                         size_t in_len,
                                         psa_outvec out_vec[],
                                         size_t out_len)
{
    struct tfm_crypto_key_storage_s *key_store = NULL;

    if ((in_len != 2) || (out_len != 0)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(psa_key_slot_t)) ||
        (in_vec[1].len != sizeof(psa_key_lifetime_t))) {
        return PSA_CONNECTION_REFUSED;
    }

    psa_key_slot_t key = *((psa_key_slot_t *)in_vec[0].base);
    psa_key_lifetime_t lifetime = *((psa_key_lifetime_t *)in_vec[1].base);

    /* Check that the lifetime is valid */
    if (lifetime != PSA_KEY_LIFETIME_VOLATILE
        && lifetime != PSA_KEY_LIFETIME_PERSISTENT
        && lifetime != PSA_KEY_LIFETIME_WRITE_ONCE) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    key_store = get_key_store(key);
    if (key_store == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* TF-M Crypto service does not support changing the lifetime of an occupied
     * slot.
     */
    if (key_store->in_use != TFM_CRYPTO_NOT_IN_USE) {
        return PSA_ERROR_OCCUPIED_SLOT;
    }

    /* Only volatile keys are currently supported */
    if (lifetime != PSA_KEY_LIFETIME_VOLATILE) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    key_store->lifetime = lifetime;

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_get_key_lifetime(psa_invec in_vec[],
                                         size_t in_len,
                                         psa_outvec out_vec[],
                                         size_t out_len)
{
    struct tfm_crypto_key_storage_s *key_store = NULL;

    if ((in_len != 1) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(psa_key_slot_t)) ||
        (out_vec[0].len != sizeof(psa_key_lifetime_t))) {
        return PSA_CONNECTION_REFUSED;
    }

    psa_key_slot_t key = *((psa_key_slot_t *)in_vec[0].base);
    psa_key_lifetime_t *lifetime = out_vec[0].base;

    key_store = get_key_store(key);
    if (key_store == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    *lifetime = key_store->lifetime;

    return PSA_SUCCESS;
}
/*!@}*/
