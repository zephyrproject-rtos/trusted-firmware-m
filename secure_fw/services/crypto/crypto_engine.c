/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**
 * \brief Include the Crypto engine header. This source module implements the
 *        interface which is being used by the other components of the service
 *        to interact with the cryptography primitives available, in SW or HW.
 *        The current implementation provides only a SW implementation based on
 *        Mbed TLS functions.
 */
#include "crypto_engine.h"

/**
 * \brief Default value for the size of the static buffer used by the Engine
 *        module as a scratch buffer for its own internal allocations
 */
#ifndef TFM_CRYPTO_ENGINE_BUF_SIZE
#define TFM_CRYPTO_ENGINE_BUF_SIZE (1024)
#endif

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
/**
 * \brief Buffer size used by Mbed TLS for its allocations
 */
#define TFM_CRYPTO_MBEDTLS_MEM_BUF_LEN (TFM_CRYPTO_ENGINE_BUF_SIZE)

/**
 * \brief Static buffer to be used by Mbed TLS for memory allocations
 *
 */
static uint8_t mbedtls_mem_buf[TFM_CRYPTO_MBEDTLS_MEM_BUF_LEN] = {0};

/**
 * \brief Converts a PSA key type and key size to an Mbed TLS cipher ID.
 *
 * \param[in] key_type  PSA key type
 * \param[in] key_size  Key size in bits
 *
 * \return An mbedtls_cipher_id_t value corresponding to key_type and key_size,
 *         or MBEDTLS_CIPHER_ID_NONE if no such cipher ID is supported by
 *         Mbed TLS.
 */
static mbedtls_cipher_id_t psa_to_mbedtls_cipher_id(psa_key_type_t key_type,
                                                    size_t key_size)
{
    switch (key_type) {
    case PSA_KEY_TYPE_AES:
        return MBEDTLS_CIPHER_ID_AES;
    case PSA_KEY_TYPE_DES:
        return (key_size == 192) ? MBEDTLS_CIPHER_ID_3DES
                                 : MBEDTLS_CIPHER_ID_DES;
    case PSA_KEY_TYPE_CAMELLIA:
        return MBEDTLS_CIPHER_ID_CAMELLIA;
    case PSA_KEY_TYPE_ARC4:
        return MBEDTLS_CIPHER_ID_ARC4;
    default:
        return MBEDTLS_CIPHER_ID_NONE;
    }
}

/**
 * \brief Converts a PSA algorithm to an Mbed TLS cipher mode.
 *
 * \param[in] alg  PSA algorithm
 *
 * \return An mbedtls_cipher_mode_t value corresponding to alg, or
 *         MBEDTLS_MODE_NONE if no such cipher mode is supported by Mbed TLS.
 */
static mbedtls_cipher_mode_t psa_to_mbedtls_cipher_mode(psa_algorithm_t alg)
{
    if (PSA_ALG_IS_BLOCK_CIPHER(alg)) {
        /* Clear the padding mask */
        alg &= ~PSA_ALG_BLOCK_CIPHER_PADDING_MASK;
    }

    switch (alg) {
    case PSA_ALG_CBC_BASE:
        return MBEDTLS_MODE_CBC;
    case PSA_ALG_CFB_BASE:
        return MBEDTLS_MODE_CFB;
    case PSA_ALG_OFB_BASE:
        return MBEDTLS_MODE_OFB;
    case PSA_ALG_XTS_BASE:
        return MBEDTLS_MODE_NONE; /* FIXME: requires Mbed TLS 2.11 */
    case PSA_ALG_CTR:
        return MBEDTLS_MODE_CTR;
    case PSA_ALG_ARC4:
        return MBEDTLS_MODE_STREAM; /* ARC4 is a stream cipher */
    case PSA_ALG_CCM:
        return MBEDTLS_MODE_CCM;
    case PSA_ALG_GCM:
        return MBEDTLS_MODE_GCM;
    default:
        return MBEDTLS_MODE_NONE;
    }
}

/**
 * \brief Given a PSA key type, algorithm and key size, finds the corresponding
 *        Mbed TLS cipher info struct.
 *
 * \param[in] key_type  PSA key type
 * \param[in] alg       PSA algorithm
 * \param[in] key_size  Key size in bits
 *
 * \return A pointer to the mbedtls_cipher_info_t struct corresponding to
 *         key_type, alg and key_size, or NULL if Mbed TLS does not support this
 *         combination.
 */
