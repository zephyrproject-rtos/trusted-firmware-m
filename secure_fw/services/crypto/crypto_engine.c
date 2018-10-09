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

#if defined(TFM_CRYPTO_ENGINE_MBEDTLS)
/**
 * \brief Buffer size used by Mbed TLS for its allocations
 */
#define TFM_CRYPTO_MBEDTLS_MEM_BUF_LEN (1024)

/**
 * \brief Static buffer to be used by Mbed TLS for memory allocations
 *
 */
static uint8_t mbedtls_mem_buf[TFM_CRYPTO_MBEDTLS_MEM_BUF_LEN] = {0};

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

    /* FIXME: For the time being map all errors to PSA_ERROR_UNKNOW_ERROR */
    switch (ret) {
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
    default:
        break;
    }

    engine_info->type = (uint32_t) type;

    /* Mbed TLS currently does not support SHA3: PSA_ALG_SHA3_224,
     * PSA_ALG_SHA3_256, PSA_ALG_SHA3_384, PSA_ALG_SHA3_512;
     * Mbed TLS currently does not support SHA-512 Truncated modes:
     * PSA_ALG_SHA_512_224, PSA_ALG_SHA_512_256
     */
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
    mbedtls_cipher_type_t type = MBEDTLS_CIPHER_NONE;
    mbedtls_cipher_padding_t padding_mode = MBEDTLS_PADDING_NONE;
    psa_algorithm_t padding_mode_field = PSA_ALG_BLOCK_CIPHER_PAD_NONE;

    /* FIXME: Check that key is compatible with alg. It's assumed that key
     *        key_type will be AES, this needs to be extended as well. A
     *        proper function to determine cipher type based on inputs
     *        will need to be designed here.
     */
    if ((engine_cipher_mode != ENGINE_CIPHER_MODE_ENCRYPT) &&
        (engine_cipher_mode != ENGINE_CIPHER_MODE_DECRYPT)) {
        return_value = PSA_ERROR_NOT_SUPPORTED;
    } else {
        /* Mbed TLS cipher setup */
        if (key_size == 128) {
            if (alg == PSA_ALG_CBC_BASE) {
                type = MBEDTLS_CIPHER_AES_128_CBC;
            } else if (alg == PSA_ALG_CFB_BASE) {
                if (engine_cipher_mode == ENGINE_CIPHER_MODE_ENCRYPT) {
                   type = MBEDTLS_CIPHER_AES_128_CFB128;
                }
            }
        }

        if (type == MBEDTLS_CIPHER_NONE) {
            return_value = PSA_ERROR_NOT_SUPPORTED;
        } else {
            /* If CBC, need to set check the padding mode */
            if ((alg & ~PSA_ALG_BLOCK_CIPHER_PADDING_MASK) == PSA_ALG_CBC_BASE) {

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
        engine_info->type = (uint32_t) type;
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
