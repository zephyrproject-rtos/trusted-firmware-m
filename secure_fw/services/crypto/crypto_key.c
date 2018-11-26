/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>

#include "tfm_crypto_api.h"
#include "crypto_utils.h"
#include "psa_crypto.h"
#include "tfm_crypto_defs.h"
#include "secure_fw/core/secure_utilities.h"

/**
 * \brief This value defines the maximum number of simultaneous key stores
 *        supported by this implementation.
 */
#define TFM_CRYPTO_KEY_STORAGE_NUM (4)

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
/*!
 * \defgroup public Public functions
 *
 */

/*!@{*/
enum tfm_crypto_err_t tfm_crypto_init_key(void)
{
    /* Clear the contents of the local key_storage */
    tfm_memset(key_storage, 0, sizeof(key_storage));
    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_get_key(psa_key_slot_t key,
                                         psa_key_usage_t usage,
                                         psa_algorithm_t alg,
                                         uint8_t *data,
                                         size_t data_size,
                                         size_t *data_length)
{
    size_t i;

    if (key >= TFM_CRYPTO_KEY_STORAGE_NUM) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_storage[key].in_use == TFM_CRYPTO_NOT_IN_USE) {
        return TFM_CRYPTO_ERR_PSA_ERROR_EMPTY_SLOT;
    }

    /* Check that usage is permitted for this key */
    if ((usage & key_storage[key].policy.usage) != usage) {
        return TFM_CRYPTO_ERR_PSA_ERROR_NOT_PERMITTED;
    }

    /* Check that alg is compatible with this key */
    if (alg != 0 && alg != key_storage[key].policy.alg) {
        return TFM_CRYPTO_ERR_PSA_ERROR_NOT_PERMITTED;
    }

    if (key_storage[key].data_length > data_size) {
        return TFM_CRYPTO_ERR_PSA_ERROR_BUFFER_TOO_SMALL;
    }

    for (i = 0; i < key_storage[key].data_length; i++) {
        data[i] = key_storage[key].data[i];
    }

