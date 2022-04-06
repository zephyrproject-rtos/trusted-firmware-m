/*
 * Copyright (c) 2019-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "tfm_sp_log.h"

#include "tfm_mbedcrypto_include.h"

/* Required for mbedtls_calloc in tfm_crypto_huk_derivation_input_bytes */
#include "mbedtls/platform.h"

#include "tfm_crypto_api.h"
#include "tfm_crypto_defs.h"

#include "tfm_plat_crypto_keys.h"

#ifndef MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER
#error "MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER must be selected in Mbed TLS config file"
#endif

#ifdef TFM_PARTITION_TEST_PS
#include "psa_manifest/pid.h"
#endif /* TFM_PARTITION_TEST_PS */

#ifndef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
static psa_status_t tfm_crypto_huk_derivation_setup(
                                      psa_key_derivation_operation_t *operation,
                                      psa_algorithm_t alg)
{
    operation->MBEDTLS_PRIVATE(alg) = TFM_CRYPTO_ALG_HUK_DERIVATION;
    return PSA_SUCCESS;
}

static psa_status_t tfm_crypto_huk_derivation_input_bytes(
                                      psa_key_derivation_operation_t *operation,
                                      psa_key_derivation_step_t step,
                                      const uint8_t *data,
                                      size_t data_length)
{
    psa_status_t status;
    int32_t partition_id;
    psa_tls12_prf_key_derivation_t *tls12_prf;

    if (step != PSA_KEY_DERIVATION_INPUT_LABEL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Concatenate the caller's partition ID with the supplied label to prevent
     * two different partitions from deriving the same key.
     */
    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

#ifdef TFM_PARTITION_TEST_PS
    /* The PS tests run some operations under the wrong partition ID - this
     * causes the key derivation to change.
     */
    if (partition_id == TFM_SP_PS_TEST) {
        partition_id = TFM_SP_PS;
    }
#endif /* TFM_PARTITION_TEST_PS */

    /* Put the label in the tls12_prf ctx to make it available in the output key
     * step.
     */
    tls12_prf = &(operation->MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(tls12_prf));
    tls12_prf->MBEDTLS_PRIVATE(label) =
                 mbedtls_calloc(1, sizeof(partition_id) + data_length);
    if (tls12_prf->MBEDTLS_PRIVATE(label) == NULL) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }
    (void)memcpy(tls12_prf->MBEDTLS_PRIVATE(label), &partition_id,
                 sizeof(partition_id));
    (void)memcpy(tls12_prf->MBEDTLS_PRIVATE(label) + sizeof(partition_id),
                 data, data_length);
    tls12_prf->MBEDTLS_PRIVATE(label_length) = sizeof(partition_id) +
                                               data_length;

    return PSA_SUCCESS;
}

static psa_status_t tfm_crypto_huk_derivation_output_key(
                                      const psa_key_attributes_t *attributes,
                                      psa_key_derivation_operation_t *operation,
                                      mbedtls_svc_key_id_t *key_id)
{
    enum tfm_plat_err_t err;
    size_t bytes = PSA_BITS_TO_BYTES(psa_get_key_bits(attributes));
    psa_tls12_prf_key_derivation_t *tls12_prf =
                &(operation->MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(tls12_prf));

    if (sizeof(tls12_prf->MBEDTLS_PRIVATE(output_block)) < bytes) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    /* Derive key material from the HUK and output it to the operation buffer */
    /* The function below accesses an abstracted functionality at platform level
     * to get a key derived from the HUK. The specific algorithm is abstracted
     * away to the platform implementation (i.e. it can be HMAC-based KDF or
     * CMAC based KDF, the latter for example when CC-312 is present)
     */
    err = tfm_plat_get_huk_derived_key(tls12_prf->MBEDTLS_PRIVATE(label),
                                       tls12_prf->MBEDTLS_PRIVATE(label_length),
                                       NULL, 0,
                                       tls12_prf->MBEDTLS_PRIVATE(output_block),
                                       bytes);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }
    LOG_DBGFMT("\033[0;32mcomplete.\033[0m\r\n");

    return psa_import_key(attributes, tls12_prf->MBEDTLS_PRIVATE(output_block),
                          bytes, key_id);
}

static psa_status_t tfm_crypto_huk_derivation_abort(
                                      psa_key_derivation_operation_t *operation)
{
    psa_tls12_prf_key_derivation_t *tls12_prf =
                &(operation->MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(tls12_prf));

    if (tls12_prf->MBEDTLS_PRIVATE(label) != NULL) {
        (void)memset(tls12_prf->MBEDTLS_PRIVATE(label), 0,
                     tls12_prf->MBEDTLS_PRIVATE(label_length));
        mbedtls_free(tls12_prf->MBEDTLS_PRIVATE(label));
    }

    (void)memset(operation, 0, sizeof(*operation));

    return PSA_SUCCESS;
}
#endif /* TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED */

/*!
 * \addtogroup tfm_crypto_api_shim_layer
 *
 */

