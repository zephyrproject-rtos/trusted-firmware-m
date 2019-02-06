/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "secure_fw/core/tfm_memory_utils.h"
#include "tfm_crypto_defs.h"

#include "psa_crypto.h"

#include "tfm_crypto_struct.h"

#include "tfm_crypto_api.h"
#include "crypto_utils.h"

/**
 * \def UNUSED_VAR
 *
 * \brief an UNUSED_VAR() macro for better code readability
 */
#define UNUSED_VAR(x) (void)x

/**
 * \def CRYPTO_HMAC_MAX_KEY_LENGTH
 *
 * \brief Specifies the maximum key length supported by the
 *        HMAC operations in this implementation
 */
#ifndef CRYPTO_HMAC_MAX_KEY_LENGTH
#define CRYPTO_HMAC_MAX_KEY_LENGTH (32)
#endif

static void mac_zeroize(void *data, size_t size)
{
    tfm_memset(data, 0, size);
}

static size_t get_hash_block_size(psa_algorithm_t alg)
{
    switch (alg) {
    case PSA_ALG_MD2:
        return 16;
    case PSA_ALG_MD4:
        return 64;
    case PSA_ALG_MD5:
        return 64;
    case PSA_ALG_RIPEMD160:
        return 64;
    case PSA_ALG_SHA_1:
        return 64;
    case PSA_ALG_SHA_224:
        return 64;
    case PSA_ALG_SHA_256:
        return 64;
    case PSA_ALG_SHA_384:
        return 128;
    case PSA_ALG_SHA_512:
        return 128;
    default:
        return 0;
    }
}

static enum tfm_crypto_err_t tfm_crypto_mac_release(
                                                psa_mac_operation_t *operation,
                                                struct tfm_mac_operation_s *ctx)
{
    enum tfm_crypto_err_t err;

    /* No release necessary on the ctx related quantites for the time being */
    UNUSED_VAR(ctx);

    /* Release the operation context */
    err = tfm_crypto_operation_release(TFM_CRYPTO_MAC_OPERATION, operation);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

static enum tfm_crypto_err_t tfm_crypto_hmac_setup(
                                          struct tfm_mac_operation_s *ctx,
                                          psa_key_slot_t key,
                                          psa_algorithm_t alg)
{
    enum tfm_crypto_err_t err;
    psa_key_type_t key_type;
    size_t key_size;
    uint8_t key_data[CRYPTO_HMAC_MAX_KEY_LENGTH];
    uint8_t hashed_key[PSA_HMAC_MAX_HASH_BLOCK_SIZE];
    size_t block_size;
    uint8_t ipad[PSA_HMAC_MAX_HASH_BLOCK_SIZE];
    uint8_t *opad = ctx->ctx.hmac.opad;
    size_t i;
    psa_key_usage_t usage;

    /* Check provided key */
    err = tfm_crypto_get_key_information(key, &key_type, &key_size);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    if (key_type != PSA_KEY_TYPE_HMAC){
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Set the key usage based on whether this is a sign or verify operation */
    if ((ctx->key_usage_sign == 1) && (ctx->key_usage_verify == 0)) {
        usage = PSA_KEY_USAGE_SIGN;
    } else if ((ctx->key_usage_sign == 0) && (ctx->key_usage_verify == 1)) {
        usage = PSA_KEY_USAGE_VERIFY;
    } else {
        return TFM_CRYPTO_ERR_PSA_ERROR_BAD_STATE;
    }

    /* Get the key data to start the HMAC */
    err = tfm_crypto_get_key(key,
                             usage,
                             alg,
                             key_data,
                             CRYPTO_HMAC_MAX_KEY_LENGTH,
                             &key_size);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* Bind the digest size to the MAC operation */
    ctx->mac_size = PSA_HASH_SIZE(PSA_ALG_HMAC_HASH(alg));

    block_size = get_hash_block_size(PSA_ALG_HMAC_HASH(alg));

    /* The HMAC algorithm is the standard procedure as described in
     * RFC-2104 (https://tools.ietf.org/html/rfc2104)
     */
    if (key_size > block_size) {
        /* Hash the key to reduce it to block size */
        err = tfm_crypto_hash_setup(&(ctx->ctx.hmac.hash_operation),
                                    PSA_ALG_HMAC_HASH(alg));
        if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
            return err;
        }

        err = tfm_crypto_hash_update(&(ctx->ctx.hmac.hash_operation),
                                     &key_data[0],
                                     key_size);
        if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
            return err;
        }

        /* Replace the key with the hashed key */
        err = tfm_crypto_hash_finish(&(ctx->ctx.hmac.hash_operation),
                                     hashed_key, sizeof(hashed_key),
                                     &key_size);
        if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
            return err;
        }
    } else {
        /* Copy the key inside the hashed_key buffer */
        for (i=0; i<key_size; i++) {
            hashed_key[i] = key_data[i];
        }
    }

    /* Create ipad = hashed_key XOR 0x36 and opad = hashed_key XOR 0x5C */
    for (i=0; i<key_size; i++) {
        ipad[i] = hashed_key[i] ^ 0x36;
        opad[i] = hashed_key[i] ^ 0x5C;
    }
    /* Fill ipad and opad to match block size */
    for (i=key_size; i<block_size; i++) {
        ipad[i] = 0x36;
        opad[i] = 0x5C;
    }

    /* Start hash1 = H(i_key_pad || message) */
    err = tfm_crypto_hash_setup(&(ctx->ctx.hmac.hash_operation),
                                PSA_ALG_HMAC_HASH(alg));
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        /* Clear key information on stack */
        for (i=0; i<key_size; i++) {
            hashed_key[i] = 0;
            ipad[i] = 0;
        }
        return err;
    }

    err = tfm_crypto_hash_update(&(ctx->ctx.hmac.hash_operation),
                                 ipad,
                                 block_size);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

