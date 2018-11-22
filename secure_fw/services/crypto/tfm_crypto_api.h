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
    TFM_CRYPTO_KEY_POLICY = 4,

    /* Used to force the enum size */
    TFM_CRYPTO_OPERATION_TYPE_MAX = INT_MAX
};

/**
 * \brief Initialises the service
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_init(void);

/**
 * \brief Allocates an operation object
 *
 * \param[in]  type   Type of the operation object to allocate
 * \param[out] handle Pointer to the corresponding handle assigned
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_operation_alloc(
                                        enum tfm_crypto_operation_type type,
                                        uint32_t *handle);
/**
 * \brief Releases an operation object
 *
 * \param[in] handle Pointer to the handle for the release of the
 *                   corresponding object
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_operation_release(uint32_t *handle);

/**
 * \brief Looks up an operation object pointer from the corresponding handle
 *
 * \param[in]  type   Type of the operation object to look up
 * \param[in]  handle Handle to the operation object to look up
 * \param[out] oper   Double pointer to the corresponding object
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_operation_lookup(
                                        enum tfm_crypto_operation_type type,
                                        uint32_t handle,
                                        void **oper);
/**
 * \brief Import the key data in the provided key slot
 *
 * \param[in] key         Key slot
 * \param[in] type        Key type
 * \param[in] data        Key data to import
 * \param[in] data_length Length in bytes of the data field
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_import_key(psa_key_slot_t key,
                                            psa_key_type_t type,
                                            const uint8_t *data,
                                            size_t data_length);
/**
 * \brief Destroy the key on the provided key slot
 *
 * \param[in] key         Key slot
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_destroy_key(psa_key_slot_t key);

/**
 * \brief Retrieve key information for the provided key slot
 *
 * \param[in]  key  Key slot
 * \param[out] type Key type associated to the key slot requested
 * \param[out] bits Length in bits of the key in the requested slot
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
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
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_export_key(psa_key_slot_t key,
                                            uint8_t *data,
                                            size_t data_size,
                                            size_t *data_length);
/**
 * \brief Export the public key contained in the provided key slot
 *        for an asymmetric key pair.
 *
 * \param[in]  key         Key slot
 * \param[out] data        Buffer to hold the exported key
 * \param[in]  data_size   Length of the buffer pointed to by data
 * \param[out] data_length Length of the exported key
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
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
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_cipher_set_iv(
                                              psa_cipher_operation_t *operation,
                                              const unsigned char *iv,
                                              size_t iv_length);
/**
 * \brief Set the cipher operation using the provided algorithm and key slot,
 *        for encryption context.
 *
 * \param[in] operation Cipher operation context
 * \param[in] key       Key slot to bind to the cipher context
 * \param[in] alg       Algorithm to use for the cipher operation
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_cipher_encrypt_setup(
                                              psa_cipher_operation_t *operation,
                                              psa_key_slot_t key,
                                              psa_algorithm_t alg);
/**
 * \brief Set the cipher operation using the provided algorithm and key slot,
 *        for decryption context.
 *
 * \param[in] operation Cipher operation context
 * \param[in] key       Key slot to bind to the cipher context
 * \param[in] alg       Algorithm to use for the cipher operation
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_cipher_decrypt_setup(
                                              psa_cipher_operation_t *operation,
                                              psa_key_slot_t key,
                                              psa_algorithm_t alg);
/**
 * \brief Update the cipher context with a chunk of input data to create a
 *        chunk of encrypted output data (for encryption contexts), or to
 *        decrypt a chunk of encrypted input data to obtain decrypted data
 *        (for decryption contexts).
 *
 * \param[in]  operation     Cipher operation context
 * \param[in]  input         Buffer containing input data
 * \param[in]  input_length  Input length
 * \param[out] output        Buffer containing output data
 * \param[in]  output_size   Size of the output buffer
 * \param[out] output_length Size of the produced output
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_cipher_update(
                                              psa_cipher_operation_t *operation,
                                              const uint8_t *input,
                                              size_t input_length,
                                              unsigned char *output,
                                              size_t output_size,
                                              size_t *output_length);
/**
 * \brief Finalises a cipher context flushing out any remaining block of
 *        output data
 *
 * \param[in]  operation     Cipher operation context
 * \param[out] output        Buffer containing output data
 * \param[in]  output_size   Size of the output buffer
 * \param[out] output_length Size of the produced output
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_cipher_finish(
                                              psa_cipher_operation_t *operation,
                                              uint8_t *output,
                                              size_t output_size,
                                              size_t *output_length);
/**
 * \brief Aborts a cipher operation, clears the operation context provided
 *
 * \param[in] operation Cipher operation context
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_cipher_abort(
                                             psa_cipher_operation_t *operation);
/**
 * \brief Starts a hash operation with the provided algorithm
 *
 * \param[in] operation Hash operation context
 * \param[in] alg       Algorithm chosen as hash
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_hash_setup(psa_hash_operation_t *operation,
                                            psa_algorithm_t alg);
/**
 * \brief Adds a new input chunk to the data for which the final hash value
 *        will be computed
 *
 * \param[in] operation    Hash operation context
 * \param[in] input        Buffer containing the input data
 * \param[in] input_length Size of the provided input data
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_hash_update(psa_hash_operation_t *operation,
                                             const uint8_t *input,
                                             size_t input_length);
/**
 * \brief Finalises a hash context operation producing the final hash value
 *
 * \param[in]  operation   Hash operation context
 * \param[out] hash        Buffer containing hash data
 * \param[in]  hash_size   Size of the hash buffer
 * \param[out] hash_length Size of the produced hash
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_hash_finish(psa_hash_operation_t *operation,
                                             uint8_t *hash,
                                             size_t hash_size,
                                             size_t *hash_length);
/**
 * \brief Finalises a hash context operation, verifying that the final hash
 *        value matches the one provided as input
 *
 * \param[in] operation   Hash operation context
 * \param[in] hash        Buffer containing the provided hash value
 * \param[in] hash_length Size of the provided hash value
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_hash_verify(psa_hash_operation_t *operation,
                                             const uint8_t *hash,
                                             size_t hash_length);
/**
 * \brief Aborts a hash operation, clears the operation context provided
 *
 * \param[in] operation Hash operation context
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_hash_abort(psa_hash_operation_t *operation);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_CRYPTO_API_H__ */
