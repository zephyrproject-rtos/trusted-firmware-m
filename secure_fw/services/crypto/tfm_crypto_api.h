/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_CRYPTO_API_H__
#define __TFM_CRYPTO_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "tfm_api.h"
#include "tfm_crypto_defs.h"
#include "psa_crypto.h"

#define UNIFORM_SIGNATURE_API(api_name) \
    psa_status_t api_name(psa_invec[], size_t, psa_outvec[], size_t)

/**
 * \brief List of possible operation types supported by the TFM based
 *        implementation. This type is needed by the operation allocation,
 *        lookup and release functions.
 *
 */
enum tfm_crypto_operation_type {
    TFM_CRYPTO_OPERATION_NONE = 0,
    TFM_CRYPTO_CIPHER_OPERATION = 1,
    TFM_CRYPTO_MAC_OPERATION = 2,
    TFM_CRYPTO_HASH_OPERATION = 3,

    /* Used to force the enum size */
    TFM_CRYPTO_OPERATION_TYPE_MAX = INT_MAX
};

/**
 * \brief Initialise the service
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_init(void);

/**
 * \brief Initialise the Key module
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_init_key(void);

/**
 * \brief Initialise the Alloc module
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_init_alloc(void);

/**
 * \brief Allocate an operation context in the backend
 *
 * \param[in]  type Type of the operation context to allocate
 * \param[out] oper Pointer to the frontend operation
 * \param[out  ctx  Double pointer to the corresponding context
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_operation_alloc(enum tfm_crypto_operation_type type,
                                        void *oper,
                                        void **ctx);
/**
 * \brief Release an operation context in the backend
 *
 * \param[in]     type Type of the operation context to release
 * \param[in/out] oper Pointer to the frontend operation for the release
 *                     of the corresponding backend context
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_operation_release(enum tfm_crypto_operation_type type,
                                          void *oper);
/**
 * \brief Look up an operation context in the backend for the corresponding
 *        frontend operation
 *
 * \param[in]  type Type of the operation context to look up
 * \param[in]  oper Pointer to the frontend operation
 * \param[out] ctx  Double pointer to the corresponding context
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_operation_lookup(enum tfm_crypto_operation_type type,
                                         const void *oper,
                                         void **ctx);
/**
 * \brief Retrieve a key from the provided key slot according to the key
 *        policy and algorithm provided. This function is expected to be
 *        called intra-service
 *
 * \param[in]  key         Key slot
 * \param[in]  usage       Usage policy to be used on the retrieved key
 * \param[in]  alg         Algorithm to be used for the retrieved key
 * \param[out] data        Buffer to hold the exported key
 * \param[in]  data_size   Length of the buffer pointed to by data
 * \param[out] data_length Length of the exported key
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_get_key(psa_key_slot_t key,
                                psa_key_usage_t usage,
                                psa_algorithm_t alg,
                                uint8_t *data,
                                size_t data_size,
                                size_t *data_length);

#define LIST_TFM_CRYPTO_UNIFORM_SIGNATURE_API \
    X(tfm_crypto_import_key);                 \
    X(tfm_crypto_destroy_key);                \
    X(tfm_crypto_get_key_information);        \
    X(tfm_crypto_export_key);                 \
    X(tfm_crypto_key_policy_init);            \
    X(tfm_crypto_key_policy_set_usage);       \
    X(tfm_crypto_key_policy_get_usage);       \
    X(tfm_crypto_key_policy_get_algorithm);   \
    X(tfm_crypto_set_key_policy);             \
    X(tfm_crypto_get_key_policy);             \
    X(tfm_crypto_set_key_lifetime);           \
    X(tfm_crypto_get_key_lifetime);           \
    X(tfm_crypto_export_public_key);          \
    X(tfm_crypto_cipher_set_iv);              \
    X(tfm_crypto_cipher_encrypt_setup);       \
    X(tfm_crypto_cipher_decrypt_setup);       \
    X(tfm_crypto_cipher_update);              \
    X(tfm_crypto_cipher_finish);              \
    X(tfm_crypto_cipher_abort);               \
    X(tfm_crypto_hash_setup);                 \
    X(tfm_crypto_hash_update);                \
    X(tfm_crypto_hash_finish);                \
    X(tfm_crypto_hash_verify);                \
    X(tfm_crypto_hash_abort);                 \
    X(tfm_crypto_mac_sign_setup);             \
    X(tfm_crypto_mac_verify_setup);           \
    X(tfm_crypto_mac_update);                 \
    X(tfm_crypto_mac_sign_finish);            \
    X(tfm_crypto_mac_verify_finish);          \
    X(tfm_crypto_mac_abort);                  \
    X(tfm_crypto_aead_encrypt);               \
    X(tfm_crypto_aead_decrypt);               \

#define X(api_name) UNIFORM_SIGNATURE_API(api_name)
LIST_TFM_CRYPTO_UNIFORM_SIGNATURE_API
#undef X

#ifdef __cplusplus
}
#endif

#endif /* __TFM_CRYPTO_API_H__ */