static enum tfm_crypto_err_t tfm_crypto_mac_setup(psa_mac_operation_t *operation,
                                                  psa_key_slot_t key,
                                                  psa_algorithm_t alg,
                                                  uint8_t sign_operation)
{
    enum tfm_crypto_err_t err;

    struct tfm_mac_operation_s *ctx = NULL;

    if (!PSA_ALG_IS_MAC(alg)) {
        return TFM_CRYPTO_ERR_PSA_ERROR_NOT_SUPPORTED;
    }

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_mac_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Allocate the operation context in the secure world */
    err = tfm_crypto_operation_alloc(TFM_CRYPTO_MAC_OPERATION,
                                     operation,
                                     (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* Bind the algorithm to the mac operation */
    ctx->alg = alg;

    /* Specify if this will be used for a sign or verify operation */
    if (sign_operation) {
        ctx->key_usage_verify = 0;
        ctx->key_usage_sign = 1;
    } else {
        ctx->key_usage_verify = 1;
        ctx->key_usage_sign = 0;
    }

    if (PSA_ALG_IS_HMAC(alg)) {
        err = tfm_crypto_hmac_setup(ctx, key, alg);
        if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
            /* Release the operation context */
            (void)tfm_crypto_mac_release(operation, ctx);
            return err;
        }

        ctx->key_set = 1;
    } else {
        /* Other MAC types constructions are not supported */
        /* Release the operation context */
        (void)tfm_crypto_mac_release(operation, ctx);
        return TFM_CRYPTO_ERR_PSA_ERROR_NOT_SUPPORTED;
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

static enum tfm_crypto_err_t tfm_crypto_mac_finish(
                                                psa_mac_operation_t *operation,
                                                struct tfm_mac_operation_s *ctx,
                                                uint8_t *mac,
                                                size_t mac_size,
                                                size_t *mac_length)
{
    enum tfm_crypto_err_t err;
    uint8_t hash1[PSA_HASH_MAX_SIZE];
    size_t hash_size;
    uint8_t *opad;
    size_t block_size;

    /* Sanity checks */
    if (mac_size < ctx->mac_size) {
        (void)tfm_crypto_mac_release(operation, ctx);
        return TFM_CRYPTO_ERR_PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (!(ctx->has_input)) {
        (void)tfm_crypto_mac_release(operation, ctx);
        return TFM_CRYPTO_ERR_PSA_ERROR_BAD_STATE;
    }

    if (PSA_ALG_IS_HMAC(ctx->alg)) {
        opad = ctx->ctx.hmac.opad;
        block_size = get_hash_block_size(PSA_ALG_HMAC_HASH(ctx->alg));

        /* finish the hash1 = H(ipad || message) */
        err = tfm_crypto_hash_finish(&(ctx->ctx.hmac.hash_operation),
                                     hash1,
                                     sizeof(hash1),
                                     &hash_size);
        if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
            (void)tfm_crypto_mac_release(operation, ctx);
            return err;
        }

        /* compute the final mac value = H(opad || hash1) */
        err = tfm_crypto_hash_setup(&(ctx->ctx.hmac.hash_operation),
                                    PSA_ALG_HMAC_HASH(ctx->alg));
        if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
            mac_zeroize(hash1, sizeof(hash1));
            (void)tfm_crypto_mac_release(operation, ctx);
            return err;
        }

        err = tfm_crypto_hash_update(&(ctx->ctx.hmac.hash_operation),
                                     opad,
                                     block_size);
        if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
            mac_zeroize(hash1, sizeof(hash1));
            (void)tfm_crypto_mac_release(operation, ctx);
            return err;
        }

        err = tfm_crypto_hash_update(&(ctx->ctx.hmac.hash_operation),
                                     hash1,
                                     hash_size);
        if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
            mac_zeroize(hash1, sizeof(hash1));
            (void)tfm_crypto_mac_release(operation, ctx);
            return err;
        }

        err = tfm_crypto_hash_finish(&(ctx->ctx.hmac.hash_operation),
                                     mac,
                                     mac_size,
                                     mac_length);
        if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
            mac_zeroize(hash1, sizeof(hash1));
            (void)tfm_crypto_mac_release(operation, ctx);
            return err;
        }

        /* Clear intermediate hash value */
        mac_zeroize(hash1, sizeof(hash1));

    } else {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    return tfm_crypto_mac_release(operation, ctx);
}