static const mbedtls_cipher_info_t *get_mbedtls_cipher_info(
                                                        psa_key_type_t key_type,
                                                        psa_algorithm_t alg,
                                                        size_t key_size)
{
    mbedtls_cipher_id_t cipher_id;
    mbedtls_cipher_mode_t cipher_mode;

    /* Get the Mbed TLS cipher ID */
    cipher_id = psa_to_mbedtls_cipher_id(key_type, key_size);
    if (cipher_id == MBEDTLS_CIPHER_ID_NONE) {
        /* The requested key type is not supported by Mbed TLS */
        return NULL;
    }

    /* Get the Mbed TLS cipher mode */
    cipher_mode = psa_to_mbedtls_cipher_mode(alg);
    if (cipher_mode == MBEDTLS_MODE_NONE) {
        /* The requested algorithm is not supported by Mbed TLS */
        return NULL;
    }

    /* Get the Mbed TLS cipher info pointer */
    return mbedtls_cipher_info_from_values(cipher_id, key_size, cipher_mode);
}

/**
 * \brief Converts a PSA algorithm to an Mbed TLS message digest type.
 *
 * \param[in] alg  PSA algorithm
 *
 * \return An mbedtls_md_type_t value corresponding to alg, or
 *         MBEDTLS_MD_NONE if no such message digest is available.
 */
static mbedtls_md_type_t psa_to_mbedtls_md_type(psa_algorithm_t alg)
{
    mbedtls_md_type_t type = MBEDTLS_MD_NONE;

    /* Mbed TLS message digest setup */
    switch(alg) {
    case PSA_ALG_MD2:
#if defined(MBEDTLS_MD2_C)
        type = MBEDTLS_MD_MD2;
#endif
        break;
    case PSA_ALG_MD4:
#if defined(MBEDTLS_MD4_C)
        type = MBEDTLS_MD_MD4;
#endif
        break;
    case PSA_ALG_MD5:
#if defined(MBEDTLS_MD5_C)
        type = MBEDTLS_MD_MD5;
#endif
        break;
    case PSA_ALG_RIPEMD160:
#if defined(MBEDTLS_RIPEMD160_C)
        type = MBEDTLS_MD_RIPEMD160;
#endif
        break;
    case PSA_ALG_SHA_1:
#if defined(MBEDTLS_SHA1_C)
        type = MBEDTLS_MD_SHA1;
#endif
        break;
    case PSA_ALG_SHA_224:
#if defined(MBEDTLS_SHA256_C)
        type = MBEDTLS_MD_SHA224;
#endif
        break;
    case PSA_ALG_SHA_256:
#if defined(MBEDTLS_SHA256_C)
        type = MBEDTLS_MD_SHA256;
#endif
        break;
    case PSA_ALG_SHA_384:
#if defined(MBEDTLS_SHA512_C)
        type = MBEDTLS_MD_SHA384;
#endif
        break;
    case PSA_ALG_SHA_512:
#if defined(MBEDTLS_SHA512_C)
        type = MBEDTLS_MD_SHA512;
#endif
        break;
    case PSA_ALG_SHA3_224:
    case PSA_ALG_SHA3_256:
    case PSA_ALG_SHA3_384:
    case PSA_ALG_SHA3_512:
        /* SHA3 not yet supported */
        type = MBEDTLS_MD_NONE;
        break;
    case PSA_ALG_SHA_512_224:
    case PSA_ALG_SHA_512_256:
        /* SHA-512 Truncated modes not yet supported */
        type = MBEDTLS_MD_NONE;
        break;
    default:
        break;
    }

    return type;
}

/**
 * \brief This function maps Mbed TLS return codes to PSA return codes.
 *
 * \param[in] ret Mbed TLS return value
 *
 * \return Return values as specified by \ref psa_status_t
 */
static psa_status_t mbedtls_to_psa_return(int ret)
{
    /* zero return value means success */
    if (ret == 0) {
        return PSA_SUCCESS;
    }

    /* FIXME: Investigate all possible Mbed TLS errors and map them
     *        to the the correct corresponding PSA status
     */
    switch (ret) {
    case MBEDTLS_ERR_CIPHER_FULL_BLOCK_EXPECTED:
        return PSA_ERROR_INVALID_ARGUMENT;
    case MBEDTLS_ERR_CIPHER_AUTH_FAILED:
        return PSA_ERROR_INVALID_SIGNATURE;
    default:
        return PSA_ERROR_UNKNOWN_ERROR;
    }
}
#endif /* TFM_CRYPTO_ENGINE_MBEDTLS */

