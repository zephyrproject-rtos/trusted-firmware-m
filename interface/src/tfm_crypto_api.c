/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_veneers.h"
#include "tfm_crypto_defs.h"
#include "psa_crypto.h"
#include "tfm_ns_lock.h"

#define NS_LOCK_DISPATCH(sfn_name)                                   \
    tfm_ns_lock_dispatch((veneer_fn)tfm_##sfn_name##_veneer,         \
        (uint32_t)in_vec, sizeof(in_vec)/sizeof(in_vec[0]),          \
        (uint32_t)out_vec, sizeof(out_vec)/sizeof(out_vec[0]))

#define NS_LOCK_DISPATCH_NO_INVEC(sfn_name)                          \
    tfm_ns_lock_dispatch((veneer_fn)tfm_##sfn_name##_veneer,         \
        (uint32_t)NULL, 0,                                           \
        (uint32_t)out_vec, sizeof(out_vec)/sizeof(out_vec[0]))

#define NS_LOCK_DISPATCH_NO_OUTVEC(sfn_name)                         \
    tfm_ns_lock_dispatch((veneer_fn)tfm_##sfn_name##_veneer,         \
        (uint32_t)in_vec, sizeof(in_vec)/sizeof(in_vec[0]),          \
        (uint32_t)NULL, 0)

#define API_DISPATCH(sfn_name) NS_LOCK_DISPATCH(sfn_name)
#define API_DISPATCH_NO_INVEC(sfn_name) NS_LOCK_DISPATCH_NO_INVEC(sfn_name)
#define API_DISPATCH_NO_OUTVEC(sfn_name) NS_LOCK_DISPATCH_NO_OUTVEC(sfn_name)

psa_status_t psa_crypto_init(void)
{
    /* Service init is performed during TFM boot up,
     * so application level initialisation is empty
     */
    return PSA_SUCCESS;
}

psa_status_t psa_import_key(psa_key_slot_t key,
                            psa_key_type_t type,
                            const uint8_t *data,
                            size_t data_length)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = &key, .len = sizeof(psa_key_slot_t)},
        {.base = &type, .len = sizeof(psa_key_type_t)},
        {.base = data, .len = data_length}
    };

    status = API_DISPATCH_NO_OUTVEC(tfm_crypto_import_key);

    return status;
}

psa_status_t psa_destroy_key(psa_key_slot_t key)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = &key, .len = sizeof(psa_key_slot_t)},
    };

    status = API_DISPATCH_NO_OUTVEC(tfm_crypto_destroy_key);

    return status;
}

psa_status_t psa_get_key_information(psa_key_slot_t key,
                                     psa_key_type_t *type,
                                     size_t *bits)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = &key, .len = sizeof(psa_key_slot_t)},
    };
    psa_outvec out_vec[] = {
        {.base = type, .len = sizeof(psa_key_type_t)},
        {.base = bits, .len = sizeof(size_t)}
    };

    status = API_DISPATCH(tfm_crypto_get_key_information);

    return status;
}

psa_status_t psa_export_key(psa_key_slot_t key,
                            uint8_t *data,
                            size_t data_size,
                            size_t *data_length)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = &key, .len = sizeof(psa_key_slot_t)},
    };
    psa_outvec out_vec[] = {
        {.base = data, .len = data_size}
    };

    status = API_DISPATCH(tfm_crypto_export_key);

    *data_length = out_vec[0].len;

    return status;
}

psa_status_t psa_export_public_key(psa_key_slot_t key,
                                   uint8_t *data,
                                   size_t data_size,
                                   size_t *data_length)
{
    (void)key;
    (void)data;
    (void)data_size;
    (void)data_length;

    /* TODO: This API is not supported yet */
    return PSA_ERROR_NOT_SUPPORTED;
}

void psa_key_policy_init(psa_key_policy_t *policy)
{
    psa_status_t status;
    psa_outvec out_vec[] = {
        {.base = policy, .len = sizeof(psa_key_policy_t)},
    };

    /* PSA API returns void so just ignore error value returned */
    status = API_DISPATCH_NO_INVEC(tfm_crypto_key_policy_init);
}

void psa_key_policy_set_usage(psa_key_policy_t *policy,
                              psa_key_usage_t usage,
                              psa_algorithm_t alg)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = &usage, .len = sizeof(psa_key_usage_t)},
        {.base = &alg, .len = sizeof(psa_algorithm_t)}
    };
    psa_outvec out_vec[] = {
        {.base = policy, .len = sizeof(psa_key_policy_t)},
    };

    /* PSA API returns void so just ignore error value returned */
    status = API_DISPATCH(tfm_crypto_key_policy_set_usage);
}

