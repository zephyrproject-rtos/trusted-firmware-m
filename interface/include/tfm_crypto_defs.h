/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
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
#include "psa_crypto.h"

/**
 * \brief This type is used to overcome a limitation in the number of maximum
 *        IOVECs that can be used especially in psa_aead_encrypt and
 *        psa_aead_decrypt. To be removed in case the AEAD APIs number of
 *        parameters passed gets restructured
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
    uint32_t sfn_id;             /*!< Secure function ID used to dispatch the
                                  *   request
                                  */
    psa_key_slot_t key;          /*!< Key slot */
    psa_key_type_t type;         /*!< Key type */
    psa_key_usage_t usage;       /*!< Usage policy for a key */
    psa_algorithm_t alg;         /*!< Algorithm */
    psa_key_lifetime_t lifetime; /*!< Lifetime policy for a key */
    uint32_t handle;             /*!< Frontend context handle associated to a
                                  *   multipart operation
                                  */

    struct tfm_crypto_aead_pack_input aead_in; /*!< FixMe: Temporarily used for
                                                *   AEAD until the API is
                                                *   restructured
                                                */
};

/**
 * \brief Define a numerical value for each SFID which can be used when
 *        dispatching the requests to the service
 */
#define TFM_CRYPTO_IMPORT_KEY_SFID               (0u)
#define TFM_CRYPTO_DESTROY_KEY_SFID              (1u)
#define TFM_CRYPTO_GET_KEY_INFORMATION_SFID      (2u)
#define TFM_CRYPTO_EXPORT_KEY_SFID               (3u)
#define TFM_CRYPTO_KEY_POLICY_INIT_SFID          (4u)
#define TFM_CRYPTO_KEY_POLICY_SET_USAGE_SFID     (5u)
#define TFM_CRYPTO_KEY_POLICY_GET_USAGE_SFID     (6u)
#define TFM_CRYPTO_KEY_POLICY_GET_ALGORITHM_SFID (7u)
#define TFM_CRYPTO_SET_KEY_POLICY_SFID           (8u)
#define TFM_CRYPTO_GET_KEY_POLICY_SFID           (9u)
#define TFM_CRYPTO_SET_KEY_LIFETIME_SFID         (10u)
#define TFM_CRYPTO_GET_KEY_LIFETIME_SFID         (11u)
#define TFM_CRYPTO_CIPHER_SET_IV_SFID            (12u)
#define TFM_CRYPTO_CIPHER_ENCRYPT_SETUP_SFID     (13u)
#define TFM_CRYPTO_CIPHER_DECRYPT_SETUP_SFID     (14u)
#define TFM_CRYPTO_CIPHER_UPDATE_SFID            (15u)
#define TFM_CRYPTO_CIPHER_ABORT_SFID             (16u)
#define TFM_CRYPTO_CIPHER_FINISH_SFID            (17u)
#define TFM_CRYPTO_HASH_SETUP_SFID               (18u)
#define TFM_CRYPTO_HASH_UPDATE_SFID              (19u)
#define TFM_CRYPTO_HASH_FINISH_SFID              (20u)
#define TFM_CRYPTO_HASH_VERIFY_SFID              (21u)
#define TFM_CRYPTO_HASH_ABORT_SFID               (22u)
#define TFM_CRYPTO_MAC_SIGN_SETUP_SFID           (23u)
#define TFM_CRYPTO_MAC_VERIFY_SETUP_SFID         (24u)
#define TFM_CRYPTO_MAC_UPDATE_SFID               (25u)
#define TFM_CRYPTO_MAC_SIGN_FINISH_SFID          (26u)
#define TFM_CRYPTO_MAC_VERIFY_FINISH_SFID        (27u)
#define TFM_CRYPTO_MAC_ABORT_SFID                (28u)
#define TFM_CRYPTO_AEAD_ENCRYPT_SFID             (29u)
#define TFM_CRYPTO_AEAD_DECRYPT_SFID             (30u)

/**
 * \brief Define the SID values and minor versions to match the ones defined in
 *        the service manifest files
 */
#define TFM_CRYPTO_SID     (0x00004000u)
#define TFM_CRYPTO_MIN_VER (0x0001)

/**
 * \brief Define the maximum value based on the previous list, and
 *        an invalid value
 */
#define TFM_CRYPTO_SFID_MAX (TFM_CRYPTO_AEAD_DECRYPT_SFID + 1u)
#define TFM_CRYPTO_SFID_INVALID (~0x0u)

/**
 * \brief This value is used to mark an handle as invalid.
 *
 */
#define TFM_CRYPTO_INVALID_HANDLE (~0x0u)

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