psa_status_t tfm_crypto_engine_init(void)
{
    psa_status_t return_value = PSA_ERROR_NOT_SUPPORTED;

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
    /* Initialise the Mbed TLS static memory allocator so that Mbed TLS
     * allocates memory from the provided static buffer instead of from
     * the heap.
     */
    mbedtls_memory_buffer_alloc_init(mbedtls_mem_buf,
                                     TFM_CRYPTO_MBEDTLS_MEM_BUF_LEN);
    /* The previous function doesn't return any error code */
    return_value = PSA_SUCCESS;
#endif /* TFM_CRYPTO_ENGINE_MBEDTLS */

    return return_value;
}

psa_status_t tfm_crypto_engine_hash_setup(const psa_algorithm_t alg,
                                          struct hash_engine_info *engine_info)
{
    psa_status_t return_value = PSA_ERROR_NOT_SUPPORTED;

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
    mbedtls_md_type_t type = MBEDTLS_MD_NONE;

    type = psa_to_mbedtls_md_type(alg);

    engine_info->type = (uint32_t) type;

    if (type == MBEDTLS_MD_NONE) {
        return_value = PSA_ERROR_NOT_SUPPORTED;
    } else {
        return_value = PSA_SUCCESS;
    }
#endif /* TFM_CRYPTO_ENGINE_MBEDTLS */

    return return_value;
}

psa_status_t tfm_crypto_engine_hash_start(
                                     union engine_hash_context *hp,
                                     const struct hash_engine_info *engine_info)
{
    psa_status_t return_value = PSA_ERROR_NOT_SUPPORTED;

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
    const mbedtls_md_info_t *info = NULL;
    int ret;

    /* Mbed TLS message digest init */
    mbedtls_md_init(&(hp->ctx));
    info = mbedtls_md_info_from_type((mbedtls_md_type_t)engine_info->type);
    ret = mbedtls_md_setup(&(hp->ctx), info, 0); /* 0: not HMAC */
    if (ret != 0) {
        return_value = mbedtls_to_psa_return(ret);
    } else {
        /* Start the message digest context */
        ret = mbedtls_md_starts(&(hp->ctx));
        return_value = mbedtls_to_psa_return(ret);
    }
#endif /* TFM_CRYPTO_ENGINE_MBEDTLS */

    return return_value;
}

psa_status_t tfm_crypto_engine_hash_update(union engine_hash_context *hp,
                                           const uint8_t *input,
                                           const uint32_t input_length)
{
    psa_status_t return_value = PSA_ERROR_NOT_SUPPORTED;

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
    int ret;
    /* Update the message digest with a new chunk of information */
    ret = mbedtls_md_update(&(hp->ctx), input, input_length);
    return_value = mbedtls_to_psa_return(ret);
#endif /* TFM_CRYPTO_ENGINE_MBEDTLS */

    return return_value;
}

psa_status_t tfm_crypto_engine_hash_finish(union engine_hash_context *hp,
                                           uint8_t *hash)
{
    psa_status_t return_value = PSA_ERROR_NOT_SUPPORTED;

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
    int ret;
    /* Finalise the hash value, producing the output */
    ret = mbedtls_md_finish(&(hp->ctx), hash);
    return_value = mbedtls_to_psa_return(ret);
#endif /* TFM_CRYPTO_ENGINE_MBEDTLS */

    return return_value;
}

psa_status_t tfm_crypto_engine_hash_release(union engine_hash_context *hp)
{
    psa_status_t return_value = PSA_ERROR_NOT_SUPPORTED;

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
    /* Clear the Mbed TLS message digest context */
    mbedtls_md_free(&(hp->ctx));
    /* The previous function doesn't return any error code */
    return_value = PSA_SUCCESS;
#endif /* TFM_CRYPTO_ENGINE_MBEDTLS */

    return return_value;
}