/*!
 * \defgroup public_psa Public functions, PSA
 *
 */

/*!@{*/
enum tfm_crypto_err_t tfm_crypto_mac_sign_setup(psa_mac_operation_t *operation,
                                                psa_key_slot_t key,
                                                psa_algorithm_t alg)
{
    return tfm_crypto_mac_setup(operation, key, alg, 1);
}

enum tfm_crypto_err_t tfm_crypto_mac_verify_setup(
                                                psa_mac_operation_t *operation,
                                                psa_key_slot_t key,
                                                psa_algorithm_t alg)
{
    return tfm_crypto_mac_setup(operation, key, alg, 0);
}

enum tfm_crypto_err_t tfm_crypto_mac_update(psa_mac_operation_t *operation,
                                            const uint8_t *input,
                                            size_t input_length)
{
    enum tfm_crypto_err_t err;

    struct tfm_mac_operation_s *ctx = NULL;

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_mac_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    err = tfm_crypto_memory_check((void *)input,
                                  input_length,
                                  TFM_MEMORY_ACCESS_RO);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Look up the corresponding operation context */
    err = tfm_crypto_operation_lookup(TFM_CRYPTO_MAC_OPERATION,
                                      operation,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* Sanity check */
    if (!(ctx->key_set)) {
        (void)tfm_crypto_mac_release(operation, ctx);
        return TFM_CRYPTO_ERR_PSA_ERROR_BAD_STATE;
    }
    if (input_length == 0) {
        (void)tfm_crypto_mac_release(operation, ctx);
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Process the input chunk */
    if (PSA_ALG_IS_HMAC(ctx->alg)) {
        err = tfm_crypto_hash_update(&(ctx->ctx.hmac.hash_operation),
                                     input,
                                     input_length);
        if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
            (void)tfm_crypto_mac_release(operation, ctx);
            return err;
        }

        /* Set this flag to avoid HMAC without data */
        ctx->has_input = 1;
    } else {
        (void)tfm_crypto_mac_release(operation, ctx);
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_mac_sign_finish(psa_mac_operation_t *operation,
                                                 uint8_t *mac,
                                                 size_t mac_size,
                                                 size_t *mac_length)
{
    enum tfm_crypto_err_t err;
    struct tfm_mac_operation_s *ctx = NULL;

    if (mac_size == 0) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_mac_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    err = tfm_crypto_memory_check((void *)mac,
                                  mac_size,
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    err = tfm_crypto_memory_check(mac_length,
                                  sizeof(size_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Look up the corresponding operation context */
    err = tfm_crypto_operation_lookup(TFM_CRYPTO_MAC_OPERATION,
                                      operation,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    if ((ctx->key_usage_sign == 1) && (ctx->key_usage_verify == 0)) {
        /* Finalise the mac operation */
        err = tfm_crypto_mac_finish(operation, ctx, mac, mac_size, mac_length);
        if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
            return err;
        }
        /* A call to tfm_crypto_mac_finish() always releases the operation */

    } else {
        (void)tfm_crypto_mac_release(operation, ctx);
        return TFM_CRYPTO_ERR_PSA_ERROR_BAD_STATE;
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_mac_verify_finish(
                                                 psa_mac_operation_t *operation,
                                                 const uint8_t *mac,
                                                 size_t mac_length)
{
    enum tfm_crypto_err_t err;
    struct tfm_mac_operation_s *ctx = NULL;
    uint8_t computed_mac[PSA_HMAC_MAX_HASH_BLOCK_SIZE];
    size_t computed_mac_length;
    size_t i;
    uint32_t comp_mismatch = 0;

    if (mac_length == 0) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_mac_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    err = tfm_crypto_memory_check((void *)mac,
                                  mac_length,
                                  TFM_MEMORY_ACCESS_RO);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Look up the corresponding operation context */
    err = tfm_crypto_operation_lookup(TFM_CRYPTO_MAC_OPERATION,
                                      operation,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    if ((ctx->key_usage_sign == 0) && (ctx->key_usage_verify == 1)) {
        /* Finalise the mac operation */
        err = tfm_crypto_mac_finish(operation,
                                    ctx,
                                    computed_mac,
                                    sizeof(computed_mac),
                                    &computed_mac_length);
        if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
            return err;
        }
        /* A call to tfm_crypto_mac_finish() always releases the operation */

        /* Check that the computed mac match the expected one */
        if (computed_mac_length != mac_length) {
            return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_SIGNATURE;
        }

        for (i=0; i<computed_mac_length ; i++) {
            if (computed_mac[i] != mac[i]) {
                comp_mismatch = 1;
            }
        }

        if (comp_mismatch == 1) {
            return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_SIGNATURE;
        }
    } else {
        (void)tfm_crypto_mac_release(operation, ctx);
        return TFM_CRYPTO_ERR_PSA_ERROR_BAD_STATE;
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_mac_abort(psa_mac_operation_t *operation)
{
    enum tfm_crypto_err_t err;
    struct tfm_mac_operation_s *ctx = NULL;

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_mac_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Look up the corresponding operation context */
    err = tfm_crypto_operation_lookup(TFM_CRYPTO_MAC_OPERATION,
                                      operation,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    if (PSA_ALG_IS_HMAC(ctx->alg)){
        /* Check if the HMAC internal context needs to be deallocated */
        if (ctx->ctx.hmac.hash_operation.handle != TFM_CRYPTO_INVALID_HANDLE) {
            /* Clear hash context */
            err = tfm_crypto_hash_abort(&(ctx->ctx.hmac.hash_operation));
            if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
                return err;
            }
        }
    } else {
        /* MACs other than HMACs not currently supported */
        return TFM_CRYPTO_ERR_PSA_ERROR_NOT_SUPPORTED;
    }

    return tfm_crypto_mac_release(operation, ctx);
}
/*!@}*/
