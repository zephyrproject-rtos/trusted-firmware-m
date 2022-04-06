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
 * \addtogroup tfm_crypto_api_shim_layer
 *
 */

/*!@{*/
psa_status_t tfm_crypto_asymmetric_interface(psa_invec in_vec[],
                                             psa_outvec out_vec[],
                                             mbedtls_svc_key_id_t *encoded_key)
{
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

#ifdef TFM_CRYPTO_ASYM_ENCRYPT_MODULE_DISALBED
    if (TFM_CRYPTO_IS_GROUP_ID(iov->function_id,
                               TFM_CRYPTO_GROUP_ID_ASYM_ENCRYPT)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
#endif
#ifdef TFM_CRYPTO_ASYM_SIGN_MODULE_DISALBED
    if (TFM_CRYPTO_IS_GROUP_ID(iov->function_id,
                               TFM_CRYPTO_GROUP_ID_ASYM_SIGN)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
#endif

    switch (iov->function_id) {
    case TFM_CRYPTO_ASYMMETRIC_SIGN_MESSAGE_SID:
    {
        const uint8_t *input = in_vec[1].base;
        size_t input_length = in_vec[1].len;
        uint8_t *signature = out_vec[0].base;
        size_t signature_size = out_vec[0].len;

        status = psa_sign_message(*encoded_key, iov->alg, input, input_length,
                                  signature, signature_size, &(out_vec[0].len));
    }
    break;
    case TFM_CRYPTO_ASYMMETRIC_VERIFY_MESSAGE_SID:
    {
        const uint8_t *input = in_vec[1].base;
        size_t input_length = in_vec[1].len;
        const uint8_t *signature = in_vec[2].base;
        size_t signature_length = in_vec[2].len;

        status = psa_verify_message(*encoded_key, iov->alg, input, input_length,
                                    signature, signature_length);
    }
    break;
    case TFM_CRYPTO_ASYMMETRIC_SIGN_HASH_SID:
    {
        const uint8_t *hash = in_vec[1].base;
        size_t hash_length = in_vec[1].len;
        uint8_t *signature = out_vec[0].base;
        size_t signature_size = out_vec[0].len;

        status = psa_sign_hash(*encoded_key, iov->alg, hash, hash_length,
                               signature, signature_size, &(out_vec[0].len));
    }
    break;
    case TFM_CRYPTO_ASYMMETRIC_VERIFY_HASH_SID:
    {
        const uint8_t *hash = in_vec[1].base;
        size_t hash_length = in_vec[1].len;
        const uint8_t *signature = in_vec[2].base;
        size_t signature_length = in_vec[2].len;

        status = psa_verify_hash(*encoded_key, iov->alg, hash, hash_length,
                                 signature, signature_length);
    }
    break;
    case TFM_CRYPTO_ASYMMETRIC_ENCRYPT_SID:
    {
        const uint8_t *input = in_vec[1].base;
        size_t input_length = in_vec[1].len;
        const uint8_t *salt = in_vec[2].base;
        size_t salt_length = in_vec[2].len;
        uint8_t *output = out_vec[0].base;
        size_t output_size = out_vec[0].len;

        status = psa_asymmetric_encrypt(*encoded_key, iov->alg,
                                        input, input_length,
                                        salt, salt_length,
                                        output, output_size,
                                        &(out_vec[0].len));
    }
    break;
    case TFM_CRYPTO_ASYMMETRIC_DECRYPT_SID:
    {
        const uint8_t *input = in_vec[1].base;
        size_t input_length = in_vec[1].len;
        const uint8_t *salt = in_vec[2].base;
        size_t salt_length = in_vec[2].len;
        uint8_t *output = out_vec[0].base;
        size_t output_size = out_vec[0].len;

        status = psa_asymmetric_decrypt(*encoded_key, iov->alg,
                                        input, input_length,
                                        salt, salt_length,
                                        output, output_size,
                                        &(out_vec[0].len));
    }
    break;
    default:
        status = PSA_ERROR_NOT_SUPPORTED;
    }

    return status;
}
/*!@}*/