psa_status_t tfm_crypto_engine_cipher_setup(const psa_algorithm_t alg,
                             const psa_key_type_t key_type,
                             const uint32_t key_size,
                             const enum engine_cipher_mode_t engine_cipher_mode,
                             struct cipher_engine_info *engine_info)
{
    psa_status_t return_value = PSA_ERROR_NOT_SUPPORTED;

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
    const mbedtls_cipher_info_t *info;
    mbedtls_cipher_padding_t padding_mode = MBEDTLS_PADDING_NONE;
    psa_algorithm_t padding_mode_field = PSA_ALG_BLOCK_CIPHER_PAD_NONE;

    if ((engine_cipher_mode != ENGINE_CIPHER_MODE_ENCRYPT) &&
        (engine_cipher_mode != ENGINE_CIPHER_MODE_DECRYPT)) {
        return_value = PSA_ERROR_NOT_SUPPORTED;
    } else {
        /* Get the Mbed TLS cipher info from PSA key_type/alg/key_size. */
        info = get_mbedtls_cipher_info(key_type, alg, key_size);
        if (info == NULL) {
            /* The combination of key_type, alg and key_size is not a valid
             * Mbed TLS cipher configuration.
             */
            return_value = PSA_ERROR_NOT_SUPPORTED;
        } else {
            /* If CBC, need to check the padding mode */
            if ((alg & ~PSA_ALG_BLOCK_CIPHER_PADDING_MASK) == PSA_ALG_CBC_BASE){

                /* Check the value of padding field */
                padding_mode_field = alg & PSA_ALG_BLOCK_CIPHER_PADDING_MASK;

                switch (padding_mode_field) {
                case PSA_ALG_BLOCK_CIPHER_PAD_PKCS7:
                    padding_mode = MBEDTLS_PADDING_PKCS7;
                    return_value = PSA_SUCCESS;
                    break;
                case PSA_ALG_BLOCK_CIPHER_PAD_NONE:
                    padding_mode = MBEDTLS_PADDING_NONE;
                    return_value = PSA_SUCCESS;
                    break;
                default:
                    return_value = PSA_ERROR_NOT_SUPPORTED;
                }
            } else {
                /* The engine is correctly configured */
                return_value = PSA_SUCCESS;
            }
        }
    }

    if (return_value == PSA_SUCCESS) {
        engine_info->type = (uint32_t) (info->type);
        engine_info->cipher_mode = engine_cipher_mode;
        engine_info->padding_mode = (uint32_t) padding_mode;
    }
#endif /* TFM_CRYPTO_ENGINE_MBEDTLS */

    return return_value;
}

psa_status_t tfm_crypto_engine_cipher_set_padding_mode(
                                   union engine_cipher_context *cp,
                                   const struct cipher_engine_info *engine_info)
{
    psa_status_t return_value = PSA_ERROR_NOT_SUPPORTED;

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
    int ret;
    ret = mbedtls_cipher_set_padding_mode(&(cp->ctx),
                           (mbedtls_cipher_padding_t)engine_info->padding_mode);
    return_value = mbedtls_to_psa_return(ret);
#endif /* TFM_CRYPTO_ENGINE_MBEDTLS */

    return return_value;
}

psa_status_t tfm_crypto_engine_cipher_start(
                                   union engine_cipher_context *cp,
                                   const struct cipher_engine_info *engine_info)
{
    psa_status_t return_value = PSA_ERROR_NOT_SUPPORTED;

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
    int ret;
    const mbedtls_cipher_info_t *info = NULL;

    /* Mbed TLS cipher init */
    mbedtls_cipher_init(&(cp->ctx));
    info = mbedtls_cipher_info_from_type(engine_info->type);

    ret = mbedtls_cipher_setup(&(cp->ctx), info);
    return_value = mbedtls_to_psa_return(ret);
#endif /* TFM_CRYPTO_ENGINE_MBEDTLS */

    return return_value;
}

