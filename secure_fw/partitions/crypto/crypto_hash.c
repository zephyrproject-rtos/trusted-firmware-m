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
psa_status_t tfm_crypto_hash_interface(psa_invec in_vec[],
                                       psa_outvec out_vec[])
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#endif
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_hash_operation_t *operation = NULL;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = NULL;
    /* Used to save the target_operation during cloning */
    uint32_t handle_clone = 0;
    enum tfm_crypto_function_type function_type =
                        TFM_CRYPTO_GET_FUNCTION_TYPE(iov->function_id);

    if (function_type != TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART) {
        handle_out = (out_vec && out_vec[0].base != NULL) ?
                                                out_vec[0].base : &handle;
        *handle_out = handle;
        status = tfm_crypto_operation_handling(TFM_CRYPTO_HASH_OPERATION,
                                               function_type,
                                               handle_out,
                                               (void **)&operation);
        if (status != PSA_SUCCESS) {
            return (iov->function_id == TFM_CRYPTO_HASH_ABORT_SID) ?
                    PSA_SUCCESS : status;
        }
    }

    switch (iov->function_id) {
    case TFM_CRYPTO_HASH_SETUP_SID:
    {
        status = psa_hash_setup(operation, iov->alg);
        if (status != PSA_SUCCESS) {
            goto release_operation_and_return;
        }
    }
    break;
    case TFM_CRYPTO_HASH_UPDATE_SID:
    {
        const uint8_t *input = in_vec[1].base;
        size_t input_length = in_vec[1].len;

        status = psa_hash_update(operation, input, input_length);
    }
    break;
    case TFM_CRYPTO_HASH_FINISH_SID:
    {
        uint8_t *hash = out_vec[1].base;
        size_t hash_size = out_vec[1].len;
        /* Initialise the output_length to zero */
        out_vec[1].len = 0;

        status = psa_hash_finish(operation, hash, hash_size, &out_vec[1].len);
        if (status == PSA_SUCCESS) {
            goto release_operation_and_return;
        }
    }
    break;
    case TFM_CRYPTO_HASH_VERIFY_SID:
    {
        const uint8_t *hash = in_vec[1].base;
        size_t hash_length = in_vec[1].len;

        status = psa_hash_verify(operation, hash, hash_length);
        if (status == PSA_SUCCESS) {
            goto release_operation_and_return;
        }
    }
    break;
    case TFM_CRYPTO_HASH_ABORT_SID:
    {
        status = psa_hash_abort(operation);

        if (status != PSA_SUCCESS) {
            goto release_operation_and_return;
        } else {
            status = tfm_crypto_operation_release(handle_out);
        }
    }
    break;
    case TFM_CRYPTO_HASH_CLONE_SID:
    {
        psa_hash_operation_t *target_operation = NULL;

        /* handle_out is initialised here for the hash clone as a special
         * case which don't rely on the standard flow for handling
         * multipart operation contexts
         */
        *handle_out = handle_clone;

        /* Allocate the target operation context in the secure world */
        status = tfm_crypto_operation_alloc(TFM_CRYPTO_HASH_OPERATION,
                                            handle_out,
                                            (void **)&target_operation);
        if (status != PSA_SUCCESS) {
            return status;
        }
        status = psa_hash_clone(operation, target_operation);
        if (status != PSA_SUCCESS) {
            goto release_operation_and_return;
        }
    }
    break;
    case TFM_CRYPTO_HASH_COMPUTE_SID:
    {
#ifdef CRYPTO_SINGLE_PART_FUNCS_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
        const uint8_t *input = in_vec[1].base;
        size_t input_length = in_vec[1].len;
        uint8_t *hash = out_vec[0].base;
        size_t hash_size = out_vec[0].len;
        /* Initialize hash_length to zero */
        out_vec[0].len = 0;

        status = psa_hash_compute(iov->alg, input, input_length,
                                  hash, hash_size, &out_vec[0].len);
        break;
#endif
    }
    case TFM_CRYPTO_HASH_COMPARE_SID:
    {
#ifdef CRYPTO_SINGLE_PART_FUNCS_DISABLED
        return PSA_ERROR_NOT_SUPPORTED;
#else
        const uint8_t *input = in_vec[1].base;
        size_t input_length = in_vec[1].len;
        const uint8_t *hash = in_vec[2].base;
        size_t hash_length = in_vec[2].len;

        status = psa_hash_compare(iov->alg, input, input_length,
                                  hash, hash_length);
        break;
#endif
    }
    default:
        status = PSA_ERROR_NOT_SUPPORTED;
    }

    return status;
release_operation_and_return:
    /* Release the operation context, ignore if the release fails. */
    (void)tfm_crypto_operation_release(handle_out);
    return status;
}
/*!@}*/
