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
#include "tfm_crypto_defs.h"
#include "psa_crypto.h"

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
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_init(void);

/**
 * \brief Initialise the Key module
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_init_key(void);

/**
 * \brief Initialise the Alloc module
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_init_alloc(void);

/**
 * \brief Allocate an operation context in the backend
 *
 * \param[in]  type Type of the operation context to allocate
 * \param[out] oper Pointer to the frontend operation
 * \param[out  ctx  Double pointer to the corresponding context
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_operation_alloc(
                                        enum tfm_crypto_operation_type type,
                                        void *oper,
                                        void **ctx);
/**
 * \brief Release an operation context in the backend
 *
 * \param[in]     type Type of the operation context to release
 * \param[in/out] oper Pointer to the frontend operation for the release
 *                     of the corresponding backend context
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_operation_release(
                                        enum tfm_crypto_operation_type type,
                                        void *oper);
/**
 * \brief Look up an operation context in the backend for the corresponding
 *        frontend operation
 *
 * \param[in]  type Type of the operation context to look up
 * \param[in]  oper Pointer to the frontend operation
 * \param[out] ctx  Double pointer to the corresponding context
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_operation_lookup(
                                        enum tfm_crypto_operation_type type,
                                        void *oper,
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
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_get_key(psa_key_slot_t key,
                                         psa_key_usage_t usage,
                                         psa_algorithm_t alg,
                                         uint8_t *data,
                                         size_t data_size,
                                         size_t *data_length);
/**
 * \brief Import the key data in the provided key slot
 *
 * \param[in] key         Key slot
 * \param[in] type        Key type
 * \param[in] data        Key data to import
 * \param[in] data_length Length in bytes of the data field
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_import_key(psa_key_slot_t key,
                                            psa_key_type_t type,
                                            const uint8_t *data,
                                            size_t data_length);
/**
 * \brief Destroy the key in the provided key slot
 *
 * \param[in] key         Key slot
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_destroy_key(psa_key_slot_t key);

/**
 * \brief Retrieve key information for the provided key slot
 *
 * \param[in]  key  Key slot
 * \param[out] type Key type associated to the key slot requested
 * \param[out] bits Length in bits of the key in the requested slot
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_get_key_information(psa_key_slot_t key,
                                                     psa_key_type_t *type,
                                                     size_t *bits);
/**
 * \brief Export the key contained in the provided key slot
 *
 * \param[in]  key         Key slot
 * \param[out] data        Buffer to hold the exported key
 * \param[in]  data_size   Length of the buffer pointed to by data
 * \param[out] data_length Length of the exported key
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_export_key(psa_key_slot_t key,
                                            uint8_t *data,
                                            size_t data_size,
                                            size_t *data_length);

/**
 * \brief Initialise the key policy to a default that forbids any use of the
 *        key
 *
 * \param[out] policy  Key policy to initialise
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_key_policy_init(psa_key_policy_t *policy);

/**
 * \brief Set the permitted usage and algorithm for the provided key policy
 *
 * \param[out] policy  Key policy to modify
 * \param[in]  usage   Permitted usage
 * \param[in]  alg     Permitted algorithm
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_key_policy_set_usage(psa_key_policy_t *policy,
                                                      psa_key_usage_t usage,
                                                      psa_algorithm_t alg);

/**
 * \brief Get the permitted usage for the provided key policy
 *
 * \param[in]  policy  Key policy
 * \param[out] usage   Permitted usage for this key policy
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_key_policy_get_usage(
                                                 const psa_key_policy_t *policy,
                                                 psa_key_usage_t *usage);

/**
 * \brief Get the permitted algorithm for the provided key policy
 *
 * \param[in]  policy  Key policy
 * \param[out] alg     Permitted algorithm for this key policy
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_key_policy_get_algorithm(
                                                 const psa_key_policy_t *policy,
                                                 psa_algorithm_t *alg);

/**
 * \brief Set the key policy for the provided key slot
 *
 * \param[in] key     Key slot
 * \param[in] policy  Key policy
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_set_key_policy(psa_key_slot_t key,
                                                const psa_key_policy_t *policy);

/**
 * \brief Get the key policy for the provided key slot
 *
 * \param[in]  key     Key slot
 * \param[out] policy  Key policy
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_get_key_policy(psa_key_slot_t key,
                                                psa_key_policy_t *policy);

/**
 * \brief Set the lifetime for the provided key slot
 *
 * \param[in] key       Key slot
 * \param[in] lifetime  Lifetime value
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_set_key_lifetime(psa_key_slot_t key,
                                                  psa_key_lifetime_t lifetime);

/**
 * \brief Get the lifetime for the provided key slot
 *
 * \param[in]  key       Key slot
 * \param[out] lifetime  Lifetime value
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_get_key_lifetime(psa_key_slot_t key,
                                                  psa_key_lifetime_t *lifetime);

/**
 * \brief Export the public key contained in the provided key slot
 *        for an asymmetric key pair
 *
 * \param[in]  key         Key slot
 * \param[out] data        Buffer to hold the exported key
 * \param[in]  data_size   Length of the buffer pointed to by data
 * \param[out] data_length Length of the exported key
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_export_public_key(psa_key_slot_t key,
                                                   uint8_t *data,
                                                   size_t data_size,
                                                   size_t *data_length);
/**
 * \brief Set the initialisation vector on the provided cipher operation
 *
 * \param[in] operation  Cipher operation context
 * \param[in] iv         Buffer that contains the IV
 * \param[in] iv_length  Length of the provided IV
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_cipher_set_iv(
                                              psa_cipher_operation_t *operation,
                                              const unsigned char *iv,
                                              size_t iv_length);
/**
 * \brief Set the cipher operation using the provided algorithm and key slot,
 *        for encryption context
 *
 * \note A successful call to this function initialises a cipher operation
 *       context which will be referred using the operation parameter
 *
 * \param[out] operation Cipher operation context
 * \param[in]  key       Key slot to bind to the cipher context
 * \param[in]  alg       Algorithm to use for the cipher operation
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_cipher_encrypt_setup(
                                              psa_cipher_operation_t *operation,
                                              psa_key_slot_t key,
                                              psa_algorithm_t alg);
/**
 * \brief Set the cipher operation using the provided algorithm and key slot,
 *        for decryption context
 *
 * \note A successful call to this function initialises a cipher operation
 *       context which will be referred using the operation parameter
 *
 * \param[out] operation Cipher operation context
 * \param[in]  key       Key slot to bind to the cipher context
 * \param[in]  alg       Algorithm to use for the cipher operation
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_cipher_decrypt_setup(
                                              psa_cipher_operation_t *operation,
                                              psa_key_slot_t key,
                                              psa_algorithm_t alg);
/**
 * \brief Update the cipher context with a chunk of input data to create a
 *        chunk of encrypted output data (for encryption contexts), or to
 *        decrypt a chunk of encrypted input data to obtain decrypted data
 *        (for decryption contexts)
 *
 * \param[in,out] operation     Cipher operation context
 * \param[in]     input         Buffer containing input data
 * \param[in]     input_length  Input length
 * \param[out]    output        Buffer containing output data
 * \param[in]     output_size   Size of the output buffer
 * \param[out]    output_length Size of the produced output
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_cipher_update(
                                              psa_cipher_operation_t *operation,
                                              const uint8_t *input,
                                              size_t input_length,
                                              unsigned char *output,
                                              size_t output_size,
                                              size_t *output_length);
/**
 * \brief Finalise a cipher context flushing out any remaining block of
 *        output data
 *
 * \note A successful call to this function de-initialises the cipher operation
 *       context provided as parameter
 *
 * \param[in,out] operation     Cipher operation context
 * \param[out]    output        Buffer containing output data
 * \param[in]     output_size   Size of the output buffer
 * \param[out]    output_length Size of the produced output
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_cipher_finish(
                                              psa_cipher_operation_t *operation,
                                              uint8_t *output,
                                              size_t output_size,
                                              size_t *output_length);
/**
 * \brief Abort a cipher operation, clears the operation context provided
 *
 * \note A successful call to this function de-initialises the cipher operation
 *       context provided as parameter
 *
 * \param[in,out] operation Cipher operation context
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_cipher_abort(
                                             psa_cipher_operation_t *operation);
/**
 * \brief Start a hash operation with the provided algorithm
 *
 * \note A successful call to this function initialises a hash operation
 *       context which will be referred using the operation parameter
 *
 * \param[out] operation Hash operation context
 * \param[in]  alg       Algorithm chosen as hash
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_hash_setup(psa_hash_operation_t *operation,
                                            psa_algorithm_t alg);
/**
 * \brief Add a new input chunk to the data for which the final hash value
 *        will be computed
 *
 * \param[in] operation    Hash operation context
 * \param[in] input        Buffer containing the input data
 * \param[in] input_length Size of the provided input data
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_hash_update(psa_hash_operation_t *operation,
                                             const uint8_t *input,
                                             size_t input_length);
/**
 * \brief Finalise a hash context operation producing the final hash value
 *
 * \note A successful call to this function de-initialises the hash operation
 *       context provided as parameter
 *
 * \param[in,out] operation   Hash operation context
 * \param[out]    hash        Buffer containing hash data
 * \param[in]     hash_size   Size of the hash buffer
 * \param[out]    hash_length Size of the produced hash
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_hash_finish(psa_hash_operation_t *operation,
                                             uint8_t *hash,
                                             size_t hash_size,
                                             size_t *hash_length);
/**
 * \brief Finalise a hash context operation, verifying that the final hash
 *        value matches the one provided as input
 *
 * \note A successful call to this function de-initialises the hash operation
 *       context provided as parameter. The hash operation is de-initialised
 *       also in case TFM_CRYPTO_ERR_PSA_ERROR_INVALID_SIGNATURE is returned
 *
 * \param[in,out] operation   Hash operation context
 * \param[in]     hash        Buffer containing the provided hash value
 * \param[in]     hash_length Size of the provided hash value
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_hash_verify(psa_hash_operation_t *operation,
                                             const uint8_t *hash,
                                             size_t hash_length);
/**
 * \brief Abort a hash operation, clears the operation context provided
 *
 * \note A successful call to this function de-initialises the hash operation
 *       context provided as parameter
 *
 * \param[in,out] operation Hash operation context
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_hash_abort(psa_hash_operation_t *operation);

/**
 * \brief Start a MAC operation with the provided algorithm (for signing)
 *
 * \note A successful call to this function initialises a MAC operation
 *       context which will be referred using the operation parameter
 *
 * \param[out] operation MAC operation context
 * \param[in]  key       Key slot to bind to the MAC context
 * \param[in]  alg       Algorithm chosen as MAC
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_mac_sign_setup(psa_mac_operation_t *operation,
                                                psa_key_slot_t key,
                                                psa_algorithm_t alg);
/**
 * \brief Start a MAC operation with the provided algorithm (for verifying)
 *
 * \note A successful call to this function initialises a MAC operation
 *       context which will be referred using the operation parameter
 *
 * \param[out] operation MAC operation context
 * \param[in]  key       Key slot to bind to the MAC context
 * \param[in]  alg       Algorithm chosen as MAC
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_mac_verify_setup(
                                                psa_mac_operation_t *operation,
                                                psa_key_slot_t key,
                                                psa_algorithm_t alg);
/**
 * \brief Adds a new input chunk to the data for which the final MAC value
 *        will be computed
 *
 * \param[in] operation    MAC operation context
 * \param[in] input        Buffer containing the input data
 * \param[in] input_length Size of the provided input data
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_mac_update(psa_mac_operation_t *operation,
                                            const uint8_t *input,
                                            size_t input_length);
/**
 * \brief Finalise a MAC context operation producing the final MAC value
 *
 * \param[in,out] operation  Mac operation context
 * \param[out]    mac        Buffer containing MAC data
 * \param[in]     mac_size   Size of the mac buffer
 * \param[out]    mac_length Size of the produced mac
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_mac_sign_finish(psa_mac_operation_t *operation,
                                                 uint8_t *mac,
                                                 size_t mac_size,
                                                 size_t *mac_length);
/**
 * \brief Finalise a MAC context operation, verifying that the final MAC value
 *        matches the one provided as input
 *
 * \param[in,out] operation  MAC operation context
 * \param[in]     mac        Buffer containing the provided MAC value
 * \param[in]     mac_length Size of the provided MAC value
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_mac_verify_finish(
                                                 psa_mac_operation_t *operation,
                                                 const uint8_t *mac,
                                                 size_t mac_length);
/**
 * \brief Abort a MAC operation, clear the operation context provided
 *
 * \param[in,out] operation MAC operation context
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_mac_abort(psa_mac_operation_t *operation);

/**
 * \brief Perform an AEAD encryption operation on input data with additional
 *        data to be authenticated, producing ciphertext in output with an
 *        appended authentication tag
 *
 * \param[in]  key                    Key slot for the key
 * \param[in]  alg                    Algorithm to be used
 * \param[in]  nonce                  Pointer to a buffer holding a nonce or IV
 *                                    to use
 * \param[in]  nonce_length           Size in bytes of the nonce or IV data
 * \param[in]  additional_data        Additional information to be authenticated
 * \param[in]  additional_data_length Size in bytes of the additional data
 * \param[in]  plaintext              Buffer pointing to data to be encrypted
 * \param[in]  plaintext_length       Size in bytes of the plain text buffer
 * \param[out] ciphertext             Output encrypted data, with the
 *                                    authentication tag appended
 * \param[in]  ciphertext_size        Size in bytes of the buffer to hold the
 *                                    cipher text plus authentication tag
 * \param[out] ciphertext_length      Size of the ciphertext plus tag produced
 *                                    as output
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_aead_encrypt(psa_key_slot_t key,
                                              psa_algorithm_t alg,
                                              const uint8_t *nonce,
                                              size_t nonce_length,
                                              const uint8_t *additional_data,
                                              size_t additional_data_length,
                                              const uint8_t *plaintext,
                                              size_t plaintext_length,
                                              uint8_t *ciphertext,
                                              size_t ciphertext_size,
                                              size_t *ciphertext_length);
/**
 * \brief Perform an AEAD decryption operation on input data with additional
 *        data to be verified, producing back the original plain text in case
 *        the verification of the authentication tag is successful
 *
 * \param[in]  key                    Key slot for the key
 * \param[in]  alg                    Algorithm to be used
 * \param[in]  nonce                  Pointer to a buffer holding a nonce or IV
 *                                    to use
 * \param[in]  nonce_length           Size in bytes of the nonce or IV data
 * \param[in]  additional_data        Additional information which was
 *                                    authenticated but not encrypted
 * \param[in]  additional_data_length Size in bytes of the additional data
 * \param[in]  ciphertext             Buffer pointing to data be decrypted
 * \param[in]  ciphertext_length      Size in bytes of the cipher text buffer
 * \param[out] plaintext              Buffer for decrypted output data
 * \param[in]  plaintext_size         Size in bytes of the buffer to hold the
 *                                    plain text
 * \param[out] plaintext_length       Size of the plain text actually produced
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_aead_decrypt(psa_key_slot_t key,
                                              psa_algorithm_t alg,
                                              const uint8_t *nonce,
                                              size_t nonce_length,
                                              const uint8_t *additional_data,
                                              size_t additional_data_length,
                                              const uint8_t *ciphertext,
                                              size_t ciphertext_length,
                                              uint8_t *plaintext,
                                              size_t plaintext_size,
                                              size_t *plaintext_length);
#ifdef __cplusplus
}
#endif

#endif /* __TFM_CRYPTO_API_H__ */