/*!@{*/
psa_status_t tfm_crypto_key_derivation_interface(psa_invec in_vec[],
                                            psa_outvec out_vec[],
                                            mbedtls_svc_key_id_t *encoded_key)
{
#ifdef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#endif
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_derivation_operation_t *operation = NULL;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = NULL;
    enum tfm_crypto_function_type function_type =
                        TFM_CRYPTO_GET_FUNCTION_TYPE(iov->function_id);

    if (function_type != TFM_CRYPTO_FUNCTION_TYPE_NON_MULTIPART) {
        handle_out = (out_vec && out_vec[0].base != NULL) ?
                                                out_vec[0].base : &handle;
        *handle_out = handle;
        status = tfm_crypto_operation_handling(
                                        TFM_CRYPTO_KEY_DERIVATION_OPERATION,
                                        function_type,
                                        handle_out,
                                        (void **)&operation);
        if (status != PSA_SUCCESS) {
            return (iov->function_id == TFM_CRYPTO_KEY_DERIVATION_ABORT_SID) ?
                    PSA_SUCCESS : status;
        }
    }

    switch (iov->function_id) {
    case TFM_CRYPTO_KEY_DERIVATION_SETUP_SID:
    {
        if (iov->alg == TFM_CRYPTO_ALG_HUK_DERIVATION) {
            status = tfm_crypto_huk_derivation_setup(operation, iov->alg);
        } else {
            status = psa_key_derivation_setup(operation, iov->alg);
        }

        if (status != PSA_SUCCESS) {
            goto release_operation_and_return;
        }
    }
    break;
    case TFM_CRYPTO_KEY_DERIVATION_GET_CAPACITY_SID:
    {
        size_t *capacity = out_vec[0].base;

        status = psa_key_derivation_get_capacity(operation, capacity);
    }
    break;
    case TFM_CRYPTO_KEY_DERIVATION_SET_CAPACITY_SID:
    {
        status = psa_key_derivation_set_capacity(operation, iov->capacity);
    }
    break;
    case TFM_CRYPTO_KEY_DERIVATION_INPUT_BYTES_SID:
    {
        const uint8_t *data = in_vec[1].base;
        size_t data_length = in_vec[1].len;

        if (operation->MBEDTLS_PRIVATE(alg) == TFM_CRYPTO_ALG_HUK_DERIVATION) {
            status = tfm_crypto_huk_derivation_input_bytes(operation,
                                                           iov->step, data,
                                                           data_length);
        } else {
            status = psa_key_derivation_input_bytes(operation, iov->step, data,
                                                    data_length);
        }
    }
    break;
    case TFM_CRYPTO_KEY_DERIVATION_OUTPUT_BYTES_SID:
    {
        uint8_t *output = out_vec[0].base;
        size_t output_length = out_vec[0].len;

        status = psa_key_derivation_output_bytes(operation,
                                                 output, output_length);
    }
    break;
    case TFM_CRYPTO_KEY_DERIVATION_INPUT_KEY_SID:
    {
         status = psa_key_derivation_input_key(operation,
                                               iov->step, *encoded_key);
    }
    break;
    case TFM_CRYPTO_KEY_DERIVATION_OUTPUT_KEY_SID:
    {
        const struct psa_client_key_attributes_s *client_key_attr =
                                                            in_vec[1].base;
        psa_key_id_t *key_handle = out_vec[0].base;
        psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
        int32_t partition_id = MBEDTLS_SVC_KEY_ID_GET_OWNER_ID(*encoded_key);

        status = tfm_crypto_key_attributes_from_client(client_key_attr,
                                                       partition_id,
                                                       &key_attributes);
        if (status != PSA_SUCCESS) {
            return status;
        }

        if (operation->MBEDTLS_PRIVATE(alg) == TFM_CRYPTO_ALG_HUK_DERIVATION) {
            status = tfm_crypto_huk_derivation_output_key(&key_attributes,
                                                    operation, encoded_key);
        } else {
            status = psa_key_derivation_output_key(&key_attributes, operation,
                                                   encoded_key);
        }

        *key_handle = encoded_key->MBEDTLS_PRIVATE(key_id);
    }
    break;
    case TFM_CRYPTO_KEY_DERIVATION_ABORT_SID:
    {
        if (operation->MBEDTLS_PRIVATE(alg) == TFM_CRYPTO_ALG_HUK_DERIVATION) {
            status = tfm_crypto_huk_derivation_abort(operation);
        } else {
            status = psa_key_derivation_abort(operation);
        }

        if (status != PSA_SUCCESS) {
            goto release_operation_and_return;
        } else {
            status = tfm_crypto_operation_release(handle_out);
        }
    }
    break;
    case TFM_CRYPTO_KEY_DERIVATION_KEY_AGREEMENT_SID:
    {
        const uint8_t *peer_key = in_vec[1].base;
        size_t peer_key_length = in_vec[1].len;

        status = psa_key_derivation_key_agreement(operation, iov->step,
                                                  *encoded_key,
                                                  peer_key,
                                                  peer_key_length);
    }
    break;
    case TFM_CRYPTO_RAW_KEY_AGREEMENT_SID:
    {
        uint8_t *output = out_vec[0].base;
        size_t output_size = out_vec[0].len;
        const uint8_t *peer_key = in_vec[1].base;
        size_t peer_key_length = in_vec[1].len;

        status = psa_raw_key_agreement(iov->alg, *encoded_key,
                                       peer_key, peer_key_length,
                                       output, output_size, &out_vec[0].len);
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