    *data_length = key_storage[key].data_length;

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_import_key(psa_key_slot_t key,
                                            psa_key_type_t type,
                                            const uint8_t *data,
                                            size_t data_length)
{
    enum tfm_crypto_err_t err;
    size_t i;

    err = tfm_crypto_memory_check((uint8_t *)data, data_length,
                                  TFM_MEMORY_ACCESS_RO);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key >= TFM_CRYPTO_KEY_STORAGE_NUM) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_storage[key].in_use != TFM_CRYPTO_NOT_IN_USE) {
        return TFM_CRYPTO_ERR_PSA_ERROR_OCCUPIED_SLOT;
    }

    if (data_length > TFM_CRYPTO_MAX_KEY_LENGTH) {
        return TFM_CRYPTO_ERR_PSA_ERROR_NOT_SUPPORTED;
    }

    key_storage[key].in_use = TFM_CRYPTO_IN_USE;
    key_storage[key].type = type;

    for (i=0; i<data_length; i++) {
        key_storage[key].data[i] = data[i];
    }

    key_storage[key].data_length = data_length;

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_destroy_key(psa_key_slot_t key)
{
    uint32_t i;

    if (key >= TFM_CRYPTO_KEY_STORAGE_NUM) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    volatile uint8_t *p_mem = (uint8_t *) &(key_storage[key]);
    uint32_t size_mem = sizeof(struct tfm_crypto_key_storage_s);

    /* memset the key_storage */
    for (i=0; i<size_mem; i++) {
        p_mem[i] = 0;
    }

    /* Set default values */
    key_storage[key].in_use = TFM_CRYPTO_NOT_IN_USE;

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_get_key_information(psa_key_slot_t key,
                                                     psa_key_type_t *type,
                                                     size_t *bits)
{
    enum tfm_crypto_err_t err;

    err = tfm_crypto_memory_check(type, sizeof(psa_key_type_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    err = tfm_crypto_memory_check(bits, sizeof(size_t), TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key >= TFM_CRYPTO_KEY_STORAGE_NUM) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_storage[key].in_use == TFM_CRYPTO_NOT_IN_USE) {
        return TFM_CRYPTO_ERR_PSA_ERROR_EMPTY_SLOT;
    }

    /* Get basic metadata */
    *type = key_storage[key].type;
    *bits = PSA_BYTES_TO_BITS(key_storage[key].data_length);

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_export_key(psa_key_slot_t key,
                                            uint8_t *data,
                                            size_t data_size,
                                            size_t *data_length)
{
    enum tfm_crypto_err_t err;

    err = tfm_crypto_memory_check(data, data_size, TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    err = tfm_crypto_memory_check(data_length, sizeof(size_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    return tfm_crypto_get_key(key, PSA_KEY_USAGE_EXPORT, 0, data, data_size,
                              data_length);
}

enum tfm_crypto_err_t tfm_crypto_export_public_key(psa_key_slot_t key,
                                                   uint8_t *data,
                                                   size_t data_size,
                                                   size_t *data_length)
{
    /* FIXME: This API is not supported yet */
    return TFM_CRYPTO_ERR_PSA_ERROR_NOT_SUPPORTED;
}

enum tfm_crypto_err_t tfm_crypto_key_policy_init(psa_key_policy_t *policy)
{
    enum tfm_crypto_err_t err;

    err = tfm_crypto_memory_check(policy, sizeof(psa_key_policy_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    policy->usage = 0;
    policy->alg = 0;

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_key_policy_set_usage(psa_key_policy_t *policy,
                                                      psa_key_usage_t usage,
                                                      psa_algorithm_t alg)
{
    enum tfm_crypto_err_t err;

    err = tfm_crypto_memory_check(policy, sizeof(psa_key_policy_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    policy->usage = usage;
    policy->alg = alg;

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_key_policy_get_usage(
                                                 const psa_key_policy_t *policy,
                                                 psa_key_usage_t *usage)
{
    enum tfm_crypto_err_t err;

    err = tfm_crypto_memory_check((psa_key_policy_t *)policy,
                                  sizeof(psa_key_policy_t),
                                  TFM_MEMORY_ACCESS_RO);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    err = tfm_crypto_memory_check((psa_key_policy_t *)usage,
                                  sizeof(psa_key_usage_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    *usage = policy->usage;

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_key_policy_get_algorithm(
                                                 const psa_key_policy_t *policy,
                                                 psa_algorithm_t *alg)
{
    enum tfm_crypto_err_t err;

    err = tfm_crypto_memory_check((psa_key_policy_t *)policy,
                                  sizeof(psa_key_policy_t),
                                  TFM_MEMORY_ACCESS_RO);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    err = tfm_crypto_memory_check((psa_key_policy_t *)alg,
                                  sizeof(psa_algorithm_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    *alg = policy->alg;

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_set_key_policy(psa_key_slot_t key,
                                                const psa_key_policy_t *policy)
{
    enum tfm_crypto_err_t err;

    err = tfm_crypto_memory_check((psa_key_policy_t *)policy,
                                  sizeof(psa_key_policy_t),
                                  TFM_MEMORY_ACCESS_RO);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key >= TFM_CRYPTO_KEY_STORAGE_NUM) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Check that the policy is valid */
    if (policy->usage & ~(PSA_KEY_USAGE_EXPORT
                          | PSA_KEY_USAGE_ENCRYPT
                          | PSA_KEY_USAGE_DECRYPT
                          | PSA_KEY_USAGE_SIGN
                          | PSA_KEY_USAGE_VERIFY
                          | PSA_KEY_USAGE_DERIVE)) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Changing the policy of an occupied slot is not permitted as
     * this is a requirement of the PSA Crypto API
     */
    if (key_storage[key].in_use != TFM_CRYPTO_NOT_IN_USE) {
        return TFM_CRYPTO_ERR_PSA_ERROR_OCCUPIED_SLOT;
    }

    key_storage[key].policy = *policy;

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_get_key_policy(psa_key_slot_t key,
                                                psa_key_policy_t *policy)
{
    enum tfm_crypto_err_t err;

    err = tfm_crypto_memory_check(policy, sizeof(psa_key_policy_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key >= TFM_CRYPTO_KEY_STORAGE_NUM) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    *policy = key_storage[key].policy;

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_set_key_lifetime(psa_key_slot_t key,
                                                  psa_key_lifetime_t lifetime)
{
    if (key >= TFM_CRYPTO_KEY_STORAGE_NUM) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Check that the lifetime is valid */
    if (lifetime != PSA_KEY_LIFETIME_VOLATILE
        && lifetime != PSA_KEY_LIFETIME_PERSISTENT
        && lifetime != PSA_KEY_LIFETIME_WRITE_ONCE) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* TF-M Crypto service does not support changing the lifetime of an occupied
     * slot.
     */
    if (key_storage[key].in_use != TFM_CRYPTO_NOT_IN_USE) {
        return TFM_CRYPTO_ERR_PSA_ERROR_OCCUPIED_SLOT;
    }

    /* Only volatile keys are currently supported */
    if (lifetime != PSA_KEY_LIFETIME_VOLATILE) {
        return TFM_CRYPTO_ERR_PSA_ERROR_NOT_SUPPORTED;
    }

    key_storage[key].lifetime = lifetime;

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_get_key_lifetime(psa_key_slot_t key,
                                                  psa_key_lifetime_t *lifetime)
{
    enum tfm_crypto_err_t err;

    err = tfm_crypto_memory_check(lifetime, sizeof(psa_key_lifetime_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key >= TFM_CRYPTO_KEY_STORAGE_NUM) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    *lifetime = key_storage[key].lifetime;

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

/*!@}*/