psa_status_t tfm_crypto_engine_cipher_set_key(
                                   union engine_cipher_context *cp,
                                   const uint8_t *key_data,
                                   const uint32_t key_size,
                                   const struct cipher_engine_info *engine_info)
{
    psa_status_t return_value = PSA_ERROR_NOT_SUPPORTED;

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
    int ret;
    /* Mbed TLS cipher set key */
    if ((engine_info->cipher_mode != ENGINE_CIPHER_MODE_ENCRYPT) &&
        (engine_info->cipher_mode != ENGINE_CIPHER_MODE_DECRYPT)) {
        return_value = PSA_ERROR_NOT_SUPPORTED;
    } else {
        /* Set the key on the context */
        ret = mbedtls_cipher_setkey(
                      &(cp->ctx),
                      key_data,
                      PSA_BYTES_TO_BITS(key_size),
                      (engine_info->cipher_mode == ENGINE_CIPHER_MODE_ENCRYPT) ?
                      MBEDTLS_ENCRYPT : MBEDTLS_DECRYPT);
        return_value = mbedtls_to_psa_return(ret);
    }
#endif /* TFM_CRYPTO_ENGINE_MBEDTLS */

    return return_value;
}

psa_status_t tfm_crypto_engine_cipher_set_iv(union engine_cipher_context *cp,
                                             const uint8_t *iv,
                                             const uint32_t iv_length)
{
    psa_status_t return_value = PSA_ERROR_NOT_SUPPORTED;

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
    int ret;

    /* Bind the IV to the cipher operation */
    ret = mbedtls_cipher_set_iv(&(cp->ctx), iv, iv_length);
    if (ret != 0) {
        return_value = mbedtls_to_psa_return(ret);
    } else {
        /* Reset the context after IV is set */
        ret = mbedtls_cipher_reset(&(cp->ctx));
        return_value = mbedtls_to_psa_return(ret);
    }
#endif /* TFM_CRYPTO_ENGINE_MBEDTLS */

    return return_value;
}

psa_status_t tfm_crypto_engine_cipher_update(union engine_cipher_context *cp,
                                             const uint8_t *input,
                                             const uint32_t input_length,
                                             uint8_t *output,
                                             uint32_t *output_length)
{
    psa_status_t return_value = PSA_ERROR_NOT_SUPPORTED;

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
    int ret;
    /* Update with the chunk of input data, eventually producing output */
    ret = mbedtls_cipher_update(&(cp->ctx), input, input_length,
                                output, (size_t *)output_length);
    return_value = mbedtls_to_psa_return(ret);
#endif /* TFM_CRYPTO_ENGINE_MBEDTLS */

    return return_value;
}

psa_status_t tfm_crypto_engine_cipher_finish(union engine_cipher_context *cp,
                                             uint8_t *output,
                                             uint32_t *output_length)
{
    psa_status_t return_value = PSA_ERROR_NOT_SUPPORTED;

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
    int ret;
    /* Finalise the cipher operation */
    ret = mbedtls_cipher_finish(&(cp->ctx), output, (size_t *)output_length);
    return_value = mbedtls_to_psa_return(ret);
#endif /* TFM_CRYPTO_ENGINE_MBEDTLS */

    return return_value;
}

psa_status_t tfm_crypto_engine_cipher_release(union engine_cipher_context *cp)
{
    psa_status_t return_value = PSA_ERROR_NOT_SUPPORTED;

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
    /* Clear the Mbed TLS context */
    mbedtls_cipher_free(&(cp->ctx));
    /* The previous function doesn't return any error code */
    return_value = PSA_SUCCESS;
#endif /* TFM_CRYPTO_ENGINE_MBEDTLS */

    return return_value;
}