psa_key_usage_t psa_key_policy_get_usage(const psa_key_policy_t *policy)
{
    psa_status_t status;
    psa_key_usage_t usage;

    /* Initialise to a sensible default to avoid returning an uninitialised
     * value in case the secure function fails.
     */
    usage = 0;

    psa_invec in_vec[] = {
        {.base = policy, .len = sizeof(psa_key_policy_t)},
    };
    psa_outvec out_vec[] = {
        {.base = &usage, .len = sizeof(psa_key_usage_t)},
    };

    /* The PSA API does not return an error, so ignore any error from TF-M */
    status = API_DISPATCH(tfm_crypto_key_policy_get_usage);

    return usage;
}

psa_algorithm_t psa_key_policy_get_algorithm(const psa_key_policy_t *policy)
{
    psa_status_t status;
    psa_algorithm_t alg;

    /* Initialise to a sensible default to avoid returning an uninitialised
     * value in case the secure function fails.
     */
    alg = 0;

    psa_invec in_vec[] = {
        {.base = policy, .len = sizeof(psa_key_policy_t)},
    };
    psa_outvec out_vec[] = {
        {.base = &alg, .len = sizeof(psa_algorithm_t)},
    };

    /* The PSA API does not return an error, so ignore any error from TF-M */
    status = API_DISPATCH(tfm_crypto_key_policy_get_algorithm);

    return alg;
}

psa_status_t psa_set_key_policy(psa_key_slot_t key,
                                const psa_key_policy_t *policy)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = &key, .len = sizeof(psa_key_slot_t)},
        {.base = policy, .len = sizeof(psa_key_policy_t)},
    };

    status = API_DISPATCH_NO_OUTVEC(tfm_crypto_set_key_policy);

    return status;
}

psa_status_t psa_get_key_policy(psa_key_slot_t key,
                                psa_key_policy_t *policy)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = &key, .len = sizeof(psa_key_slot_t)},
    };
    psa_outvec out_vec[] = {
        {.base = policy, .len = sizeof(psa_key_policy_t)},
    };

    status = API_DISPATCH(tfm_crypto_get_key_policy);

    return status;
}

psa_status_t psa_set_key_lifetime(psa_key_slot_t key,
                                  psa_key_lifetime_t lifetime)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = &key, .len = sizeof(psa_key_slot_t)},
        {.base = &lifetime, .len = sizeof(psa_key_lifetime_t)},
    };

    status = API_DISPATCH_NO_OUTVEC(tfm_crypto_set_key_lifetime);

    return status;
}

psa_status_t psa_get_key_lifetime(psa_key_slot_t key,
                                  psa_key_lifetime_t *lifetime)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = &key, .len = sizeof(psa_key_slot_t)},
    };
    psa_outvec out_vec[] = {
        {.base = lifetime, .len = sizeof(psa_key_lifetime_t)},
    };

    status = API_DISPATCH(tfm_crypto_get_key_lifetime);

    return status;
}

psa_status_t psa_cipher_set_iv(psa_cipher_operation_t *operation,
                               const unsigned char *iv,
                               size_t iv_length)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = iv, .len = iv_length},
    };
    psa_outvec out_vec[] = {
        {.base = operation, .len = sizeof(psa_cipher_operation_t)},
    };

    status = API_DISPATCH(tfm_crypto_cipher_set_iv);

    return status;
}

psa_status_t psa_cipher_encrypt_setup(psa_cipher_operation_t *operation,
                                      psa_key_slot_t key,
                                      psa_algorithm_t alg)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = &key, .len = sizeof(psa_key_slot_t)},
        {.base = &alg, .len = sizeof(psa_algorithm_t)},
    };
    psa_outvec out_vec[] = {
        {.base = operation, .len = sizeof(psa_cipher_operation_t)},
    };

    status = API_DISPATCH(tfm_crypto_cipher_encrypt_setup);

    return status;
}

psa_status_t psa_cipher_decrypt_setup(psa_cipher_operation_t *operation,
                                      psa_key_slot_t key,
                                      psa_algorithm_t alg)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = &key, .len = sizeof(psa_key_slot_t)},
        {.base = &alg, .len = sizeof(psa_algorithm_t)},
    };
    psa_outvec out_vec[] = {
        {.base = operation, .len = sizeof(psa_cipher_operation_t)},
    };

    status = API_DISPATCH(tfm_crypto_cipher_decrypt_setup);

    return status;
}

