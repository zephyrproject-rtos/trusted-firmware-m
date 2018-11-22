/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <limits.h>

#include "tfm_crypto_defs.h"

/* Pre include Mbed TLS headers */
#define LIB_PREFIX_NAME __tfm_crypto__
#include "mbedtls_global_symbols.h"

/* Include the Mbed TLS configuration file, the way Mbed TLS does it
 * in each of its header files.
 */
#if !defined(MBEDTLS_CONFIG_FILE)
#include "platform/ext/common/tfm_mbedtls_config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "psa_crypto.h"

#include "tfm_crypto_struct.h"

#include "tfm_crypto_api.h"
#include "crypto_utils.h"

/**
 * \brief For a TFM_CRYPTO_CIPHER_OPERATION, define the possible
 *        modes of configuration.
 *
 */
enum tfm_crypto_cipher_mode_t {
   TFM_CRYPTO_CIPHER_MODE_DECRYPT = 0,
   TFM_CRYPTO_CIPHER_MODE_ENCRYPT = 1,
};

static enum tfm_crypto_err_t tfm_crypto_cipher_setup(
                                           psa_cipher_operation_t *operation,
                                           psa_key_slot_t key,
                                           psa_algorithm_t alg,
                                           enum tfm_crypto_cipher_mode_t c_mode)
{
    const mbedtls_cipher_info_t *info = NULL;
    psa_algorithm_t padding_mode = PSA_ALG_BLOCK_CIPHER_PAD_NONE;
    psa_key_type_t key_type;
    size_t key_size;
    enum tfm_crypto_err_t err;
    uint8_t key_data[TFM_CRYPTO_MAX_KEY_LENGTH];
    uint32_t ret;
    mbedtls_cipher_type_t type = MBEDTLS_CIPHER_NONE;
    mbedtls_cipher_padding_t mbedtls_padding_mode = MBEDTLS_PADDING_NONE;

    struct tfm_cipher_operation_s *ctx = NULL;

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_cipher_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (!PSA_ALG_IS_CIPHER(alg)) {
        return TFM_CRYPTO_ERR_PSA_ERROR_NOT_SUPPORTED;
    }

    /* FIXME: Check that key is compatible with alg */
    err = tfm_crypto_get_key_information(key, &key_type, &key_size);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    err = tfm_crypto_export_key(key, &key_data[0], TFM_CRYPTO_MAX_KEY_LENGTH,
                                                                     &key_size);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* Mbed TLS cipher setup */
    if (PSA_BYTES_TO_BITS(key_size) == 128) {
        if (alg == PSA_ALG_CBC_BASE) {
            type = MBEDTLS_CIPHER_AES_128_CBC;
        } else if (alg == PSA_ALG_CFB_BASE) {
            if (c_mode == TFM_CRYPTO_CIPHER_MODE_ENCRYPT) {
                type = MBEDTLS_CIPHER_AES_128_CFB128;
            }
        }
    }

    /* The requested alg/key/mode is not supported */
    if (type == MBEDTLS_CIPHER_NONE) {
        return TFM_CRYPTO_ERR_PSA_ERROR_NOT_SUPPORTED;
    }

    /* Allocate the operation context in the TFM space */
    err = tfm_crypto_operation_alloc(TFM_CRYPTO_CIPHER_OPERATION,
                                     &(operation->handle));
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* Look up the corresponding operation context */
    err = tfm_crypto_operation_lookup(TFM_CRYPTO_CIPHER_OPERATION,
                                      operation->handle,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* Bind the algorithm to the cipher operation */
    ctx->alg = alg;

    /* Mbed TLS cipher init */
    mbedtls_cipher_init(&(ctx->cipher));
    info = mbedtls_cipher_info_from_type(type);
    ret = mbedtls_cipher_setup(&(ctx->cipher), info);
    if (ret != 0) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return TFM_CRYPTO_ERR_PSA_ERROR_COMMUNICATION_FAILURE;
    }

    /* FIXME: Check based on the algorithm, if we need to have an IV */
    ctx->iv_required = 1;

