/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_crypto_veneers.h"
#include "secure_fw/services/crypto/tfm_crypto_api.h"
#include "tfm_secure_api.h"
#include "tfm_api.h"
#include "spm_api.h"

#include "psa_crypto.h"

#include "crypto_psa_wrappers.h"

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_import_key(psa_key_slot_t key,
                                                   psa_key_type_t type,
                                                   const uint8_t *data,
                                                   size_t data_length)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_import_key,
                         key, type, data, data_length);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_destroy_key(psa_key_slot_t key)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_destroy_key,
                         key, 0, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_get_key_information(
                                                           psa_key_slot_t key,
                                                           psa_key_type_t *type,
                                                           size_t *bits)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_get_key_information,
                         key, type, bits, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_export_key(psa_key_slot_t key,
                                                   uint8_t *data,
                                                   size_t data_size,
                                                   size_t *data_length)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_export_key,
                         key, data, data_size, data_length);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_key_policy_init(
                                                       psa_key_policy_t *policy)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_key_policy_init,
                         policy, 0, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_key_policy_set_usage(
                                                       psa_key_policy_t *policy,
                                                       psa_key_usage_t usage,
                                                       psa_algorithm_t alg)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_key_policy_set_usage,
                         policy, usage, alg, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_key_policy_get_usage(
                                                 const psa_key_policy_t *policy,
                                                 psa_key_usage_t *usage)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_key_policy_get_usage,
                         policy, usage, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_key_policy_get_algorithm(
                                                 const psa_key_policy_t *policy,
                                                 psa_algorithm_t *alg)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_key_policy_get_algorithm,
                         policy, alg, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_set_key_policy(
                                                 psa_key_slot_t key,
                                                 const psa_key_policy_t *policy)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_set_key_policy,
                         key, policy, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_get_key_policy(psa_key_slot_t key,
                                                       psa_key_policy_t *policy)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_get_key_policy,
                         key, policy, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_set_key_lifetime(
                                                    psa_key_slot_t key,
                                                    psa_key_lifetime_t lifetime)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_set_key_lifetime,
                         key, lifetime, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_get_key_lifetime(
                                                   psa_key_slot_t key,
                                                   psa_key_lifetime_t *lifetime)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_get_key_lifetime,
                         key, lifetime, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_cipher_set_iv(
                                              psa_cipher_operation_t *operation,
                                              const unsigned char *iv,
                                              size_t iv_length)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_cipher_set_iv,
                         operation, iv, iv_length, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_cipher_encrypt_setup(
                                              psa_cipher_operation_t *operation,
                                              psa_key_slot_t key,
                                              psa_algorithm_t alg)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_cipher_encrypt_setup,
                         operation, key, alg, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_cipher_decrypt_setup(
                                              psa_cipher_operation_t *operation,
                                              psa_key_slot_t key,
                                              psa_algorithm_t alg)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_cipher_decrypt_setup,
                         operation, key, alg, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_cipher_update(
                                      psa_cipher_operation_t *operation,
                                      struct psa_cipher_update_input *input_s,
                                      struct psa_cipher_update_output *output_s)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_cipher_update_wrapper,
                         operation, input_s, output_s, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_cipher_abort(
                                              psa_cipher_operation_t *operation)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_cipher_abort,
                         operation, 0, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_cipher_finish(
                                              psa_cipher_operation_t *operation,
                                              uint8_t *output,
                                              size_t output_size,
                                              size_t *output_length)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_cipher_finish,
                         operation, output, output_size, output_length);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_hash_setup(
                                                psa_hash_operation_t *operation,
                                                psa_algorithm_t alg)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_hash_setup,
                         operation, alg, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_hash_update(
                                                psa_hash_operation_t *operation,
                                                const uint8_t *input,
                                                size_t input_length)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_hash_update,
                         operation, input, input_length, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_hash_finish(
                                                psa_hash_operation_t *operation,
                                                uint8_t *hash,
                                                size_t hash_size,
                                                size_t *hash_length)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_hash_finish,
                         operation, hash, hash_size, hash_length);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_hash_verify(
                                                psa_hash_operation_t *operation,
                                                const uint8_t *hash,
                                                size_t hash_length)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_hash_verify,
                         operation, hash, hash_length, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_hash_abort(
                                                psa_hash_operation_t *operation)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_hash_abort,
                         operation, 0, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_mac_sign_setup(
                                                 psa_mac_operation_t *operation,
                                                 psa_key_slot_t key,
                                                 psa_algorithm_t alg)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_mac_sign_setup,
                         operation, key, alg, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_mac_verify_setup(
                                                 psa_mac_operation_t *operation,
                                                 psa_key_slot_t key,
                                                 psa_algorithm_t alg)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_mac_verify_setup,
                         operation, key, alg, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_mac_update(
                                                 psa_mac_operation_t *operation,
                                                 const uint8_t *input,
                                                 size_t input_length)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_mac_update,
                         operation, input, input_length, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_mac_sign_finish(
                                                 psa_mac_operation_t *operation,
                                                 uint8_t *mac,
                                                 size_t mac_size,
                                                 size_t *mac_length)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_mac_sign_finish,
                         operation, mac, mac_size, mac_length);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_mac_verify_finish(
                                                 psa_mac_operation_t *operation,
                                                 const uint8_t *mac,
                                                 size_t mac_length)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_mac_verify_finish,
                         operation, mac, mac_length, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_mac_abort(
                                                 psa_mac_operation_t *operation)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_mac_abort,
                         operation, 0, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_aead_encrypt(
                                      struct psa_aead_encrypt_input *input_s,
                                      struct psa_aead_encrypt_output *output_s)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_aead_encrypt_wrapper,
                         input_s, output_s, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_crypto_err_t tfm_crypto_veneer_aead_decrypt(
                                      struct psa_aead_decrypt_input *input_s,
                                      struct psa_aead_decrypt_output *output_s)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_CRYPTO_ID, tfm_crypto_aead_decrypt_wrapper,
                         input_s, output_s, 0, 0);
}
