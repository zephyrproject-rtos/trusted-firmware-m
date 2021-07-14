/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cc3xx_psa_cipher.h"
#include "cc_pal_mem.h"

/* FixMe: implement pkcs#7 padding in HW
 * The function below was taken from Mbed TLS and implements pkcs#7
 * padding in software. It might be possible to do this in HW using
 * CC; however, we will need to modify the SW abstraction function:
 * ProcessAesDrv, and others.
 */

static psa_status_t add_pkcs_padding(
        uint8_t *output,
        size_t output_size,
        size_t   data_size)
{
    if (NULL == output) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    uint8_t padding_size = output_size - data_size;

    for (size_t i = 0; i < padding_size; i++) {
        output[data_size + i] = padding_size;
    }

    return PSA_SUCCESS;
}

static psa_status_t cipher_setup(
        cc3xx_cipher_operation_t *operation,
        const psa_key_attributes_t *attributes,
        const uint8_t *key, size_t key_length,
        psa_algorithm_t alg,
        psa_encrypt_or_decrypt_t dir)
{
    psa_status_t ret = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_type_t key_type = attributes->core.type;
    size_t key_bits = attributes->core.bits;

    (void)key_length;

    if (!PSA_ALG_IS_CIPHER(alg)) {
        return PSA_ERROR_BAD_STATE;
    }

    CC_PalMemSetZero(operation, sizeof(cc3xx_cipher_operation_t));

    operation->alg        = alg;
    operation->key_type   = key_type;
    operation->dir        = dir;
    operation->iv_size    =  PSA_CIPHER_IV_LENGTH(key_type, alg);
    operation->block_size = (PSA_ALG_IS_STREAM_CIPHER(alg) ? 1 :
                             PSA_BLOCK_CIPHER_BLOCK_LENGTH(key_type));

    switch (operation->key_type) {
    case PSA_KEY_TYPE_AES:
        cc3xx_aes_init(&operation->aes_ctx);

        switch (operation->dir) {
        case PSA_CRYPTO_DRIVER_ENCRYPT:
            if (( ret = cc3xx_aes_setkey_enc(
                    &operation->aes_ctx,
                    key,
                    key_bits) )
                != PSA_SUCCESS) {
                return ret;
            }

            break;
        case PSA_CRYPTO_DRIVER_DECRYPT:
            if (( ret = cc3xx_aes_setkey_dec(
                    &operation->aes_ctx,
                    key,
                    key_bits) )
                != PSA_SUCCESS) {
                return ret;
            }

            break;
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }

        break;
    case PSA_KEY_TYPE_CHACHA20:
        cc3xx_chacha20_init(&operation->chacha_ctx);

        if (( ret = cc3xx_chacha20_setkey(
                &operation->chacha_ctx,
                key) )
            != PSA_SUCCESS) {
            return ret;
        }

        break;
    default:
        // Software fallback
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (operation->alg == PSA_ALG_CBC_PKCS7) {
        operation->add_padding = add_pkcs_padding;
    } else {
        operation->add_padding = NULL;
    }

    return ret;
}

psa_status_t cc3xx_cipher_encrypt_setup(
        cc3xx_cipher_operation_t *operation,
        const psa_key_attributes_t *attributes,
        const uint8_t *key, size_t key_length,
        psa_algorithm_t alg)
{
    return cipher_setup(operation,
                        attributes,
                        key, key_length,
                        alg,
                        PSA_CRYPTO_DRIVER_ENCRYPT);
}

psa_status_t cc3xx_cipher_decrypt_setup(
        cc3xx_cipher_operation_t *operation,
        const psa_key_attributes_t *attributes,
        const uint8_t *key, size_t key_length,
        psa_algorithm_t alg)
{
    return cipher_setup(operation,
                        attributes,
                        key, key_length,
                        alg,
                        PSA_CRYPTO_DRIVER_DECRYPT);
}

psa_status_t cc3xx_cipher_set_iv(
        cc3xx_cipher_operation_t *operation,
        const uint8_t *iv, size_t iv_length)
{
    psa_status_t ret = PSA_ERROR_CORRUPTION_DETECTED;

    if (iv_length > AES_IV_SIZE) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (iv_length != 0) {
        CC_PalMemCopy(operation->iv, iv, iv_length);
        operation->iv_size =  iv_length;
    }

    ret = PSA_SUCCESS;

    if (operation->key_type == PSA_KEY_TYPE_CHACHA20) {
        if (( ret = cc3xx_chacha20_starts(
                &operation->chacha_ctx,
                iv,
                0U) )
            != PSA_SUCCESS) {
            return ret;
        }
    }

    return ret;
}

psa_status_t cc3xx_cipher_update(
        cc3xx_cipher_operation_t *operation,
        const uint8_t *input, size_t input_length,
        uint8_t *output, size_t output_size, size_t *output_length)
{
    psa_status_t ret = PSA_ERROR_CORRUPTION_DETECTED;
    size_t block_size = operation->block_size;
    size_t expected_output_size;

    if (!PSA_ALG_IS_STREAM_CIPHER(operation->alg)) {
        expected_output_size =
                ( operation->unprocessed_size + input_length )
                / operation->block_size * operation->block_size;
    } else {
        expected_output_size = input_length;
    }

    if (output_size < expected_output_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    *output_length = 0;

    size_t copy_len;

    switch (operation->key_type) {
    case PSA_KEY_TYPE_AES:
        switch (operation->alg) {
        case PSA_ALG_CBC_NO_PADDING:
        case PSA_ALG_CBC_PKCS7:
            if ((operation->dir == PSA_CRYPTO_DRIVER_DECRYPT &&
                 NULL != operation->add_padding              &&
                 input_length   <= block_size -
                 operation->unprocessed_size)                ||
                (operation->dir == PSA_CRYPTO_DRIVER_DECRYPT &&
                 NULL == operation->add_padding              &&
                 input_length    < block_size -
                 operation->unprocessed_size)                ||
                (operation->dir == PSA_CRYPTO_DRIVER_ENCRYPT &&
                 input_length    < block_size -
                 operation->unprocessed_size)) {
                CC_PalMemCopy(&(operation->unprocessed_data
                [operation->unprocessed_size]),
                              input,
                              input_length);

                operation->unprocessed_size += input_length;
                return PSA_SUCCESS;
            }

            if (0 != operation->unprocessed_size) {
                copy_len = block_size
                           - operation->unprocessed_size;

                CC_PalMemCopy(&(operation->unprocessed_data
                [operation->unprocessed_size]),
                              input,
                              copy_len);

                if (( ret = cc3xx_aes_crypt(
                        &operation->aes_ctx,
                        CIPHER_CBC,
                        block_size,
                        operation->iv,
                        operation->unprocessed_data,
                        output) )
                    != PSA_SUCCESS) {
                    return ret;
                }

                operation->unprocessed_size = 0;

                *output_length = block_size;
                output       += block_size;

                input         += copy_len;
                input_length  -= copy_len;
            }

            if (0 != input_length) {
                copy_len = input_length % block_size;

                if (copy_len == 0 &&
                    operation->dir==PSA_CRYPTO_DRIVER_DECRYPT &&
                    operation->add_padding != NULL) {
                    copy_len = block_size;
                }

                CC_PalMemCopy(operation->unprocessed_data,
                              &(input[copy_len]),
                              copy_len);

                operation->unprocessed_size += copy_len;
                input_length  -= copy_len;
            }

            if (0 != input_length) {
                if (( ret = cc3xx_aes_crypt(
                        &operation->aes_ctx,
                        CIPHER_CBC,
                        input_length,
                        operation->iv,
                        input,
                        output) )
                    != PSA_SUCCESS) {
                    return ret;
                }

                *output_length = input_length;
            }

            break;
        case PSA_ALG_CTR:
            if (( ret = cc3xx_aes_crypt(
                    &operation->aes_ctx,
                    CIPHER_CTR,
                    input_length,
                    operation->iv,
                    input,
                    output) )
                != PSA_SUCCESS) {
                return ret;
            }

            *output_length = input_length;

            break;
        case PSA_ALG_OFB:
            if (( ret = cc3xx_aes_crypt(
                    &operation->aes_ctx,
                    CIPHER_OFB,
                    input_length,
                    operation->iv,
                    input,
                    output) )
                != PSA_SUCCESS) {
                return ret;
            }

            *output_length = input_length;

            break;
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }

        if (*output_length > output_size) {
            return PSA_ERROR_CORRUPTION_DETECTED;
        }

        break;
    case PSA_KEY_TYPE_CHACHA20:
        if (( ret = cc3xx_chacha20_update(
                &operation->chacha_ctx,
                input_length,
                input,
                output) )
            != PSA_SUCCESS) {
            return ret;
        }

        *output_length = input_length;

        break;
    default:
        // Software fallback
        return PSA_ERROR_NOT_SUPPORTED;
    }

    return ret;
}

psa_status_t cc3xx_cipher_finish(
        cc3xx_cipher_operation_t *operation,
        uint8_t *output,
        size_t output_size,
        size_t *output_length)
{
    psa_status_t ret = PSA_ERROR_CORRUPTION_DETECTED;

    *output_length = 0;

    switch (operation->key_type) {
    case PSA_KEY_TYPE_AES:
        switch (operation->alg) {
        case PSA_ALG_CBC_NO_PADDING:
        case PSA_ALG_CBC_PKCS7:
            if (operation->dir == PSA_CRYPTO_DRIVER_ENCRYPT) {
                if (operation->add_padding == NULL) {
                    if (operation->unprocessed_size != 0) {
                        return PSA_ERROR_GENERIC_ERROR;
                    }

                    return PSA_SUCCESS;
                }

                if (( ret = operation->add_padding(
                        operation->unprocessed_data,
                        operation->block_size,
                        operation->unprocessed_size) )
                    != PSA_SUCCESS) {
                    return ret;
                }
            } else if (operation->block_size !=
                       operation->unprocessed_size) {
                if (operation->add_padding == NULL &&
                    operation->unprocessed_size == 0) {
                    return PSA_SUCCESS;
                }

                return PSA_ERROR_GENERIC_ERROR;
            }

            if (( ret = cc3xx_aes_crypt(
                    &operation->aes_ctx,
                    CIPHER_CBC,
                    output_size,
                    operation->iv,
                    operation->unprocessed_data,
                    output ))
                != PSA_SUCCESS) {
                return ret;
            }

            *output_length = operation->block_size;

            break;
        case PSA_ALG_CTR:
        case PSA_ALG_OFB:
            return PSA_SUCCESS;
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }
        break;
    case PSA_KEY_TYPE_CHACHA20:
        return PSA_SUCCESS;
    default:
        // Software fallback
        ret = PSA_ERROR_NOT_SUPPORTED;
    }

    return ret;
}

psa_status_t cc3xx_cipher_abort(cc3xx_cipher_operation_t *operation)
{
    switch (operation->key_type) {
    case PSA_KEY_TYPE_AES:
        cc3xx_aes_free(&operation->aes_ctx);
        break;
    case PSA_KEY_TYPE_CHACHA20:
        cc3xx_chacha20_free(&operation->chacha_ctx);
        return PSA_ERROR_NOT_SUPPORTED;
    default:
        // Software fallback
        return PSA_ERROR_NOT_SUPPORTED;
    }

    CC_PalMemSetZero(operation, sizeof(cc3xx_cipher_operation_t));

    return PSA_SUCCESS;
}

psa_status_t cc3xx_psa_cipher_encrypt(
        const psa_key_attributes_t *attributes,
        const uint8_t *key_buffer,
        size_t key_buffer_size,
        psa_algorithm_t alg,
        const uint8_t *input,
        size_t input_length,
        uint8_t *output,
        size_t output_size,
        size_t *output_length)
{
    psa_status_t ret = PSA_ERROR_CORRUPTION_DETECTED;
    cc3xx_cipher_operation_t operation = {0};
    size_t olength, accumulated_length;

    if ( (ret = cc3xx_cipher_encrypt_setup(
            &operation,
            attributes,
            key_buffer,
            key_buffer_size,
            alg) )
         != PSA_SUCCESS) {
        cc3xx_cipher_abort(&operation);
        return ret;
    }

    accumulated_length = 0;
    if (operation.iv_size > 0) {
        if ( (ret = cc3xx_cipher_set_iv(
                &operation,
                output,
                operation.iv_size) )
             != PSA_SUCCESS) {
            cc3xx_cipher_abort(&operation);
            return ret;
        }

        accumulated_length = operation.iv_size;
    }

    if ( (ret = cc3xx_cipher_update(
            &operation,
            input,
            input_length,
            output + operation.iv_size,
            output_size - operation.iv_size,
            &olength) )
         != PSA_SUCCESS) {
        cc3xx_cipher_abort(&operation);
        return ret;
    }

    accumulated_length += olength;

    if ( (ret = cc3xx_cipher_finish(
            &operation,
            output + accumulated_length,
            output_size - accumulated_length,
            &olength) )
         != PSA_SUCCESS) {
        cc3xx_cipher_abort(&operation);
        return ret;
    }

    *output_length = accumulated_length + olength;

    ret = cc3xx_cipher_abort(&operation);

    return ret;
}

psa_status_t cc3xx_psa_cipher_decrypt(
        const psa_key_attributes_t *attributes,
        const uint8_t *key_buffer,
        size_t key_buffer_size,
        psa_algorithm_t alg,
        const uint8_t *input,
        size_t input_length,
        uint8_t *output,
        size_t output_size,
        size_t *output_length)
{
    psa_status_t ret = PSA_ERROR_CORRUPTION_DETECTED;
    cc3xx_cipher_operation_t operation = {0};
    size_t olength, accumulated_length;

    if ( (ret = cc3xx_cipher_decrypt_setup(
            &operation,
            attributes,
            key_buffer,
            key_buffer_size,
            alg) )
         != PSA_SUCCESS) {
        cc3xx_cipher_abort(&operation);
        return ret;
    }

    if (operation.iv_size > 0) {
        if ( (ret = cc3xx_cipher_set_iv(
                &operation,
                input,
                operation.iv_size) )
             != PSA_SUCCESS) {
            cc3xx_cipher_abort(&operation);
            return ret;
        }
    }

    if ( (ret = cc3xx_cipher_update(
            &operation,
            input + operation.iv_size,
            input_length - operation.iv_size,
            output,
            output_size,
            &olength) )
         != PSA_SUCCESS) {
        cc3xx_cipher_abort(&operation);
        return ret;
    }

    accumulated_length = olength;

    if ( (ret = cc3xx_cipher_finish(
            &operation,
            output + accumulated_length,
            output_size - accumulated_length,
            &olength) )
         != PSA_SUCCESS) {
        cc3xx_cipher_abort(&operation);
        return ret;
    }

    *output_length = accumulated_length + olength;

    ret = cc3xx_cipher_abort(&operation);

    return ret;
}
