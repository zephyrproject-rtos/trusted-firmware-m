/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "secure_fw/core/tfm_memory_utils.h"
#include "tfm_crypto_api.h"
#include "crypto_engine.h"
#include "tfm_crypto_struct.h"

/* FixMe: Use PSA_CONNECTION_REFUSED when performing parameter
 *        integrity checks but this will have to be revised
 *        when the full set of error codes mandated by PSA FF
 *        is available.
 */

static psa_status_t _psa_get_key_information(psa_key_slot_t key,
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

    status = tfm_crypto_get_key_information(
                 in_vec, sizeof(in_vec)/sizeof(in_vec[0]),
                 out_vec, sizeof(out_vec)/sizeof(out_vec[0]));

    return status;
}

static psa_status_t _psa_hash_setup(uint32_t *handle,
                                    psa_algorithm_t alg)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_HASH_SETUP_SFID,
        .alg = alg,
        .handle = *handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = handle, .len = sizeof(uint32_t)},
    };

    status = tfm_crypto_hash_setup(in_vec, sizeof(in_vec)/sizeof(in_vec[0]),
                 out_vec, sizeof(out_vec)/sizeof(out_vec[0]));

    return status;
}

static psa_status_t _psa_hash_update(uint32_t *handle,
                                     const uint8_t *input,
                                     size_t input_length)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_HASH_UPDATE_SFID,
        .handle = *handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
        {.base = input, .len = input_length},
    };
    psa_outvec out_vec[] = {
        {.base = handle, .len = sizeof(uint32_t)},
    };

    status = tfm_crypto_hash_update(in_vec, sizeof(in_vec)/sizeof(in_vec[0]),
                 out_vec, sizeof(out_vec)/sizeof(out_vec[0]));

    return status;
}

static psa_status_t _psa_hash_finish(uint32_t *handle,
                                     uint8_t *hash,
                                     size_t hash_size,
                                     size_t *hash_length)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_HASH_FINISH_SFID,
        .handle = *handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = handle, .len = sizeof(uint32_t)},
        {.base = hash, .len = hash_size},
    };

    status = tfm_crypto_hash_finish(in_vec, sizeof(in_vec)/sizeof(in_vec[0]),
                   out_vec, sizeof(out_vec)/sizeof(out_vec[0]));
    *hash_length = out_vec[1].len;

    return status;
}

static psa_status_t _psa_hash_abort(uint32_t *handle)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_HASH_ABORT_SFID,
        .handle = *handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = handle, .len = sizeof(uint32_t)},
    };

    status = tfm_crypto_hash_abort(in_vec, sizeof(in_vec)/sizeof(in_vec[0]),
                 out_vec, sizeof(out_vec)/sizeof(out_vec[0]));

    return status;
}

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
    (void)tfm_memset(data, 0, size);
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

static psa_status_t tfm_crypto_mac_release(uint32_t *handle,
                                           struct tfm_mac_operation_s *ctx)
{
    /* No release necessary on the ctx related items for the time being */
    UNUSED_VAR(ctx);

    /* Release the operation context */
    return tfm_crypto_operation_release(handle);
}

