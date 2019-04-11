/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_veneers.h"
#include "tfm_crypto_defs.h"
#include "psa_crypto.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

#ifdef TFM_PSA_API
#include "psa_client.h"

/* Macro to check for a valid PSA handle */
/* FixMe: Here temporarily until it's added to the framework headers */
#define PSA_IS_HANDLE_VALID(handle) ((handle) > (psa_handle_t)0)

#define PSA_CONNECT(service)                                \
    psa_handle_t handle;                                    \
    handle = psa_connect(service##_SID, service##_MIN_VER); \
    if (!PSA_IS_HANDLE_VALID(handle)) {                     \
        return PSA_ERROR_UNKNOWN_ERROR;                     \
    }                                                       \

#define PSA_CLOSE() psa_close(handle)

#define API_DISPATCH(sfn_name, sfn_id)                     \
    psa_call(handle, /*PSA_IPC_CALL,*/                     \
        in_vec, ARRAY_SIZE(in_vec),                        \
        out_vec, ARRAY_SIZE(out_vec))

#define API_DISPATCH_NO_OUTVEC(sfn_name, sfn_id)           \
    psa_call(handle, /*PSA_IPC_CALL,*/                     \
        in_vec, ARRAY_SIZE(in_vec),                        \
        (psa_outvec *)NULL, 0)
#else
#define API_DISPATCH(sfn_name, sfn_id)               \
    tfm_##sfn_name##_veneer(                         \
        in_vec, ARRAY_SIZE(in_vec),                  \
        out_vec, ARRAY_SIZE(out_vec))

#define API_DISPATCH_NO_OUTVEC(sfn_name, sfn_id)     \
    tfm_##sfn_name##_veneer(                         \
        in_vec, ARRAY_SIZE(in_vec),                  \
        NULL, 0)
#endif /* TFM_PSA_API */

__attribute__(( section("SFN")))
psa_status_t psa_crypto_init(void)
{
    /* Service init is performed during TFM boot up,
     * so application level initialisation is empty
     */
    return PSA_SUCCESS;
}

__attribute__(( section("SFN")))
psa_status_t psa_import_key(psa_key_slot_t key,
                            psa_key_type_t type,
                            const uint8_t *data,
                            size_t data_length)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_IMPORT_KEY_SFID,
        .key = key,
        .type = type,
    };
    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = data, .len = data_length}
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH_NO_OUTVEC(tfm_crypto_import_key,
                                    TFM_CRYPTO_IMPORT_KEY);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_destroy_key(psa_key_slot_t key)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_DESTROY_KEY_SFID,
        .key = key,
    };
    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH_NO_OUTVEC(tfm_crypto_destroy_key,
                                    TFM_CRYPTO_DESTROY_KEY);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_get_key_information(psa_key_slot_t key,
                                     psa_key_type_t *type,
                                     size_t *bits)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_GET_KEY_INFORMATION_SFID,
        .key = key,
    };
    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = type, .len = sizeof(psa_key_type_t)},
        {.base = bits, .len = sizeof(size_t)}
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_get_key_information,
                          TFM_CRYPTO_GET_KEY_INFORMATION);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_export_key(psa_key_slot_t key,
                            uint8_t *data,
                            size_t data_size,
                            size_t *data_length)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_EXPORT_KEY_SFID,
        .key = key,
    };
    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = data, .len = data_size}
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_export_key,
                          TFM_CRYPTO_EXPORT_KEY);

    *data_length = out_vec[0].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
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

__attribute__(( section("SFN")))
void psa_key_policy_init(psa_key_policy_t *policy)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_KEY_POLICY_INIT_SFID,
    };
    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = policy, .len = sizeof(psa_key_policy_t)},
    };

#ifdef TFM_PSA_API
    psa_handle_t handle;
    handle = psa_connect(TFM_CRYPTO_SID,
                         TFM_CRYPTO_MIN_VER);
    if (!PSA_IS_HANDLE_VALID(handle)) {
        return;
    }