    /* Bind the key to the cipher operation */
    ctx->key = key;
    ctx->key_set = 1;

    /* Mbed TLS cipher set key */
    if (c_mode == TFM_CRYPTO_CIPHER_MODE_ENCRYPT) {

        ret = mbedtls_cipher_setkey(&(ctx->cipher),
                                    &key_data[0],
                                    PSA_BYTES_TO_BITS(key_size),
                                    MBEDTLS_ENCRYPT);

    } else if (c_mode == TFM_CRYPTO_CIPHER_MODE_DECRYPT) {

        ret = mbedtls_cipher_setkey(&(ctx->cipher),
                                    &key_data[0],
                                    PSA_BYTES_TO_BITS(key_size),
                                    MBEDTLS_DECRYPT);
    } else {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (ret != 0) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return TFM_CRYPTO_ERR_PSA_ERROR_COMMUNICATION_FAILURE;
    }

    /* Mbed TLS cipher set padding mode in case of CBC */
    if ((alg & ~PSA_ALG_BLOCK_CIPHER_PADDING_MASK) == PSA_ALG_CBC_BASE) {

        /* Check the value of padding field */
        padding_mode = alg & PSA_ALG_BLOCK_CIPHER_PADDING_MASK;

        switch (padding_mode) {
        case PSA_ALG_BLOCK_CIPHER_PAD_PKCS7:
            mbedtls_padding_mode = MBEDTLS_PADDING_PKCS7;
            break;
        case PSA_ALG_BLOCK_CIPHER_PAD_NONE:
            mbedtls_padding_mode = MBEDTLS_PADDING_NONE;
            break;
        default:
            return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
        }

        ret = mbedtls_cipher_set_padding_mode(&(ctx->cipher),
                                              mbedtls_padding_mode);
        if (ret != 0) {
            /* Release the operation context */
            tfm_crypto_operation_release(&(operation->handle));
            return TFM_CRYPTO_ERR_PSA_ERROR_COMMUNICATION_FAILURE;
        }
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

/*!
 * \defgroup public_psa Public functions, PSA
 *
 */

/*!@{*/
enum tfm_crypto_err_t tfm_crypto_cipher_set_iv(
                                              psa_cipher_operation_t *operation,
                                              const unsigned char *iv,
                                              size_t iv_length)
{
    int ret;
    enum tfm_crypto_err_t err;
    struct tfm_cipher_operation_s *ctx = NULL;

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_cipher_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }
    err = tfm_crypto_memory_check((void *)iv, iv_length, TFM_MEMORY_ACCESS_RO);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Look up the corresponding operation context */
    err = tfm_crypto_operation_lookup(TFM_CRYPTO_CIPHER_OPERATION,
                                      operation->handle,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    if (ctx->iv_required == 0) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return TFM_CRYPTO_ERR_PSA_ERROR_NOT_PERMITTED;
    }

    if (iv_length > PSA_CIPHER_IV_MAX_SIZE) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return TFM_CRYPTO_ERR_PSA_ERROR_NOT_SUPPORTED;
    }

    /* Bind the IV to the cipher operation */
    ret = mbedtls_cipher_set_iv(&(ctx->cipher), iv, iv_length);
    if (ret != 0) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return TFM_CRYPTO_ERR_PSA_ERROR_COMMUNICATION_FAILURE;
    }
    ctx->iv_set = 1;
    ctx->iv_size = iv_length;

    /* Reset the context after IV is set */
    ret = mbedtls_cipher_reset(&(ctx->cipher));
    if (ret != 0) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return TFM_CRYPTO_ERR_PSA_ERROR_COMMUNICATION_FAILURE;
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_cipher_encrypt_setup(
                                              psa_cipher_operation_t *operation,
                                              psa_key_slot_t key,
                                              psa_algorithm_t alg)
{
    return tfm_crypto_cipher_setup(operation,
                                   key,
                                   alg,
                                   TFM_CRYPTO_CIPHER_MODE_ENCRYPT);
}

