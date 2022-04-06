/*
 * Copyright (c) 2018-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_CRYPTO_DEFS_H__
#define __TFM_CRYPTO_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <limits.h>
#include "tfm_api.h"
#include "psa/crypto.h"

/**
 * \brief This type is used to overcome a limitation in the number of maximum
 *        IOVECs that can be used especially in psa_aead_encrypt and
 *        psa_aead_decrypt.
 */
#define TFM_CRYPTO_MAX_NONCE_LENGTH (16u)
struct tfm_crypto_aead_pack_input {
    uint8_t nonce[TFM_CRYPTO_MAX_NONCE_LENGTH];
    uint32_t nonce_length;
};

/**
 * \brief Structure used to pack non-pointer types in a call
 *
 */
struct tfm_crypto_pack_iovec {
    uint32_t function_id;    /*!< Used to identify the function in the
                              *   API dispatcher to the service backend
                              */
    uint16_t step;           /*!< Key derivation step */
    psa_key_id_t key_id;     /*!< Key id */
    psa_algorithm_t alg;     /*!< Algorithm */
    uint32_t op_handle;      /*!< Frontend context handle associated to a
                              *   multipart operation
                              */
    size_t capacity;         /*!< Key derivation capacity */
    size_t ad_length;        /*!< Additional Data length for multipart AEAD */
    size_t plaintext_length; /*!< Plaintext length for multipart AEAD */

    struct tfm_crypto_aead_pack_input aead_in; /*!< Packs AEAD-related inputs */
};

/**
 * \brief X macro describing each of the available PSA Crypto APIs in terms of
 *        group ID (\ref tfm_crypto_group_id) and multipart function type
 *        (\ref tfm_crypto_function_type)
 */