psa_status_t psa_cipher_update(psa_cipher_operation_t *operation,
                               const uint8_t *input,
                               size_t input_length,
                               unsigned char *output,
                               size_t output_size,
                               size_t *output_length)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = input, .len = input_length},
    };
    psa_outvec out_vec[] = {
        {.base = operation, .len = sizeof(psa_cipher_operation_t)},
        {.base = output, .len = output_size}
    };

    status = API_DISPATCH(tfm_crypto_cipher_update);

    *output_length = out_vec[1].len;

    return status;
}

psa_status_t psa_cipher_abort(psa_cipher_operation_t *operation)
{
    psa_status_t status;
    psa_outvec out_vec[] = {
        {.base = operation, .len = sizeof(psa_cipher_operation_t)},
    };

    status = API_DISPATCH_NO_INVEC(tfm_crypto_cipher_abort);

    return status;
}

psa_status_t psa_cipher_finish(psa_cipher_operation_t *operation,
                               uint8_t *output,
                               size_t output_size,
                               size_t *output_length)
{
    psa_status_t status;
    psa_outvec out_vec[] = {
        {.base = operation, .len = sizeof(psa_cipher_operation_t)},
        {.base = output, .len = output_size},
    };

    status = API_DISPATCH_NO_INVEC(tfm_crypto_cipher_finish);

    *output_length = out_vec[1].len;

    return status;
}

psa_status_t psa_hash_setup(psa_hash_operation_t *operation,
                            psa_algorithm_t alg)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = &alg, .len = sizeof(psa_algorithm_t)},
    };
    psa_outvec out_vec[] = {
        {.base = operation, .len = sizeof(psa_hash_operation_t)},
    };

    status = API_DISPATCH(tfm_crypto_hash_setup);

    return status;
}

psa_status_t psa_hash_update(psa_hash_operation_t *operation,
                             const uint8_t *input,
                             size_t input_length)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = input, .len = input_length},
    };
    psa_outvec out_vec[] = {
        {.base = operation, .len = sizeof(psa_hash_operation_t)},
    };

    status = API_DISPATCH(tfm_crypto_hash_update);

    return status;
}

psa_status_t psa_hash_finish(psa_hash_operation_t *operation,
                             uint8_t *hash,
                             size_t hash_size,
                             size_t *hash_length)
{
    psa_status_t status;
    psa_outvec out_vec[] = {
        {.base = operation, .len = sizeof(psa_hash_operation_t)},
        {.base = hash, .len = hash_size},
    };

    status = API_DISPATCH_NO_INVEC(tfm_crypto_hash_finish);

    *hash_length = out_vec[1].len;

    return status;
}

psa_status_t psa_hash_verify(psa_hash_operation_t *operation,
                             const uint8_t *hash,
                             size_t hash_length)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = hash, .len = hash_length},
    };
    psa_outvec out_vec[] = {
        {.base = operation, .len = sizeof(psa_hash_operation_t)},
    };

    status = API_DISPATCH(tfm_crypto_hash_verify);

    return status;
}

psa_status_t psa_hash_abort(psa_hash_operation_t *operation)
{
    psa_status_t status;
    psa_outvec out_vec[] = {
        {.base = operation, .len = sizeof(psa_hash_operation_t)},
    };

    status = API_DISPATCH_NO_INVEC(tfm_crypto_hash_abort);

    return status;
}

psa_status_t psa_mac_sign_setup(psa_mac_operation_t *operation,
                                psa_key_slot_t key,
                                psa_algorithm_t alg)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = &key, .len = sizeof(psa_key_slot_t)},
        {.base = &alg, .len = sizeof(psa_algorithm_t)}
    };
    psa_outvec out_vec[] = {
        {.base = operation, .len = sizeof(psa_mac_operation_t)},
    };

    status = API_DISPATCH(tfm_crypto_mac_sign_setup);

    return status;
}

psa_status_t psa_mac_verify_setup(psa_mac_operation_t *operation,
                                  psa_key_slot_t key,
                                  psa_algorithm_t alg)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = &key, .len = sizeof(psa_key_slot_t)},
        {.base = &alg, .len = sizeof(psa_algorithm_t)}
    };
    psa_outvec out_vec[] = {
        {.base = operation, .len = sizeof(psa_mac_operation_t)},
    };

    status = API_DISPATCH(tfm_crypto_mac_verify_setup);

    return status;
}