#endif

    /* PSA API returns void so just ignore error value returned */
    status = API_DISPATCH(tfm_crypto_key_policy_init,
                          TFM_CRYPTO_KEY_POLICY_INIT);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif
}

__attribute__(( section("SFN")))
void psa_key_policy_set_usage(psa_key_policy_t *policy,
                              psa_key_usage_t usage,
                              psa_algorithm_t alg)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_KEY_POLICY_SET_USAGE_SFID,
        .usage = usage,
        .alg = alg,
    };
    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = policy, .len = sizeof(psa_key_policy_t)},
    };

#ifdef TFM_PSA_API
    psa_handle_t handle;
    handle = psa_connect(TFM_CRYPTO_SID,
                         TFM_CRYPTO_MIN_VER);
    if (!PSA_IS_HANDLE_VALID(handle)) {
        return;
    }
#endif

    /* PSA API returns void so just ignore error value returned */
    status = API_DISPATCH(tfm_crypto_key_policy_set_usage,
                          TFM_CRYPTO_KEY_POLICY_SET_USAGE);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif
}

__attribute__(( section("SFN")))
psa_key_usage_t psa_key_policy_get_usage(const psa_key_policy_t *policy)
{
    psa_status_t status;
    psa_key_usage_t usage;

    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_KEY_POLICY_GET_USAGE_SFID,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = policy, .len = sizeof(psa_key_policy_t)},
    };
    psa_outvec out_vec[] = {
        {.base = &usage, .len = sizeof(psa_key_usage_t)},
    };

    /* Initialise to a sensible default to avoid returning an uninitialised
     * value in case the secure function fails.
     */
    usage = 0;

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    /* The PSA API does not return an error, so ignore any error from TF-M */
    status = API_DISPATCH(tfm_crypto_key_policy_get_usage,
                          TFM_CRYPTO_KEY_POLICY_GET_USAGE);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return usage;
}

__attribute__(( section("SFN")))
psa_algorithm_t psa_key_policy_get_algorithm(const psa_key_policy_t *policy)
{
    psa_status_t status;
    psa_algorithm_t alg;

    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_KEY_POLICY_GET_ALGORITHM_SFID,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = policy, .len = sizeof(psa_key_policy_t)},
    };
    psa_outvec out_vec[] = {
        {.base = &alg, .len = sizeof(psa_algorithm_t)},
    };

    /* Initialise to a sensible default to avoid returning an uninitialised
     * value in case the secure function fails.
     */
    alg = 0;

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    /* The PSA API does not return an error, so ignore any error from TF-M */
    status = API_DISPATCH(tfm_crypto_key_policy_get_algorithm,
                          TFM_CRYPTO_KEY_POLICY_GET_ALGORITHM);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return alg;
}