#define TFM_CRYPTO_SERVICE_API_DESCRIPTION          \
    X(TFM_CRYPTO_GET_KEY_ATTRIBUTES_SID,            \
        TFM_CRYPTO_GROUP_ID_KEY_MANAGEMENT,         \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_RESET_KEY_ATTRIBUTES_SID,          \
        TFM_CRYPTO_GROUP_ID_KEY_MANAGEMENT,         \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_OPEN_KEY_SID,                      \
        TFM_CRYPTO_GROUP_ID_KEY_MANAGEMENT,         \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_CLOSE_KEY_SID,                     \
        TFM_CRYPTO_GROUP_ID_KEY_MANAGEMENT,         \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_IMPORT_KEY_SID,                    \
        TFM_CRYPTO_GROUP_ID_KEY_MANAGEMENT,         \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_DESTROY_KEY_SID,                   \
        TFM_CRYPTO_GROUP_ID_KEY_MANAGEMENT,         \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_EXPORT_KEY_SID,                    \
        TFM_CRYPTO_GROUP_ID_KEY_MANAGEMENT,         \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_EXPORT_PUBLIC_KEY_SID,             \
        TFM_CRYPTO_GROUP_ID_KEY_MANAGEMENT,         \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_PURGE_KEY_SID,                     \
        TFM_CRYPTO_GROUP_ID_KEY_MANAGEMENT,         \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_COPY_KEY_SID,                      \
        TFM_CRYPTO_GROUP_ID_KEY_MANAGEMENT,         \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_GENERATE_KEY_SID,                  \
        TFM_CRYPTO_GROUP_ID_KEY_MANAGEMENT,         \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_HASH_COMPUTE_SID,                  \
        TFM_CRYPTO_GROUP_ID_HASH,                   \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_HASH_COMPARE_SID,                  \
        TFM_CRYPTO_GROUP_ID_HASH,                   \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_HASH_SETUP_SID,                    \
        TFM_CRYPTO_GROUP_ID_HASH,                   \
        TFM_CRYPTO_FUNCTION_TYPE_SETUP)             \
    X(TFM_CRYPTO_HASH_UPDATE_SID,                   \
        TFM_CRYPTO_GROUP_ID_HASH,                   \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_HASH_CLONE_SID,                    \
        TFM_CRYPTO_GROUP_ID_HASH,                   \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_HASH_FINISH_SID,                   \
        TFM_CRYPTO_GROUP_ID_HASH,                   \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_HASH_VERIFY_SID,                   \
        TFM_CRYPTO_GROUP_ID_HASH,                   \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_HASH_ABORT_SID,                    \
        TFM_CRYPTO_GROUP_ID_HASH,                   \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_MAC_COMPUTE_SID,                   \
        TFM_CRYPTO_GROUP_ID_MAC,                    \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_MAC_VERIFY_SID,                    \
        TFM_CRYPTO_GROUP_ID_MAC,                    \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_MAC_SIGN_SETUP_SID,                \
        TFM_CRYPTO_GROUP_ID_MAC,                    \
        TFM_CRYPTO_FUNCTION_TYPE_SETUP)             \
    X(TFM_CRYPTO_MAC_VERIFY_SETUP_SID,              \
        TFM_CRYPTO_GROUP_ID_MAC,                    \
        TFM_CRYPTO_FUNCTION_TYPE_SETUP)             \
    X(TFM_CRYPTO_MAC_UPDATE_SID,                    \
        TFM_CRYPTO_GROUP_ID_MAC,                    \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_MAC_SIGN_FINISH_SID,               \
        TFM_CRYPTO_GROUP_ID_MAC,                    \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_MAC_VERIFY_FINISH_SID,             \
        TFM_CRYPTO_GROUP_ID_MAC,                    \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_MAC_ABORT_SID,                     \
        TFM_CRYPTO_GROUP_ID_MAC,                    \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_CIPHER_ENCRYPT_SID,                \
        TFM_CRYPTO_GROUP_ID_CIPHER,                 \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_CIPHER_DECRYPT_SID,                \
        TFM_CRYPTO_GROUP_ID_CIPHER,                 \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_CIPHER_ENCRYPT_SETUP_SID,          \
        TFM_CRYPTO_GROUP_ID_CIPHER,                 \
        TFM_CRYPTO_FUNCTION_TYPE_SETUP)             \
    X(TFM_CRYPTO_CIPHER_DECRYPT_SETUP_SID,          \
        TFM_CRYPTO_GROUP_ID_CIPHER,                 \
        TFM_CRYPTO_FUNCTION_TYPE_SETUP)             \
    X(TFM_CRYPTO_CIPHER_GENERATE_IV_SID,            \
        TFM_CRYPTO_GROUP_ID_CIPHER,                 \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_CIPHER_SET_IV_SID,                 \
        TFM_CRYPTO_GROUP_ID_CIPHER,                 \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_CIPHER_UPDATE_SID,                 \
        TFM_CRYPTO_GROUP_ID_CIPHER,                 \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_CIPHER_FINISH_SID,                 \
        TFM_CRYPTO_GROUP_ID_CIPHER,                 \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_CIPHER_ABORT_SID,                  \
        TFM_CRYPTO_GROUP_ID_CIPHER,                 \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_AEAD_ENCRYPT_SID,                  \
        TFM_CRYPTO_GROUP_ID_AEAD,                   \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_AEAD_DECRYPT_SID,                  \
        TFM_CRYPTO_GROUP_ID_AEAD,                   \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_AEAD_ENCRYPT_SETUP_SID,            \
        TFM_CRYPTO_GROUP_ID_AEAD,                   \
        TFM_CRYPTO_FUNCTION_TYPE_SETUP)             \
    X(TFM_CRYPTO_AEAD_DECRYPT_SETUP_SID,            \
        TFM_CRYPTO_GROUP_ID_AEAD,                   \
        TFM_CRYPTO_FUNCTION_TYPE_SETUP)             \
    X(TFM_CRYPTO_AEAD_GENERATE_NONCE_SID,           \
        TFM_CRYPTO_GROUP_ID_AEAD,                   \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_AEAD_SET_NONCE_SID,                \
        TFM_CRYPTO_GROUP_ID_AEAD,                   \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_AEAD_SET_LENGTHS_SID,              \
        TFM_CRYPTO_GROUP_ID_AEAD,                   \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_AEAD_UPDATE_AD_SID,                \
        TFM_CRYPTO_GROUP_ID_AEAD,                   \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_AEAD_UPDATE_SID,                   \
        TFM_CRYPTO_GROUP_ID_AEAD,                   \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_AEAD_FINISH_SID,                   \
        TFM_CRYPTO_GROUP_ID_AEAD,                   \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_AEAD_VERIFY_SID,                   \
        TFM_CRYPTO_GROUP_ID_AEAD,                   \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_AEAD_ABORT_SID,                    \
        TFM_CRYPTO_GROUP_ID_AEAD,                   \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_ASYMMETRIC_SIGN_MESSAGE_SID,       \
        TFM_CRYPTO_GROUP_ID_ASYM_SIGN,              \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_ASYMMETRIC_VERIFY_MESSAGE_SID,     \
        TFM_CRYPTO_GROUP_ID_ASYM_SIGN,              \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_ASYMMETRIC_SIGN_HASH_SID,          \
        TFM_CRYPTO_GROUP_ID_ASYM_SIGN,              \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_ASYMMETRIC_VERIFY_HASH_SID,        \
        TFM_CRYPTO_GROUP_ID_ASYM_SIGN,              \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_ASYMMETRIC_ENCRYPT_SID,            \
        TFM_CRYPTO_GROUP_ID_ASYM_ENCRYPT,           \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_ASYMMETRIC_DECRYPT_SID,            \
        TFM_CRYPTO_GROUP_ID_ASYM_ENCRYPT,           \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_RAW_KEY_AGREEMENT_SID,             \
        TFM_CRYPTO_GROUP_ID_KEY_DERIVATION,         \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_KEY_DERIVATION_SETUP_SID,          \
        TFM_CRYPTO_GROUP_ID_KEY_DERIVATION,         \
        TFM_CRYPTO_FUNCTION_TYPE_SETUP)             \
    X(TFM_CRYPTO_KEY_DERIVATION_GET_CAPACITY_SID,   \
        TFM_CRYPTO_GROUP_ID_KEY_DERIVATION,         \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_KEY_DERIVATION_SET_CAPACITY_SID,   \
        TFM_CRYPTO_GROUP_ID_KEY_DERIVATION,         \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_KEY_DERIVATION_INPUT_BYTES_SID,    \
        TFM_CRYPTO_GROUP_ID_KEY_DERIVATION,         \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_KEY_DERIVATION_INPUT_KEY_SID,      \
        TFM_CRYPTO_GROUP_ID_KEY_DERIVATION,         \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_KEY_DERIVATION_KEY_AGREEMENT_SID,  \
        TFM_CRYPTO_GROUP_ID_KEY_DERIVATION,         \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_KEY_DERIVATION_OUTPUT_BYTES_SID,   \
        TFM_CRYPTO_GROUP_ID_KEY_DERIVATION,         \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_KEY_DERIVATION_OUTPUT_KEY_SID,     \
        TFM_CRYPTO_GROUP_ID_KEY_DERIVATION,         \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_KEY_DERIVATION_ABORT_SID,          \
        TFM_CRYPTO_GROUP_ID_KEY_DERIVATION,         \
        TFM_CRYPTO_FUNCTION_TYPE_LOOKUP)            \
    X(TFM_CRYPTO_GENERATE_RANDOM_SID,               \
        TFM_CRYPTO_GROUP_ID_RANDOM,                 \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
    X(TFM_CRYPTO_API_DISPATCHER_SID,                \
        TFM_CRYPTO_GROUP_ID_RANDOM,                 \
        TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART)     \
/**
 * \brief Numerical progressive value identifying a function API exposed through
 *        the interfaces (S or NS). It's used to dispatch the requests from S/NS
 *        to the corresponding API implementation in the Crypto service backend.
 */
#define X(function_id, group_id, function_type) function_id,
enum tfm_crypto_function_id {
    TFM_CRYPTO_SERVICE_API_DESCRIPTION
};
#undef X

/**
 * \brief Define an invalid value for an SID
 *
 */
#define TFM_CRYPTO_SID_INVALID (~0x0u)

/**
 * \brief This value is used to mark an handle as invalid.
 *
 */
#define TFM_CRYPTO_INVALID_HANDLE (0x0u)

/**
 * \brief The persistent key identifier that refers to the hardware unique key.
 *
 */
#define TFM_CRYPTO_KEY_ID_HUK (0xFFFF815Bu)

/**
 * \brief The algorithm identifier that refers to key derivation from the
 *        hardware unique key.
 *
 */
#define TFM_CRYPTO_ALG_HUK_DERIVATION ((psa_algorithm_t)0xB0000F00)

/**
 * \brief Define miscellaneous literal constants that are used in the service
 *
 */
enum {
    TFM_CRYPTO_NOT_IN_USE = 0,
    TFM_CRYPTO_IN_USE = 1
};

#ifdef __cplusplus
}
#endif

#endif /* __TFM_CRYPTO_DEFS_H__ */