static psa_status_t tfm_crypto_hmac_setup(struct tfm_mac_operation_s *ctx,
                                          psa_key_slot_t key,
                                          psa_algorithm_t alg)
{
    psa_status_t status = PSA_SUCCESS;
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
    status = _psa_get_key_information(key, &key_type, &key_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if (key_type != PSA_KEY_TYPE_HMAC){
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Set the key usage based on whether this is a sign or verify operation */
    if ((ctx->key_usage_sign == 1) && (ctx->key_usage_verify == 0)) {
        usage = PSA_KEY_USAGE_SIGN;
    } else if ((ctx->key_usage_sign == 0) && (ctx->key_usage_verify == 1)) {
        usage = PSA_KEY_USAGE_VERIFY;
    } else {
        return PSA_ERROR_BAD_STATE;
    }

    /* Get the key data to start the HMAC */
    status = tfm_crypto_get_key(key,
                                usage,
                                alg,
                                key_data,
                                CRYPTO_HMAC_MAX_KEY_LENGTH,
                                &key_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Bind the digest size to the MAC operation */
    ctx->mac_size = PSA_HASH_SIZE(PSA_ALG_HMAC_HASH(alg));

    block_size = get_hash_block_size(PSA_ALG_HMAC_HASH(alg));

    /* The HMAC algorithm is the standard procedure as described in
     * RFC-2104 (https://tools.ietf.org/html/rfc2104)
     */
    if (key_size > block_size) {
        /* Hash the key to reduce it to block size */
        status = _psa_hash_setup(&(ctx->ctx.hmac.hash_operation.handle),
                                 PSA_ALG_HMAC_HASH(alg));
        if (status != PSA_SUCCESS) {
            return status;
        }

        status = _psa_hash_update(&(ctx->ctx.hmac.hash_operation.handle),
                                  &key_data[0],
                                  key_size);
        if (status != PSA_SUCCESS) {
            return status;
        }

        /* Replace the key with the hashed key */
        status = _psa_hash_finish(&(ctx->ctx.hmac.hash_operation.handle),
                                  hashed_key,
                                  sizeof(hashed_key),
                                  &key_size);
        if (status != PSA_SUCCESS) {
            return status;
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
    status = _psa_hash_setup(&(ctx->ctx.hmac.hash_operation.handle),
                             PSA_ALG_HMAC_HASH(alg));
    if (status != PSA_SUCCESS) {
        /* Clear key information on stack */
        for (i=0; i<key_size; i++) {
            hashed_key[i] = 0;
            ipad[i] = 0;
        }
        return status;
    }

    status = _psa_hash_update(&(ctx->ctx.hmac.hash_operation.handle),
                              ipad,
                              block_size);
    return status;
}

static psa_status_t tfm_crypto_mac_setup(uint32_t *handle,
                                         psa_key_slot_t key,
                                         psa_algorithm_t alg,
                                         uint8_t sign_operation)
{
    psa_status_t status = PSA_SUCCESS;
    struct tfm_mac_operation_s *ctx = NULL;

    if (!PSA_ALG_IS_MAC(alg)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* Allocate the operation context in the secure world */
    status = tfm_crypto_operation_alloc(TFM_CRYPTO_MAC_OPERATION,
                                        handle,
                                        (void **)&ctx);
    if (status != PSA_SUCCESS) {
        return status;
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
        status = tfm_crypto_hmac_setup(ctx, key, alg);
        if (status != PSA_SUCCESS) {
            /* Release the operation context */
            (void)tfm_crypto_mac_release(handle, ctx);
            return status;
        }

        ctx->key_set = 1;
    } else {
        /* Other MAC types constructions are not supported */
        /* Release the operation context */
      (void)tfm_crypto_mac_release(handle, ctx);
        return PSA_ERROR_NOT_SUPPORTED;
    }

    return PSA_SUCCESS;
}

static psa_status_t tfm_crypto_mac_finish(uint32_t *handle,
                                          struct tfm_mac_operation_s *ctx,
                                          uint8_t *mac,
                                          size_t mac_size,
                                          size_t *mac_length)
{
    psa_status_t status = PSA_SUCCESS;
    uint8_t hash1[PSA_HASH_MAX_SIZE];
    size_t hash_size;
    uint8_t *opad;
    size_t block_size;

    /* Sanity checks */
    if (mac_size < ctx->mac_size) {
      (void)tfm_crypto_mac_release(handle, ctx);
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (!(ctx->has_input)) {
        (void)tfm_crypto_mac_release(handle, ctx);
        return PSA_ERROR_BAD_STATE;
    }

    if (PSA_ALG_IS_HMAC(ctx->alg)) {
        opad = ctx->ctx.hmac.opad;
        block_size = get_hash_block_size(PSA_ALG_HMAC_HASH(ctx->alg));

        /* finish the hash1 = H(ipad || message) */
        status = _psa_hash_finish(&(ctx->ctx.hmac.hash_operation.handle),
                                  hash1,
                                  sizeof(hash1),
                                  &hash_size);
        if (status != PSA_SUCCESS) {
            (void)tfm_crypto_mac_release(handle, ctx);
            return status;
        }

        /* compute the final mac value = H(opad || hash1) */
        status = _psa_hash_setup(&(ctx->ctx.hmac.hash_operation.handle),
                                 PSA_ALG_HMAC_HASH(ctx->alg));
        if (status != PSA_SUCCESS) {
            mac_zeroize(hash1, sizeof(hash1));
            (void)tfm_crypto_mac_release(handle, ctx);
            return status;
        }

        status = _psa_hash_update(&(ctx->ctx.hmac.hash_operation.handle),
                                  opad,
                                  block_size);
        if (status != PSA_SUCCESS) {
            mac_zeroize(hash1, sizeof(hash1));
            (void)tfm_crypto_mac_release(handle, ctx);
            return status;
        }

        status = _psa_hash_update(&(ctx->ctx.hmac.hash_operation.handle),
                                  hash1,
                                  hash_size);
        if (status != PSA_SUCCESS) {
            mac_zeroize(hash1, sizeof(hash1));
            (void)tfm_crypto_mac_release(handle, ctx);
            return status;
        }

        status = _psa_hash_finish(&(ctx->ctx.hmac.hash_operation.handle),
                                  mac,
                                  mac_size,
                                  mac_length);
        if (status != PSA_SUCCESS) {
            mac_zeroize(hash1, sizeof(hash1));
            (void)tfm_crypto_mac_release(handle, ctx);
            return status;
        }

        /* Clear intermediate hash value */
        mac_zeroize(hash1, sizeof(hash1));

    } else {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return tfm_crypto_mac_release(handle, ctx);
}

/*!
 * \defgroup public_psa Public functions, PSA
 *
 */

/*!@{*/
psa_status_t tfm_crypto_mac_sign_setup(psa_invec in_vec[],
                                       size_t in_len,
                                       psa_outvec out_vec[],
                                       size_t out_len)
{
    psa_status_t status = PSA_SUCCESS;
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((out_vec[0].len != sizeof(uint32_t)) ||
        (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->handle;
    uint32_t *handle_out = out_vec[0].base;
    psa_key_slot_t key = iov->key;
    psa_algorithm_t alg = iov->alg;

    status = tfm_crypto_mac_setup(&handle, key, alg, 1);
    if (status == PSA_SUCCESS) {
        *handle_out = handle;
    } else {
        *handle_out = iov->handle;
    }
    return status;
}

psa_status_t tfm_crypto_mac_verify_setup(psa_invec in_vec[],
                                         size_t in_len,
                                         psa_outvec out_vec[],
                                         size_t out_len)
{
    psa_status_t status = PSA_SUCCESS;
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((out_vec[0].len != sizeof(uint32_t)) ||
        (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->handle;
    uint32_t *handle_out = out_vec[0].base;
    psa_key_slot_t key = iov->key;
    psa_algorithm_t alg = iov->alg;

    status = tfm_crypto_mac_setup(&handle, key, alg, 0);
    if (status == PSA_SUCCESS) {
        *handle_out = handle;
    } else {
        *handle_out = iov->handle;
    }
    return status;
}

psa_status_t tfm_crypto_mac_update(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
    psa_status_t status = PSA_SUCCESS;
    struct tfm_mac_operation_s *ctx = NULL;

    if ((in_len != 2) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->handle;
    uint32_t *handle_out = out_vec[0].base;
    const uint8_t *input = in_vec[1].base;
    size_t input_length = in_vec[1].len;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->handle;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_MAC_OPERATION,
                                         handle,
                                         (void **)&ctx);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Sanity check */
    if (!(ctx->key_set)) {
        if (tfm_crypto_mac_release(&handle, ctx) == PSA_SUCCESS) {
            *handle_out = handle;
        }
        return PSA_ERROR_BAD_STATE;
    }
    if (input_length == 0) {
        if (tfm_crypto_mac_release(&handle, ctx) == PSA_SUCCESS) {
            *handle_out = handle;
        }
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Process the input chunk */
    if (PSA_ALG_IS_HMAC(ctx->alg)) {
        status = _psa_hash_update(&(ctx->ctx.hmac.hash_operation.handle),
                                  input,
                                  input_length);
        if (status != PSA_SUCCESS) {
            if (tfm_crypto_mac_release(&handle, ctx) == PSA_SUCCESS) {
                *handle_out = handle;
            }
            return status;
        }

        /* Set this flag to avoid HMAC without data */
        ctx->has_input = 1;
    } else {
        if (tfm_crypto_mac_release(&handle, ctx) == PSA_SUCCESS) {
            *handle_out = handle;
        }
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_mac_sign_finish(psa_invec in_vec[],
                                        size_t in_len,
                                        psa_outvec out_vec[],
                                        size_t out_len)
{
    psa_status_t status = PSA_SUCCESS;
    struct tfm_mac_operation_s *ctx = NULL;

    if ((in_len != 1) || (out_len != 2)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->handle;
    uint32_t *handle_out = out_vec[0].base;
    uint8_t *mac = out_vec[1].base;
    size_t mac_size = out_vec[1].len;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->handle;

    /* Initialise mac_length to zero */
    out_vec[1].len = 0;

    if (mac_size == 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_MAC_OPERATION,
                                         handle,
                                         (void **)&ctx);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if ((ctx->key_usage_sign == 1) && (ctx->key_usage_verify == 0)) {
        /* Finalise the mac operation */
        status = tfm_crypto_mac_finish(&handle,
                       ctx, mac, mac_size, &(out_vec[1].len));
    } else {
        if (tfm_crypto_mac_release(&handle, ctx) == PSA_SUCCESS) {
            *handle_out = handle;
        }
        return PSA_ERROR_BAD_STATE;
    }

    *handle_out = handle;
    return status;
}

psa_status_t tfm_crypto_mac_verify_finish(psa_invec in_vec[],
                                          size_t in_len,
                                          psa_outvec out_vec[],
                                          size_t out_len)
{
    psa_status_t status = PSA_SUCCESS;
    struct tfm_mac_operation_s *ctx = NULL;
    uint8_t computed_mac[PSA_HMAC_MAX_HASH_BLOCK_SIZE];
    size_t computed_mac_length;
    size_t i;
    uint32_t comp_mismatch = 0;

    if ((in_len != 2) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->handle;
    uint32_t *handle_out = out_vec[0].base;
    const uint8_t *mac = in_vec[1].base;
    size_t mac_length = in_vec[1].len;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->handle;

    if (mac_length == 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_MAC_OPERATION,
                                         handle,
                                         (void **)&ctx);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if ((ctx->key_usage_sign == 0) && (ctx->key_usage_verify == 1)) {
        /* Finalise the mac operation */
        status = tfm_crypto_mac_finish(&handle,
                                       ctx,
                                       computed_mac,
                                       sizeof(computed_mac),
                                       &computed_mac_length);
        *handle_out = handle;
        if (status != PSA_SUCCESS) {
            return status;
        }

        /* Check that the computed mac match the expected one */
        if (computed_mac_length != mac_length) {
            return PSA_ERROR_INVALID_SIGNATURE;
        }

        for (i=0; i<computed_mac_length ; i++) {
            if (computed_mac[i] != mac[i]) {
                comp_mismatch = 1;
            }
        }

        if (comp_mismatch == 1) {
            return PSA_ERROR_INVALID_SIGNATURE;
        }
    } else {
        if (tfm_crypto_mac_release(&handle, ctx) == PSA_SUCCESS) {
            *handle_out = handle;
        }
        return PSA_ERROR_BAD_STATE;
    }

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_mac_abort(psa_invec in_vec[],
                                  size_t in_len,
                                  psa_outvec out_vec[],
                                  size_t out_len)
{
    psa_status_t status = PSA_SUCCESS;
    struct tfm_mac_operation_s *ctx = NULL;

    if ((in_len != 1) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->handle;
    uint32_t *handle_out = out_vec[0].base;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->handle;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_MAC_OPERATION,
                                         handle,
                                         (void **)&ctx);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if (PSA_ALG_IS_HMAC(ctx->alg)){
        /* Check if the HMAC internal context needs to be deallocated */
        if (ctx->ctx.hmac.hash_operation.handle != TFM_CRYPTO_INVALID_HANDLE) {
            /* Clear hash context */
            status = _psa_hash_abort(&(ctx->ctx.hmac.hash_operation.handle));
            if (status != PSA_SUCCESS) {
                return status;
            }
        }
    } else {
        /* MACs other than HMACs not currently supported */
        return PSA_ERROR_NOT_SUPPORTED;
    }

    status = tfm_crypto_mac_release(&handle, ctx);
    if (status == PSA_SUCCESS) {
        *handle_out = handle;
    }
    return status;
}
/*!@}*/