enum tfm_crypto_err_t tfm_crypto_cipher_decrypt_setup(
                                              psa_cipher_operation_t *operation,
                                              psa_key_slot_t key,
                                              psa_algorithm_t alg)
{
    return tfm_crypto_cipher_setup(operation,
                                   key,
                                   alg,
                                   TFM_CRYPTO_CIPHER_MODE_DECRYPT);
}

enum tfm_crypto_err_t tfm_crypto_cipher_update(
                                              psa_cipher_operation_t *operation,
                                              const uint8_t *input,
                                              size_t input_length,
                                              unsigned char *output,
                                              size_t output_size,
                                              size_t *output_length)
{
    int ret;
    enum tfm_crypto_err_t err;
    size_t olen;
    struct tfm_cipher_operation_s *ctx = NULL;

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_cipher_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }
    err = tfm_crypto_memory_check((void *)input,
                                  input_length,
                                  TFM_MEMORY_ACCESS_RO);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }
    err = tfm_crypto_memory_check(output,
                                  output_size,
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }
    err = tfm_crypto_memory_check(output_length,
                                  sizeof(size_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Look up the corresponding operation context */
    err = tfm_crypto_operation_lookup(TFM_CRYPTO_CIPHER_OPERATION,
                                      operation->handle,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* If the IV is required and it's not been set yet */
    if ((ctx->iv_required == 1) && (ctx->iv_set == 0)) {

        if (ctx->cipher.operation != MBEDTLS_DECRYPT) {
            /* Release the operation context */
            tfm_crypto_operation_release(&(operation->handle));
            return TFM_CRYPTO_ERR_PSA_ERROR_BAD_STATE;
        }

        /* This call is used to set the IV on the object */
        err = tfm_crypto_cipher_set_iv(operation, input, input_length);

        *output_length = 0;

        return err;
    }

    /* If the key is not set, setup phase has not been completed */
    if (ctx->key_set == 0) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return TFM_CRYPTO_ERR_PSA_ERROR_BAD_STATE;
    }

    *output_length = 0;

    ret = mbedtls_cipher_update(&(ctx->cipher), input, input_length,
                                output, &olen);
    if ((ret != 0) || (olen == 0)) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return TFM_CRYPTO_ERR_PSA_ERROR_COMMUNICATION_FAILURE;
    }

    /* Assign the output buffer length */
    *output_length = olen;

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_cipher_finish(
                                              psa_cipher_operation_t *operation,
                                              uint8_t *output,
                                              size_t output_size,
                                              size_t *output_length)
{
    int ret;
    enum tfm_crypto_err_t err;
    size_t olen;
    struct tfm_cipher_operation_s *ctx = NULL;

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_cipher_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }
    err = tfm_crypto_memory_check(output,
                                  output_size,
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }
    err = tfm_crypto_memory_check(output_length,
                                  sizeof(size_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    *output_length = 0;

    /* Look up the corresponding operation context */
    err = tfm_crypto_operation_lookup(TFM_CRYPTO_CIPHER_OPERATION,
                                      operation->handle,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    ret = mbedtls_cipher_finish(&(ctx->cipher), output, &olen);
    if (ret != 0) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return TFM_CRYPTO_ERR_PSA_ERROR_COMMUNICATION_FAILURE;
    }

    *output_length = olen;

    /* Clear the Mbed TLS context */
    mbedtls_cipher_free(&(ctx->cipher));

    /* Release the operation context */
    err = tfm_crypto_operation_release(&(operation->handle));
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_cipher_abort(psa_cipher_operation_t *operation)
{
    enum tfm_crypto_err_t err;
    struct tfm_cipher_operation_s *ctx = NULL;

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_cipher_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Look up the corresponding operation context */
    err = tfm_crypto_operation_lookup(TFM_CRYPTO_CIPHER_OPERATION,
                                      operation->handle,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* Clear the Mbed TLS context */
    mbedtls_cipher_free(&(ctx->cipher));

    /* Release the operation context */
    err = tfm_crypto_operation_release(&(operation->handle));
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}
/*!@}*/
