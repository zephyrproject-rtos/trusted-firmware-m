/*
 * Copyright (c) 2019-2022, Arm Limited. All rights reserved.
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
 * \defgroup tfm_crypto_api_shim_layer Set of functions implementing a thin shim
 *                                  layer between the TF-M Crypto service
 *                                  frontend and the underlying library which
 *                                  implements the PSA Crypto APIs
 *                                  (i.e. mbed TLS)
 */

/*!@{*/
psa_status_t tfm_crypto_aead_interface(psa_invec in_vec[],
                                       psa_outvec out_vec[],
                                       mbedtls_svc_key_id_t *encoded_key)
{
#ifdef TFM_CRYPTO_AEAD_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#endif
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_aead_operation_t *operation = NULL;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = NULL;
    enum tfm_crypto_function_type function_type =
                        TFM_CRYPTO_GET_FUNCTION_TYPE(iov->function_id);

    if (function_type != TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART) {
        handle_out = (out_vec && out_vec[0].base != NULL) ?
                                                out_vec[0].base : &handle;
        *handle_out = handle;
        status = tfm_crypto_operation_handling(TFM_CRYPTO_AEAD_OPERATION,
                                               function_type,
                                               handle_out,
                                               (void **)&operation);
        if (status != PSA_SUCCESS) {
            return (iov->function_id == TFM_CRYPTO_AEAD_ABORT_SID) ?
                    PSA_SUCCESS : status;
        }
    }

    switch (iov->function_id) {
    case TFM_CRYPTO_AEAD_ENCRYPT_SID:
    {
#ifdef CRYPTO_SINGLE_PART_FUNCS_DISABLED
        return PSA_ERROR_NOT_SUPPORTED;
#else
        const struct tfm_crypto_aead_pack_input *aead_pack_input =
                                                                 &iov->aead_in;
        const uint8_t *nonce = aead_pack_input->nonce;
        size_t nonce_length = aead_pack_input->nonce_length;
        const uint8_t *plaintext = in_vec[1].base;
        size_t plaintext_length = in_vec[1].len;
        uint8_t *ciphertext = out_vec[0].base;
        size_t ciphertext_size = out_vec[0].len;
        const uint8_t *additional_data = in_vec[2].base;
        size_t additional_data_length = in_vec[2].len;
        /* Initialise ciphertext_length to zero. */
        out_vec[0].len = 0;

        status = psa_aead_encrypt(*encoded_key, iov->alg, nonce, nonce_length,
                            additional_data, additional_data_length,
                            plaintext, plaintext_length,
                            ciphertext, ciphertext_size, &out_vec[0].len);
        break;
#endif
    }
    case TFM_CRYPTO_AEAD_DECRYPT_SID:
    {
#ifdef CRYPTO_SINGLE_PART_FUNCS_DISABLED
        return PSA_ERROR_NOT_SUPPORTED;
#else
        const struct tfm_crypto_aead_pack_input *aead_pack_input =
                                                                 &iov->aead_in;
        const uint8_t *nonce = aead_pack_input->nonce;
        size_t nonce_length = aead_pack_input->nonce_length;
        const uint8_t *ciphertext = in_vec[1].base;
        size_t ciphertext_length = in_vec[1].len;
        uint8_t *plaintext = out_vec[0].base;
        size_t plaintext_size = out_vec[0].len;
        const uint8_t *additional_data = in_vec[2].base;
        size_t additional_data_length = in_vec[2].len;
        /* Initialise plaintext_length to zero. */
        out_vec[0].len = 0;

        status = psa_aead_decrypt(*encoded_key, iov->alg, nonce, nonce_length,
                            additional_data, additional_data_length,
                            ciphertext, ciphertext_length,
                            plaintext, plaintext_size, &out_vec[0].len);
        break;
#endif
    }
    case TFM_CRYPTO_AEAD_ENCRYPT_SETUP_SID:
    {
        status = psa_aead_encrypt_setup(operation, *encoded_key, iov->alg);
        if (status != PSA_SUCCESS) {
            goto release_operation_and_return;
        }
    }
    break;
    case TFM_CRYPTO_AEAD_DECRYPT_SETUP_SID:
    {
        status = psa_aead_decrypt_setup(operation, *encoded_key, iov->alg);
        if (status != PSA_SUCCESS) {
            goto release_operation_and_return;
        }
    }
    break;
    case TFM_CRYPTO_AEAD_ABORT_SID:
    {
        status = psa_aead_abort(operation);

        if (status != PSA_SUCCESS) {
            goto release_operation_and_return;
        } else {
            status = tfm_crypto_operation_release(handle_out);
        }
    }
    break;
    case TFM_CRYPTO_AEAD_FINISH_SID:
    {
        uint8_t *ciphertext = out_vec[2].base;
        size_t ciphertext_size = out_vec[2].len;
        uint8_t *tag = out_vec[1].base;
        size_t tag_size = out_vec[1].len;
        /* Initialise tag and ciphertext lengths to zero */
        out_vec[1].len = 0;
        out_vec[2].len = 0;

        status = psa_aead_finish(operation,
                                 ciphertext, ciphertext_size, &out_vec[2].len,
                                 tag, tag_size, &out_vec[1].len);
        if (status == PSA_SUCCESS) {
            goto release_operation_and_return;
        }
    }
    break;
    case TFM_CRYPTO_AEAD_GENERATE_NONCE_SID:
    {
        uint8_t *nonce = out_vec[0].base;
        size_t nonce_size = out_vec[0].len;
        /* Initialise nonce length to zero */
        out_vec[0].len = 0;

        status = psa_aead_generate_nonce(operation,
                                         nonce,
                                         nonce_size,
                                         &out_vec[0].len);
    }
    break;
    case TFM_CRYPTO_AEAD_SET_NONCE_SID:
    {
        const uint8_t *nonce = in_vec[1].base;
        size_t nonce_size = in_vec[1].len;

        status = psa_aead_set_nonce(operation, nonce, nonce_size);
    }
    break;
    case TFM_CRYPTO_AEAD_SET_LENGTHS_SID:
    {
        status = psa_aead_set_lengths(operation, iov->ad_length,
                                      iov->plaintext_length);
    }
    break;
    case TFM_CRYPTO_AEAD_UPDATE_SID:
    {
        const uint8_t *input = in_vec[1].base;
        size_t input_length = in_vec[1].len;
        uint8_t *output = out_vec[0].base;
        size_t output_size = out_vec[0].len;

        status = psa_aead_update(operation, input, input_length,
                                 output, output_size, &out_vec[0].len);
    }
    break;
    case TFM_CRYPTO_AEAD_UPDATE_AD_SID:
    {
        const uint8_t *input = in_vec[1].base;
        size_t input_length = in_vec[1].len;

        status = psa_aead_update_ad(operation, input, input_length);
    }
    break;
    case TFM_CRYPTO_AEAD_VERIFY_SID:
    {
        const uint8_t *tag = in_vec[1].base;
        size_t tag_length = in_vec[1].len;
        uint8_t *plaintext = out_vec[1].base;
        size_t plaintext_size = out_vec[1].len;

        status = psa_aead_verify(operation,
                                 plaintext, plaintext_size, &out_vec[1].len,
                                 tag, tag_length);

        if (status == PSA_SUCCESS) {
            goto release_operation_and_return;
        }
    }
    break;
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