psa_status_t psa_mac_update(psa_mac_operation_t *operation,
                            const uint8_t *input,
                            size_t input_length)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = input, .len = input_length},
    };
    psa_outvec out_vec[] = {
        {.base = operation, .len = sizeof(psa_mac_operation_t)},
    };

    status = API_DISPATCH(tfm_crypto_mac_update);

    return status;
}

psa_status_t psa_mac_sign_finish(psa_mac_operation_t *operation,
                                 uint8_t *mac,
                                 size_t mac_size,
                                 size_t *mac_length)
{
    psa_status_t status;
    psa_outvec out_vec[] = {
        {.base = operation, .len = sizeof(psa_mac_operation_t)},
        {.base = mac, .len = mac_size},
    };

    status = API_DISPATCH_NO_INVEC(tfm_crypto_mac_sign_finish);

    *mac_length = out_vec[1].len;

    return status;
}

psa_status_t psa_mac_verify_finish(psa_mac_operation_t *operation,
                                   const uint8_t *mac,
                                   size_t mac_length)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = mac, .len = mac_length},
    };
    psa_outvec out_vec[] = {
        {.base = operation, .len = sizeof(psa_mac_operation_t)},
    };

    status = API_DISPATCH(tfm_crypto_mac_verify_finish);

    return status;
}

psa_status_t psa_mac_abort(psa_mac_operation_t *operation)
{
    psa_status_t status;
    psa_outvec out_vec[] = {
        {.base = operation, .len = sizeof(psa_mac_operation_t)},
    };

    status = API_DISPATCH_NO_INVEC(tfm_crypto_mac_abort);

    return status;
}

psa_status_t psa_aead_encrypt(psa_key_slot_t key,
                              psa_algorithm_t alg,
                              const uint8_t *nonce,
                              size_t nonce_length,
                              const uint8_t *additional_data,
                              size_t additional_data_length,
                              const uint8_t *plaintext,
                              size_t plaintext_length,
                              uint8_t *ciphertext,
                              size_t ciphertext_size,
                              size_t *ciphertext_length)
{
    psa_status_t status;
    struct tfm_crypto_aead_pack_input input_s = {
      .key = key,
      .alg = alg,
      .nonce = {0},
    };
    size_t idx = 0;
    psa_invec in_vec[] = {
        {.base = &input_s, .len = nonce_length + sizeof(psa_key_slot_t)
                                               + sizeof(psa_algorithm_t)},
        {.base = additional_data, .len = additional_data_length},
        {.base = plaintext, .len = plaintext_length},
    };
    psa_outvec out_vec[] = {
        {.base = ciphertext, .len = ciphertext_size},
    };

    if (nonce_length > TFM_CRYPTO_MAX_NONCE_LENGTH) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (nonce != NULL) {
        for (idx = 0; idx < nonce_length; idx++) {
            input_s.nonce[idx] = nonce[idx];
        }
    }

    status = API_DISPATCH(tfm_crypto_aead_encrypt);

    *ciphertext_length = out_vec[0].len;

    return status;
}

psa_status_t psa_aead_decrypt(psa_key_slot_t key,
                              psa_algorithm_t alg,
                              const uint8_t *nonce,
                              size_t nonce_length,
                              const uint8_t *additional_data,
                              size_t additional_data_length,
                              const uint8_t *ciphertext,
                              size_t ciphertext_length,
                              uint8_t *plaintext,
                              size_t plaintext_size,
                              size_t *plaintext_length)
{
    psa_status_t status;
    struct tfm_crypto_aead_pack_input input_s = {
      .key = key,
      .alg = alg,
      .nonce = {0},
    };
    size_t idx = 0;
    psa_invec in_vec[] = {
        {.base = &input_s, .len = nonce_length + sizeof(psa_key_slot_t)
                                               + sizeof(psa_algorithm_t)},
        {.base = additional_data, .len = additional_data_length},
        {.base = ciphertext, .len = ciphertext_length},
    };
    psa_outvec out_vec[] = {
        {.base = plaintext, .len = plaintext_size},
    };

    if (nonce_length > TFM_CRYPTO_MAX_NONCE_LENGTH) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (nonce != NULL) {
        for (idx = 0; idx < nonce_length; idx++) {
            input_s.nonce[idx] = nonce[idx];
        }
    }

    status = API_DISPATCH(tfm_crypto_aead_decrypt);

    *plaintext_length = out_vec[0].len;

    return status;
}