psa_status_t tfm_crypto_engine_aead_encrypt(psa_key_type_t key_type,
                                            psa_algorithm_t alg,
                                            const uint8_t *key_data,
                                            uint32_t key_size,
                                            const uint8_t *nonce,
                                            uint32_t nonce_length,
                                            const uint8_t *additional_data,
                                            uint32_t additional_data_length,
                                            const uint8_t *plaintext,
                                            uint32_t plaintext_length,
                                            uint8_t *ciphertext,
                                            uint32_t ciphertext_size,
                                            uint32_t *ciphertext_length)
{
    psa_status_t return_value = PSA_ERROR_NOT_SUPPORTED;

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
    int ret;
    mbedtls_cipher_context_t aead_ctx;
    const uint32_t key_size_bits = PSA_BYTES_TO_BITS(key_size);

    const mbedtls_cipher_info_t *info = get_mbedtls_cipher_info(key_type,
                                                                alg,
                                                                key_size_bits);
    const uint32_t encrypted_message_length =
                           PSA_AEAD_ENCRYPT_OUTPUT_SIZE(alg, plaintext_length) -
                                                         PSA_AEAD_TAG_SIZE(alg);
    if (info == NULL) {
        /* The combination of key_type, alg and key_size is not a valid Mbed TLS
         * cipher configuration.
         */
        return_value = PSA_ERROR_NOT_SUPPORTED;
    } else {
        /* Init the mbedTLS context */
        mbedtls_cipher_init(&aead_ctx);

        /* Setup the mbedTLS context */
        ret = mbedtls_cipher_setup(&aead_ctx, info);
        if (ret != 0) {
            return_value = mbedtls_to_psa_return(ret);
        } else {
            /* Set the key on the Mbed TLS context */
            ret = mbedtls_cipher_setkey(&aead_ctx,
                                        key_data,
                                        key_size_bits,
                                        MBEDTLS_ENCRYPT);
            if (ret != 0) {
                return_value = mbedtls_to_psa_return(ret);
            } else {
                /* Perform the AEAD operation on the Mbed TLS context */
                ret = mbedtls_cipher_auth_encrypt(&aead_ctx,
                                                  nonce, nonce_length,
                                                  additional_data,
                                                  additional_data_length,
                                                  plaintext,
                                                  plaintext_length,
                                                  ciphertext,
                                                  (size_t *)ciphertext_length,
                                                  ciphertext +
                                                      encrypted_message_length,
                                                  PSA_AEAD_TAG_SIZE(alg));

                /* Clear the Mbed TLS context */
                mbedtls_cipher_free(&aead_ctx);
                return_value = mbedtls_to_psa_return(ret);
            }
        }
    }
#endif /* TFM_CRYPTO_ENGINE_MBEDTLS */

    return return_value;
}

psa_status_t tfm_crypto_engine_aead_decrypt(psa_key_type_t key_type,
                                            psa_algorithm_t alg,
                                            const uint8_t *key_data,
                                            uint32_t key_size,
                                            const uint8_t *nonce,
                                            uint32_t nonce_length,
                                            const uint8_t *additional_data,
                                            uint32_t additional_data_length,
                                            const uint8_t *ciphertext,
                                            uint32_t ciphertext_length,
                                            uint8_t *plaintext,
                                            uint32_t plaintext_size,
                                            uint32_t *plaintext_length)
{
    psa_status_t return_value = PSA_ERROR_NOT_SUPPORTED;

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
    int ret;
    mbedtls_cipher_context_t aead_ctx;
    const uint32_t key_size_bits = PSA_BYTES_TO_BITS(key_size);

    const mbedtls_cipher_info_t *info = get_mbedtls_cipher_info(key_type,
                                                                alg,
                                                                key_size_bits);

    const uint32_t encrypted_message_length =
                           PSA_AEAD_DECRYPT_OUTPUT_SIZE(alg, ciphertext_length);
    if (info == NULL) {
        /* The combination of key_type, alg and key_size is not a valid Mbed TLS
         * cipher configuration.
         */
        return_value = PSA_ERROR_NOT_SUPPORTED;
    } else {
        /* Init the mbedTLS context */
        mbedtls_cipher_init(&aead_ctx);

        /* Setup the mbedTLS context */
        ret = mbedtls_cipher_setup(&aead_ctx, info);
        if (ret != 0) {
            return_value = mbedtls_to_psa_return(ret);
        } else {
             /* Set the key on the Mbed TLS context */
            ret = mbedtls_cipher_setkey(&aead_ctx,
                                        key_data,
                                        key_size_bits,
                                        MBEDTLS_DECRYPT);
            if (ret != 0) {
                return_value = mbedtls_to_psa_return(ret);
            } else {
                /* Perform the AEAD operation on the Mbed TLS context */
                ret = mbedtls_cipher_auth_decrypt(&aead_ctx,
                                                  nonce, nonce_length,
                                                  additional_data,
                                                  additional_data_length,
                                                  ciphertext,
                                                  encrypted_message_length,
                                                  plaintext,
                                                  (size_t *)plaintext_length,
                                                  ciphertext +
                                                      encrypted_message_length,
                                                  PSA_AEAD_TAG_SIZE(alg));

                /* Clear the Mbed TLS context */
                mbedtls_cipher_free(&aead_ctx);
                return_value = mbedtls_to_psa_return(ret);
            }
        }
    }
#endif /* TFM_CRYPTO_ENGINE_MBEDTLS */

    return return_value;
}
