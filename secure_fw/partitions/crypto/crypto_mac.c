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
psa_status_t tfm_crypto_mac_interface(psa_invec in_vec[],
                                      psa_outvec out_vec[],
                                      mbedtls_svc_key_id_t *encoded_key)
{
#ifdef TFM_CRYPTO_MAC_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#endif
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_mac_operation_t *operation = NULL;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = NULL;
    enum tfm_crypto_function_type function_type =
                        TFM_CRYPTO_GET_FUNCTION_TYPE(iov->function_id);

    if (function_type != TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART) {
        handle_out = (out_vec && out_vec[0].base != NULL) ?
                                                out_vec[0].base : &handle;
        *handle_out = handle;
        status = tfm_crypto_operation_handling(TFM_CRYPTO_MAC_OPERATION,
                                               function_type,
                                               handle_out,
                                               (void **)&operation);
        if (status != PSA_SUCCESS) {
            return (iov->function_id == TFM_CRYPTO_MAC_ABORT_SID) ?
                    PSA_SUCCESS : status;
        }
    }

    switch (iov->function_id) {
    case TFM_CRYPTO_MAC_SIGN_SETUP_SID:
    {
        status = psa_mac_sign_setup(operation, *encoded_key, iov->alg);
        if (status != PSA_SUCCESS) {
            goto release_operation_and_return;
        }
    }
    break;
    case TFM_CRYPTO_MAC_VERIFY_SETUP_SID:
    {
        status = psa_mac_verify_setup(operation, *encoded_key, iov->alg);
        if (status != PSA_SUCCESS) {
            goto release_operation_and_return;
        }
    }
    break;
    case TFM_CRYPTO_MAC_UPDATE_SID:
    {
        const uint8_t *input = in_vec[1].base;
        size_t input_length = in_vec[1].len;

        status = psa_mac_update(operation, input, input_length);
    }
    break;
    case TFM_CRYPTO_MAC_SIGN_FINISH_SID:
    {
        uint8_t *mac = out_vec[1].base;
        size_t mac_size = out_vec[1].len;
        /* Initialise mac_length to zero */
        out_vec[1].len = 0;

        status = psa_mac_sign_finish(operation, mac, mac_size, &out_vec[1].len);
        if (status == PSA_SUCCESS) {
            /* In case of success automatically release the operation */
            goto release_operation_and_return;
        }
    }
    break;
    case TFM_CRYPTO_MAC_VERIFY_FINISH_SID:
    {
        const uint8_t *mac = in_vec[1].base;
        size_t mac_length = in_vec[1].len;

        status = psa_mac_verify_finish(operation, mac, mac_length);
        if (status == PSA_SUCCESS) {
            goto release_operation_and_return;
        }
    }
    break;
    case TFM_CRYPTO_MAC_ABORT_SID:
    {
        status = psa_mac_abort(operation);

        if (status != PSA_SUCCESS) {
            goto release_operation_and_return;
        } else {
            status = tfm_crypto_operation_release(handle_out);
        }
    }
    break;
    case TFM_CRYPTO_MAC_COMPUTE_SID:
    {
        const uint8_t *input = in_vec[1].base;
        size_t input_length = in_vec[1].len;
        uint8_t *mac = out_vec[0].base;
        size_t mac_size = out_vec[0].len;

        status = psa_mac_compute(*encoded_key, iov->alg, input, input_length,
                                 mac, mac_size, &out_vec[0].len);
    }
    break;
    case TFM_CRYPTO_MAC_VERIFY_SID:
    {
        const uint8_t *input = in_vec[1].base;
        size_t input_length = in_vec[1].len;
        const uint8_t *mac = in_vec[2].base;
        size_t mac_length = in_vec[2].len;

        status = psa_mac_verify(*encoded_key, iov->alg, input, input_length,
                                mac, mac_length);
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