__attribute__(( section("SFN")))
psa_status_t psa_set_key_policy(psa_key_slot_t key,
                                const psa_key_policy_t *policy)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_SET_KEY_POLICY_SFID,
        .key = key,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = policy, .len = sizeof(psa_key_policy_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH_NO_OUTVEC(tfm_crypto_set_key_policy,
                                    TFM_CRYPTO_SET_KEY_POLICY);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_get_key_policy(psa_key_slot_t key,
                                psa_key_policy_t *policy)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_GET_KEY_POLICY_SFID,
        .key = key,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = policy, .len = sizeof(psa_key_policy_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_get_key_policy,
                          TFM_CRYPTO_GET_KEY_POLICY);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_set_key_lifetime(psa_key_slot_t key,
                                  psa_key_lifetime_t lifetime)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_SET_KEY_LIFETIME_SFID,
        .key = key,
        .lifetime = lifetime,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH_NO_OUTVEC(tfm_crypto_set_key_lifetime,
                                    TFM_CRYPTO_SET_KEY_LIFETIME);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_get_key_lifetime(psa_key_slot_t key,
                                  psa_key_lifetime_t *lifetime)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_GET_KEY_LIFETIME_SFID,
        .key = key,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = lifetime, .len = sizeof(psa_key_lifetime_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_get_key_lifetime,
                          TFM_CRYPTO_GET_KEY_LIFETIME);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_cipher_set_iv(psa_cipher_operation_t *operation,
                               const unsigned char *iv,
                               size_t iv_length)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_CIPHER_SET_IV_SFID,
        .handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = iv, .len = iv_length},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_cipher_set_iv,
                          TFM_CRYPTO_CIPHER_SET_IV);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_cipher_encrypt_setup(psa_cipher_operation_t *operation,
                                      psa_key_slot_t key,
                                      psa_algorithm_t alg)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_CIPHER_ENCRYPT_SETUP_SFID,
        .key = key,
        .alg = alg,
        .handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_cipher_encrypt_setup,
                          TFM_CRYPTO_CIPHER_ENCRYPT_SETUP);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_cipher_decrypt_setup(psa_cipher_operation_t *operation,
                                      psa_key_slot_t key,
                                      psa_algorithm_t alg)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_CIPHER_DECRYPT_SETUP_SFID,
        .key = key,
        .alg = alg,
        .handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_cipher_decrypt_setup,
                          TFM_CRYPTO_CIPHER_DECRYPT_SETUP);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_cipher_update(psa_cipher_operation_t *operation,
                               const uint8_t *input,
                               size_t input_length,
                               unsigned char *output,
                               size_t output_size,
                               size_t *output_length)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_CIPHER_UPDATE_SFID,
        .handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = input, .len = input_length},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
        {.base = output, .len = output_size}
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_cipher_update,
                          TFM_CRYPTO_CIPHER_UPDATE);

    *output_length = out_vec[1].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_cipher_abort(psa_cipher_operation_t *operation)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_CIPHER_ABORT_SFID,
        .handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_cipher_abort,
                          TFM_CRYPTO_CIPHER_ABORT);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_cipher_finish(psa_cipher_operation_t *operation,
                               uint8_t *output,
                               size_t output_size,
                               size_t *output_length)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_CIPHER_FINISH_SFID,
        .handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
        {.base = output, .len = output_size},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_cipher_finish,
                          TFM_CRYPTO_CIPHER_FINISH);

    *output_length = out_vec[1].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_hash_setup(psa_hash_operation_t *operation,
                            psa_algorithm_t alg)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_HASH_SETUP_SFID,
        .alg = alg,
        .handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_hash_setup,
                          TFM_CRYPTO_HASH_SETUP);

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_hash_update(psa_hash_operation_t *operation,
                             const uint8_t *input,
                             size_t input_length)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_HASH_UPDATE_SFID,
        .handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = input, .len = input_length},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_hash_update,
                          TFM_CRYPTO_HASH_UPDATE);

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_hash_finish(psa_hash_operation_t *operation,
                             uint8_t *hash,
                             size_t hash_size,
                             size_t *hash_length)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_HASH_FINISH_SFID,
        .handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
        {.base = hash, .len = hash_size},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_hash_finish,
                          TFM_CRYPTO_HASH_FINISH);

    *hash_length = out_vec[1].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_hash_verify(psa_hash_operation_t *operation,
                             const uint8_t *hash,
                             size_t hash_length)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_HASH_VERIFY_SFID,
        .handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = hash, .len = hash_length},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_hash_verify,
                          TFM_CRYPTO_HASH_VERIFY);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_hash_abort(psa_hash_operation_t *operation)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_HASH_ABORT_SFID,
        .handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_hash_abort,
                          TFM_CRYPTO_HASH_ABORT);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_mac_sign_setup(psa_mac_operation_t *operation,
                                psa_key_slot_t key,
                                psa_algorithm_t alg)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_MAC_SIGN_SETUP_SFID,
        .key = key,
        .alg = alg,
        .handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_mac_sign_setup,
                          TFM_CRYPTO_MAC_SIGN_SETUP);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_mac_verify_setup(psa_mac_operation_t *operation,
                                  psa_key_slot_t key,
                                  psa_algorithm_t alg)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_MAC_VERIFY_SETUP_SFID,
        .key = key,
        .alg = alg,
        .handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_mac_verify_setup,
                          TFM_CRYPTO_MAC_VERIFY_SETUP);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_mac_update(psa_mac_operation_t *operation,
                            const uint8_t *input,
                            size_t input_length)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_MAC_UPDATE_SFID,
        .handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = input, .len = input_length},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_mac_update,
                          TFM_CRYPTO_MAC_UPDATE);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_mac_sign_finish(psa_mac_operation_t *operation,
                                 uint8_t *mac,
                                 size_t mac_size,
                                 size_t *mac_length)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_MAC_SIGN_FINISH_SFID,
        .handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
        {.base = mac, .len = mac_size},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_mac_sign_finish,
                          TFM_CRYPTO_MAC_SIGN_FINISH);

    *mac_length = out_vec[1].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_mac_verify_finish(psa_mac_operation_t *operation,
                                   const uint8_t *mac,
                                   size_t mac_length)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_MAC_VERIFY_FINISH_SFID,
        .handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = mac, .len = mac_length},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_mac_verify_finish,
                          TFM_CRYPTO_MAC_VERIFY_FINISH);

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
psa_status_t psa_mac_abort(psa_mac_operation_t *operation)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_MAC_ABORT_SFID,
        .handle = operation->handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = &(operation->handle), .len = sizeof(uint32_t)},
    };

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

    status = API_DISPATCH(tfm_crypto_mac_abort,
                          TFM_CRYPTO_MAC_ABORT);
