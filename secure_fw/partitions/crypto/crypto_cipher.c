/*
 * Copyright (c) 2018-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "tfm_mbedcrypto_include.h"

#include "tfm_crypto_api.h"
#include "tfm_crypto_defs.h"

/*!
 * \addtogroup tfm_crypto_api_shim_layer
 *
 */

/*!@{*/
psa_status_t tfm_crypto_cipher_interface(psa_invec in_vec[],
                                         psa_outvec out_vec[],
                                         mbedtls_svc_key_id_t *encoded_key)
{
#ifdef TFM_CRYPTO_CIPHER_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#endif
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_cipher_operation_t *operation = NULL;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = NULL;
    enum tfm_crypto_function_type function_type =
                        TFM_CRYPTO_GET_FUNCTION_TYPE(iov->function_id);

    if (function_type != TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART) {
        handle_out = (out_vec && out_vec[0].base != NULL) ?
                                                out_vec[0].base : &handle;
        *handle_out = handle;
        status = tfm_crypto_operation_handling(TFM_CRYPTO_CIPHER_OPERATION,
                                               function_type,
                                               handle_out,
                                               (void **)&operation);
        if (status != PSA_SUCCESS) {
            return (iov->function_id == TFM_CRYPTO_CIPHER_ABORT_SID) ?
                    PSA_SUCCESS : status;
        }
    }

    switch (iov->function_id) {
    case TFM_CRYPTO_CIPHER_GENERATE_IV_SID:
    {
        unsigned char *iv = out_vec[0].base;
        size_t iv_size = out_vec[0].len;

        status = psa_cipher_generate_iv(operation,
                                        iv, iv_size, &out_vec[0].len);
    }
    break;
    case TFM_CRYPTO_CIPHER_SET_IV_SID:
    {
        const unsigned char *iv = in_vec[1].base;
        size_t iv_length = in_vec[1].len;

        status = psa_cipher_set_iv(operation, iv, iv_length);
    }
    break;
    case TFM_CRYPTO_CIPHER_ENCRYPT_SETUP_SID:
    {
        status = psa_cipher_encrypt_setup(operation, *encoded_key, iov->alg);
        if (status != PSA_SUCCESS) {
            goto release_operation_and_return;
        }
    }
    break;
    case TFM_CRYPTO_CIPHER_DECRYPT_SETUP_SID:
    {
        status = psa_cipher_decrypt_setup(operation, *encoded_key, iov->alg);
        if (status != PSA_SUCCESS) {
            goto release_operation_and_return;
        }
    }
    break;
    case TFM_CRYPTO_CIPHER_UPDATE_SID:
    {
        const uint8_t *input = in_vec[1].base;
        size_t input_length = in_vec[1].len;
        unsigned char *output = out_vec[0].base;
        size_t output_size = out_vec[0].len;
        /* Initialise the output_length to zero */
        out_vec[0].len = 0;

        status = psa_cipher_update(operation, input, input_length,
                                   output, output_size, &out_vec[0].len);
    }
    break;
    case TFM_CRYPTO_CIPHER_FINISH_SID:
    {
        uint8_t *output = out_vec[1].base;
        size_t output_size = out_vec[1].len;
        /* Initialise the output_length to zero */
        out_vec[1].len = 0;

        status = psa_cipher_finish(operation,
                                   output, output_size, &out_vec[1].len);
        if (status == PSA_SUCCESS) {
            /* In case of success automatically release the operation */
            goto release_operation_and_return;
        }
    }
    break;
    case TFM_CRYPTO_CIPHER_ABORT_SID:
    {
        status = psa_cipher_abort(operation);

        if (status != PSA_SUCCESS) {
            goto release_operation_and_return;
        } else {
            status = tfm_crypto_operation_release(handle_out);
        }
    }
    break;
    case TFM_CRYPTO_CIPHER_ENCRYPT_SID:
    {
#ifdef CRYPTO_SINGLE_PART_FUNCS_DISABLED
        return PSA_ERROR_NOT_SUPPORTED;
#else
        const uint8_t *input = in_vec[1].base;
        size_t input_length = in_vec[1].len;
        uint8_t *output = out_vec[0].base;
        size_t output_size = out_vec[0].len;

        status = psa_cipher_encrypt(*encoded_key, iov->alg, input, input_length,
                                    output, output_size, &out_vec[0].len);
        break;
#endif
    }
    case TFM_CRYPTO_CIPHER_DECRYPT_SID:
    {
#ifdef CRYPTO_SINGLE_PART_FUNCS_DISABLED
        return PSA_ERROR_NOT_SUPPORTED;
#else
        const uint8_t *input = in_vec[1].base;
        size_t input_length = in_vec[1].len;
        uint8_t *output = out_vec[0].base;
        size_t output_size = out_vec[0].len;

        status = psa_cipher_decrypt(*encoded_key, iov->alg, input, input_length,
                                    output, output_size, &out_vec[0].len);
        break;
#endif
    }
    default:
        status = PSA_ERROR_NOT_SUPPORTED;
    }

    return status;
release_operation_and_return:
    /* Release the operation context, ignore if the operation fails. */
    (void)tfm_crypto_operation_release(handle_out);
    return status;
}
/*!@}*/