#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
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
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_AEAD_ENCRYPT_SFID,
        .key = key,
        .alg = alg,
        .aead_in = {.nonce = {0}, .nonce_length = nonce_length}
    };

    size_t idx = 0;
    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = plaintext, .len = plaintext_length},
        {.base = additional_data, .len = additional_data_length},
    };
    psa_outvec out_vec[] = {
        {.base = ciphertext, .len = ciphertext_size},
    };

    if (nonce_length > TFM_CRYPTO_MAX_NONCE_LENGTH) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (nonce != NULL) {
        for (idx = 0; idx < nonce_length; idx++) {
            iov.aead_in.nonce[idx] = nonce[idx];
        }
    }

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

#ifdef TFM_PSA_API
    size_t in_len = sizeof(in_vec)/sizeof(in_vec[0]);
    if (additional_data == NULL) {
      in_len--;
    }
    status = psa_call(handle, in_vec, in_len,
                      out_vec, sizeof(out_vec)/sizeof(out_vec[0]));
#else
    status = API_DISPATCH(tfm_crypto_aead_encrypt,
                          TFM_CRYPTO_AEAD_ENCRYPT);
#endif

    *ciphertext_length = out_vec[0].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}

__attribute__(( section("SFN")))
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
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_AEAD_DECRYPT_SFID,
        .key = key,
        .alg = alg,
        .aead_in = {.nonce = {0}, .nonce_length = nonce_length}
    };

    size_t idx = 0;
    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = ciphertext, .len = ciphertext_length},
        {.base = additional_data, .len = additional_data_length},
    };
    psa_outvec out_vec[] = {
        {.base = plaintext, .len = plaintext_size},
    };

    if (nonce_length > TFM_CRYPTO_MAX_NONCE_LENGTH) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (nonce != NULL) {
        for (idx = 0; idx < nonce_length; idx++) {
            iov.aead_in.nonce[idx] = nonce[idx];
        }
    }

#ifdef TFM_PSA_API
    PSA_CONNECT(TFM_CRYPTO);
#endif

#ifdef TFM_PSA_API
    size_t in_len = sizeof(in_vec)/sizeof(in_vec[0]);
    if (additional_data == NULL) {
      in_len--;
    }
    status = psa_call(handle, in_vec, in_len,
                      out_vec, sizeof(out_vec)/sizeof(out_vec[0]));
#else
    status = API_DISPATCH(tfm_crypto_aead_decrypt,
                          TFM_CRYPTO_AEAD_DECRYPT);
#endif

    *plaintext_length = out_vec[0].len;

#ifdef TFM_PSA_API
    PSA_CLOSE();
#endif

    return status;
}
