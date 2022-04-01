/*
 * Copyright (c) 2021-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/** \file cc3xx_psa_aead.c
 *
 * This file contains the implementations of the entry points associated to the
 * aead capability (single-part and multipart) as described by the PSA
 * Cryptoprocessor Driver interface specification.
 *
 */

#include "aes_driver.h"
#include "cc_pal_abort.h"
#include "cc_pal_mem.h"
#include "cc_pal_types.h"

#include "cc3xx_psa_aead.h"
#include "cc3xx_internal_ccm.h"
#include "cc3xx_internal_gcm.h"
#include "cc3xx_internal_chacha20.h"
#include "cc3xx_internal_chacha20_poly1305.h"

/**
 * \brief By default, the driver interface implements the single part API using
 *        the multipart functions thus enabling some saving in code size
 */
#ifndef CC3XX_CONFIG_AEAD_ONE_SHOT_USE_MULTIPART
#define CC3XX_CONFIG_AEAD_ONE_SHOT_USE_MULTIPART
#endif /* CC3XX_CONFIG_AEAD_ONE_SHOT_USE_MULTIPART */

/**
 * \brief By default, the driver interface enables Chacha20-Poly1305
 */
#ifndef CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305
#define CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305
#endif /* CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305 */

/**
 * \brief By default, the driver interface enables GCM
 */
#ifndef CC3XX_CONFIG_SUPPORT_GCM
#define CC3XX_CONFIG_SUPPORT_GCM
#endif /* CC3XX_CONFIG_SUPPORT_GCM */

/**
 * \brief By default, the driver interface enables CCM
 */
#ifndef CC3XX_CONFIG_SUPPORT_CCM
#define CC3XX_CONFIG_SUPPORT_CCM
#endif /* CC3XX_CONFIG_SUPPORT_GCM */

/* FixMe: The strategy to configure the CC3XX driver layer is not finalised,
 *        so for the time being we just use the mbed TLS config file to
 *        understand if we need to set CC3XX specific config defines.
 */
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

/* FixMe: Temporary way of bridging mbed TLS based configuration
 *        with specific CC3XX driver configuration defines
 */
#ifndef MBEDTLS_CHACHAPOLY_C
#undef CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305
#endif
#ifndef MBEDTLS_GCM_C
#undef CC3XX_CONFIG_SUPPORT_GCM
#endif
#ifndef MBEDTLS_CCM_C
#undef CC3XX_CONFIG_SUPPORT_CCM
#endif

/* Number of valid tag lengths sizes both for CCM and GCM modes */
#define VALID_TAG_LENGTH_SIZE 7

static psa_status_t check_alg(psa_algorithm_t alg, psa_algorithm_t *ref_alg)
{
    psa_algorithm_t default_alg = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg);
    size_t tag_length = PSA_ALG_AEAD_GET_TAG_LENGTH(alg);
    size_t valid_tag_lengths[VALID_TAG_LENGTH_SIZE];

    *ref_alg = default_alg;
    switch (default_alg) {
#if defined(CC3XX_CONFIG_SUPPORT_CCM)
    case PSA_ALG_CCM:
        valid_tag_lengths[0] = 4;
        valid_tag_lengths[1] = 6;
        valid_tag_lengths[2] = 8;
        valid_tag_lengths[3] = 10;
        valid_tag_lengths[4] = 12;
        valid_tag_lengths[5] = 14;
        valid_tag_lengths[6] = 16;
        break;
#endif /* CC3XX_CONFIG_SUPPORT_CCM */
#if defined(CC3XX_CONFIG_SUPPORT_GCM)
    case PSA_ALG_GCM:
        valid_tag_lengths[0] = 4;
        valid_tag_lengths[1] = 8;
        valid_tag_lengths[2] = 12;
        valid_tag_lengths[3] = 13;
        valid_tag_lengths[4] = 14;
        valid_tag_lengths[5] = 15;
        valid_tag_lengths[6] = 16;
        break;
#endif /* CC3XX_CONFIG_SUPPORT_GCM */
#if defined(CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305)
    case PSA_ALG_CHACHA20_POLY1305:
        if (tag_length != 16) {
            return PSA_ERROR_NOT_SUPPORTED;
        } else {
            return PSA_SUCCESS;
        }
        break;
#endif /* CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305 */
    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* Cycle through all valid tag lengths for CCM or GCM */
    uint32_t i;
    for (i = 0; i < VALID_TAG_LENGTH_SIZE; i++) {
        if (tag_length == valid_tag_lengths[i]) {
            break;
        }
    }

    if (i == VALID_TAG_LENGTH_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return PSA_SUCCESS;
}

static psa_status_t aead_setup(
        cc3xx_aead_operation_t *operation,
        const psa_key_attributes_t *attributes,
        const uint8_t *key, size_t key_length,
        psa_algorithm_t alg,
        psa_encrypt_or_decrypt_t dir)
{
    psa_status_t ret = PSA_ERROR_CORRUPTION_DETECTED;
    psa_algorithm_t ref_alg;
    psa_key_type_t key_type = psa_get_key_type(attributes);
    size_t key_bits = psa_get_key_bits(attributes);

    if (PSA_BYTES_TO_BITS(key_length) != key_bits) {
        /* The attributes don't match the buffer given as input */
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (!PSA_ALG_IS_AEAD(alg)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Validate the algorithm first */
    ret = check_alg(alg, &ref_alg);
    if (ret != PSA_SUCCESS) {
        return ret;
    }

    CC_PalMemSetZero(operation, sizeof(cc3xx_aead_operation_t));

    operation->alg = ref_alg;
    operation->key_type = key_type;
    operation->dir = dir;
    operation->tag_length = PSA_ALG_AEAD_GET_TAG_LENGTH(alg);

    switch (operation->key_type) {
    case PSA_KEY_TYPE_AES:
        switch (operation->alg) {
#if defined(CC3XX_CONFIG_SUPPORT_GCM)
        case PSA_ALG_GCM:
            cc3xx_gcm_init(&operation->ctx.gcm);

            switch (operation->dir) {
            case PSA_CRYPTO_DRIVER_ENCRYPT:
                if (( ret = cc3xx_gcm_setkey_enc(
                        &operation->ctx.gcm,
                        key,
                        key_bits) )
                    != PSA_SUCCESS) {
                    return ret;
                }

                break;
            case PSA_CRYPTO_DRIVER_DECRYPT:
                if (( ret = cc3xx_gcm_setkey_dec(
                        &operation->ctx.gcm,
                        key,
                        key_bits) )
                    != PSA_SUCCESS) {
                    return ret;
                }

                break;
            default:
                return PSA_ERROR_NOT_SUPPORTED;
            }
            break;
#endif /* CC3XX_CONFIG_SUPPORT_GCM */
#if defined(CC3XX_CONFIG_SUPPORT_CCM)
        case PSA_ALG_CCM:
            cc3xx_ccm_init(&operation->ctx.ccm);

            switch (operation->dir) {
            case PSA_CRYPTO_DRIVER_ENCRYPT:
                if (( ret = cc3xx_ccm_setkey_enc(
                        &operation->ctx.ccm,
                        key,
                        key_bits) )
                    != PSA_SUCCESS) {
                    return ret;
                }

                break;
            case PSA_CRYPTO_DRIVER_DECRYPT:
                if (( ret = cc3xx_ccm_setkey_dec(
                        &operation->ctx.ccm,
                        key,
                        key_bits) )
                    != PSA_SUCCESS) {
                    return ret;
                }

                break;
            default:
                return PSA_ERROR_NOT_SUPPORTED;
            }
            break;
#endif /* CC3XX_CONFIG_SUPPORT_CCM */
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }

        break;
    case PSA_KEY_TYPE_CHACHA20:
        switch (operation->alg) {
#if defined(CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305)
        case PSA_ALG_CHACHA20_POLY1305:
            /* The setup operation just sets up the object for Chacha20
             * multipart and then sets the key on the Chacha context
             */
            cc3xx_chacha20_init(&operation->ctx.chachapoly.chacha);
            ret = cc3xx_chacha20_setkey(&operation->ctx.chachapoly.chacha,
                                        key,
                                        key_length);
            if (ret != PSA_SUCCESS) {
                return ret;
            }
            if (operation->dir == PSA_CRYPTO_DRIVER_ENCRYPT) {
                operation->ctx.chachapoly.bAuthenticateInput = false;
            } else { /* PSA_CRYPTO_DRIVER_DECRYPT */
                operation->ctx.chachapoly.bAuthenticateInput = true;
            }
            break;
#endif /* CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305 */
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }
        break;

    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }

    return ret;
}

#ifdef CC3XX_CONFIG_AEAD_ONE_SHOT_USE_MULTIPART
static psa_status_t aead_one_shot_with_multipart(
                    const psa_key_attributes_t *attributes,
                    const uint8_t *key_buffer, size_t key_buffer_size,
                    psa_algorithm_t alg, const uint8_t *nonce,
                    size_t nonce_length, const uint8_t *additional_data,
                    size_t additional_data_length, const uint8_t *input,
                    size_t input_length, uint8_t *output,
                    size_t output_size, size_t *output_length,
                    psa_encrypt_or_decrypt_t dir)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    cc3xx_aead_operation_t operation = {0};
    uint8_t local_tag_buffer[PSA_AEAD_TAG_MAX_SIZE] = {0};
    size_t tag_length = 0;
    size_t expected_tag_length = PSA_AEAD_TAG_LENGTH(
                                                 psa_get_key_type(attributes),
                                                 psa_get_key_bits(attributes),
                                                 alg);
    size_t update_length = (dir == PSA_CRYPTO_DRIVER_ENCRYPT) ? input_length :
                                           input_length - expected_tag_length;

    size_t finalisation_length = 0; /* Length produced in the final step */
    if (output_length == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    *output_length = 0;

    /* Check buffer sizes */
    if (dir == PSA_CRYPTO_DRIVER_ENCRYPT) {
        if (output_size < input_length + expected_tag_length) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }
    } else {
        if (output_size < input_length - expected_tag_length) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }
    }

    /* Setup the operation in the required direction */
    if (dir == PSA_CRYPTO_DRIVER_ENCRYPT) {
        status = cc3xx_aead_encrypt_setup(&operation, attributes, key_buffer,
                                          key_buffer_size, alg);
    } else {
        status = cc3xx_aead_decrypt_setup(&operation, attributes, key_buffer,
                                          key_buffer_size, alg);
    }
    if (status != PSA_SUCCESS) {
        goto abort;
    }

    /* Set lengths and nonce */
    status = cc3xx_aead_set_lengths(&operation, additional_data_length,
                                    update_length);
    if (status != PSA_SUCCESS) {
        goto abort;
    }
    status = cc3xx_aead_set_nonce(&operation, nonce, nonce_length);
    if (status != PSA_SUCCESS) {
        goto abort;
    }

    /* Update ad and input data */
    status = cc3xx_aead_update_ad(&operation, additional_data,
                                  additional_data_length);
    if (status != PSA_SUCCESS) {
        goto abort;
    }
    status = cc3xx_aead_update(&operation, input, update_length,
                               output, output_size, output_length);
    if (status != PSA_SUCCESS) {
        goto abort;
    }

    /* Finalisation step */
    if (dir == PSA_CRYPTO_DRIVER_ENCRYPT) {
        status = cc3xx_aead_finish(&operation,
                                   &output[*output_length],
                                   output_size - *output_length,
                                   &finalisation_length,
                                   local_tag_buffer,
                                   sizeof(local_tag_buffer),
                                   &tag_length);
    } else { /* PSA_CRYPTO_DRIVER_DECRYPT */
        status = cc3xx_aead_verify(&operation,
                                   &output[*output_length],
                                   output_size - *output_length,
                                   &finalisation_length,
                                   &input[update_length],
                                   input_length - update_length);
    }
    if (status != PSA_SUCCESS) {
        goto abort;
    }

    *output_length += finalisation_length;
    /* Append the tag during encryption */
    if (dir == PSA_CRYPTO_DRIVER_ENCRYPT) {
        CC_PalMemCopy(&output[*output_length], local_tag_buffer, tag_length);
        *output_length += tag_length;
    }

abort:
    if (status != PSA_SUCCESS) {
        CC_PalMemSetZero(output, output_size);
        *output_length = 0;
        (void)cc3xx_aead_abort(&operation);
    }

    return status;
}
#endif /* CC3XX_CONFIG_AEAD_ONE_SHOT_USE_MULTIPART */

/** \defgroup psa_aead PSA driver entry points for AEAD
 *
 *  Entry points for AEAD encryption and decryption as described by the PSA
 *  Cryptoprocessor Driver interface specification
 *
 *  @{
 */
psa_status_t
cc3xx_aead_encrypt(const psa_key_attributes_t *attributes,
                   const uint8_t *key_buffer, size_t key_buffer_size,
                   psa_algorithm_t alg, const uint8_t *nonce,
                   size_t nonce_length, const uint8_t *additional_data,
                   size_t additional_data_length, const uint8_t *plaintext,
                   size_t plaintext_length, uint8_t *ciphertext,
                   size_t ciphertext_size, size_t *ciphertext_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
#ifndef CC3XX_CONFIG_AEAD_ONE_SHOT_USE_MULTIPART
    psa_algorithm_t ref_alg;

    /* Validate the algorithm first */
    status = check_alg(alg, &ref_alg);
    if (status != PSA_SUCCESS) {
        return status;
    }

    switch (ref_alg) {
#if defined(CC3XX_CONFIG_SUPPORT_CCM)
    case PSA_ALG_CCM:
        status = cc3xx_encrypt_ccm(
            attributes, key_buffer, key_buffer_size, alg, nonce, nonce_length,
            additional_data, additional_data_length, plaintext,
            plaintext_length, ciphertext, ciphertext_size, ciphertext_length);
        break;
#endif /* CC3XX_CONFIG_SUPPORT_CCM */
#if defined(CC3XX_CONFIG_SUPPORT_GCM)
    case PSA_ALG_GCM:
        status = cc3xx_gcm_encrypt(
            attributes, key_buffer, key_buffer_size, alg, nonce, nonce_length,
            additional_data, additional_data_length, plaintext,
            plaintext_length, ciphertext, ciphertext_size, ciphertext_length);
        break;
#endif /* CC3XX_CONFIG_SUPPORT_GCM */
#if defined(CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305)
    case PSA_ALG_CHACHA20_POLY1305:
        status = cc3xx_encrypt_chacha20_poly1305(
            attributes, key_buffer, key_buffer_size, alg, nonce, nonce_length,
            additional_data, additional_data_length, plaintext,
            plaintext_length, ciphertext, ciphertext_size, ciphertext_length);
        break;
#endif /* CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305 */
    default:
        status = PSA_ERROR_NOT_SUPPORTED;
        break;
    }
#else /* CC3XX_CONFIG_AEAD_ONE_SHOT_USE_MULTIPART */
    status = aead_one_shot_with_multipart(attributes,
                    key_buffer, key_buffer_size,
                    alg, nonce,
                    nonce_length, additional_data,
                    additional_data_length, plaintext,
                    plaintext_length, ciphertext,
                    ciphertext_size, ciphertext_length,
                    PSA_CRYPTO_DRIVER_ENCRYPT);
#endif /* CC3XX_CONFIG_AEAD_ONE_SHOT_USE_MULTIPART */
    return status;
}

psa_status_t cc3xx_aead_decrypt(
        const psa_key_attributes_t *attributes,
        const uint8_t *key_buffer, size_t key_buffer_size,
        psa_algorithm_t alg,
        const uint8_t *nonce, size_t nonce_length,
        const uint8_t *additional_data, size_t additional_data_length,
        const uint8_t *ciphertext, size_t ciphertext_length,
        uint8_t *plaintext, size_t plaintext_size, size_t *plaintext_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
#ifndef CC3XX_CONFIG_AEAD_ONE_SHOT_USE_MULTIPART
    psa_algorithm_t ref_alg;

    /* Validate the algorithm first */
    status = check_alg(alg, &ref_alg);
    if (status != PSA_SUCCESS) {
        return status;
    }

    switch (ref_alg) {
#if defined(CC3XX_CONFIG_SUPPORT_CCM)
    case PSA_ALG_CCM:
        status = cc3xx_decrypt_ccm(
            attributes, key_buffer, key_buffer_size, alg, nonce, nonce_length,
            additional_data, additional_data_length, ciphertext,
            ciphertext_length, plaintext, plaintext_size, plaintext_length);
        break;
#endif /* CC3XX_CONFIG_SUPPORT_CCM */
#if defined(CC3XX_CONFIG_SUPPORT_GCM)
    case PSA_ALG_GCM:
        status = cc3xx_gcm_decrypt(
            attributes, key_buffer, key_buffer_size, alg, nonce, nonce_length,
            additional_data, additional_data_length, ciphertext,
            ciphertext_length, plaintext, plaintext_size, plaintext_length);
        break;
#endif /* CC3XX_CONFIG_SUPPORT_GCM */
#if defined(CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305)
    case PSA_ALG_CHACHA20_POLY1305:
        status = cc3xx_decrypt_chacha20_poly1305(
            attributes, key_buffer, key_buffer_size, alg, nonce, nonce_length,
            additional_data, additional_data_length, ciphertext,
            ciphertext_length, plaintext, plaintext_size, plaintext_length);
        break;
#endif /* CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305 */
    default:
        status = PSA_ERROR_NOT_SUPPORTED;
        break;
    }

    return status;
#else /* CC3XX_CONFIG_AEAD_ONE_SHOT_USE_MULTIPART */
    status = aead_one_shot_with_multipart(attributes,
                    key_buffer, key_buffer_size,
                    alg, nonce,
                    nonce_length, additional_data,
                    additional_data_length, ciphertext,
                    ciphertext_length, plaintext,
                    plaintext_size, plaintext_length,
                    PSA_CRYPTO_DRIVER_DECRYPT);
#endif /* CC3XX_CONFIG_AEAD_ONE_SHOT_USE_MULTIPART */
    return status;
}

psa_status_t cc3xx_aead_encrypt_setup(
        cc3xx_aead_operation_t *operation,
        const psa_key_attributes_t *attributes,
        const uint8_t *key_buffer, size_t key_buffer_size,
        psa_algorithm_t alg)
{
    return aead_setup(operation,
                      attributes,
                      key_buffer, key_buffer_size,
                      alg,
                      PSA_CRYPTO_DRIVER_ENCRYPT);
}

psa_status_t cc3xx_aead_decrypt_setup(
        cc3xx_aead_operation_t *operation,
        const psa_key_attributes_t *attributes,
        const uint8_t *key_buffer, size_t key_buffer_size,
        psa_algorithm_t alg)
{
    return aead_setup(operation,
                      attributes,
                      key_buffer, key_buffer_size,
                      alg,
                      PSA_CRYPTO_DRIVER_DECRYPT);
}

psa_status_t cc3xx_aead_set_nonce(
        cc3xx_aead_operation_t *operation,
        const uint8_t *nonce,
        size_t nonce_length)
{
    psa_status_t ret = PSA_ERROR_CORRUPTION_DETECTED;

    switch (operation->key_type) {
    case PSA_KEY_TYPE_AES:
        switch (operation->alg) {
#if defined(CC3XX_CONFIG_SUPPORT_GCM)
        case PSA_ALG_GCM:
            if ((ret = cc3xx_gcm_set_nonce(
                    &operation->ctx.gcm,
                    nonce,
                    nonce_length,
                    operation->tag_length))
                != PSA_SUCCESS) {
                return ret;
            }
            break;
#endif /* CC3XX_CONFIG_SUPPORT_GCM */
#if defined(CC3XX_CONFIG_SUPPORT_CCM)
        case PSA_ALG_CCM:
            if ((ret = cc3xx_ccm_set_nonce(
                    &operation->ctx.ccm,
                    nonce,
                    nonce_length,
                    operation->tag_length,
                    CC3XX_CCM_MODE_CCM)) /* FixMe: check support for CCM* */
                != PSA_SUCCESS) {
                return ret;
            }
            break;
#endif /* CC3XX_CONFIG_SUPPORT_CCM */
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }

        break;
    case PSA_KEY_TYPE_CHACHA20:
        switch (operation->alg) {
#if defined(CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305)
        case PSA_ALG_CHACHA20_POLY1305:
        {
            uint32_t otk[32/sizeof(uint32_t)] = {0};
            ret = cc3xx_chacha20_set_nonce(&operation->ctx.chachapoly.chacha,
                                           nonce,
                                           nonce_length);
            if (ret != PSA_SUCCESS) {
                return ret;
            }
            /* After successfully setting the nonce, we can also setup the OTK
             * to be used in the associated Poly1305 algorithm. Internally this
             * will set the counter to 0 as specified by RFC7539
             */
            ret = cc3xx_chacha20_poly1305_gen_otk(
                                            &operation->ctx.chachapoly.chacha,
                                            (uint8_t *)otk,
                                            sizeof(otk));
            if (ret != PSA_SUCCESS) {
                return ret;
            }
            /* Set the counter back to 1 as per RFC7539. For the time being the
             * implementation does not support nonce lengths different that 12.
             * If support for nonce length equal to 16 is enabled, the first 4
             * bytes can be used to specify a counter value.
             */
            ret = cc3xx_chacha20_set_counter(&operation->ctx.chachapoly.chacha,
                                             1);
            if (ret != PSA_SUCCESS) {
                return ret;
            }

            /* Once we have an OTK, initialise the Poly1305 context with that */
            ret = cc3xx_chacha20_poly1305_set_otk(
                                            &operation->ctx.chachapoly.poly,
                                            (const uint8_t *)otk,
                                            sizeof(otk));
            CC_PalMemSetZero(otk, sizeof(otk));
            if (ret != PSA_SUCCESS) {
                return ret;
            }
        }
        break;
#endif /* CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305 */
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }
    break;

    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }

    return ret;
}

psa_status_t cc3xx_aead_set_lengths(
        cc3xx_aead_operation_t *operation,
        size_t ad_length,
        size_t plaintext_length)
{
    psa_status_t ret = PSA_ERROR_CORRUPTION_DETECTED;

    switch (operation->key_type) {
    case PSA_KEY_TYPE_AES:
        switch (operation->alg) {
#if defined(CC3XX_CONFIG_SUPPORT_GCM)
        case PSA_ALG_GCM:
            if (( ret = cc3xx_gcm_set_lengths(
                    &operation->ctx.gcm,
                    ad_length,
                    plaintext_length) )
                != PSA_SUCCESS) {
                return ret;
            }
            break;
#endif /* CC3XX_CONFIG_SUPPORT_GCM */
#if defined(CC3XX_CONFIG_SUPPORT_CCM)
        case PSA_ALG_CCM:
            if (( ret = cc3xx_ccm_set_lengths(
                    &operation->ctx.ccm,
                    ad_length,
                    plaintext_length) )
                != PSA_SUCCESS) {
                return ret;
            }
            break;
#endif /* CC3XX_CONFIG_SUPPORT_CCM */
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }
        break;

    case PSA_KEY_TYPE_CHACHA20:
        switch (operation->alg) {
#if defined(CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305)
        case PSA_ALG_CHACHA20_POLY1305:
            ret = cc3xx_chacha20_poly1305_set_lengths(
                                            &operation->ctx.chachapoly,
                                            ad_length,
                                            plaintext_length);
            if (ret != PSA_SUCCESS) {
                return ret;
            }
            break;
#endif /* CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305 */
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }
    break;

    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }

    return ret;
}

psa_status_t cc3xx_aead_update_ad(
        cc3xx_aead_operation_t *operation,
        const uint8_t *input,
        size_t input_size)
{
    psa_status_t ret = PSA_ERROR_CORRUPTION_DETECTED;

    /* There is no associated data */
    if (input_size == 0) {
        return PSA_SUCCESS;
    }

    switch (operation->key_type) {
    case PSA_KEY_TYPE_AES:
        switch (operation->alg) {
#if defined(CC3XX_CONFIG_SUPPORT_GCM)
        case PSA_ALG_GCM:
            if (( ret = cc3xx_gcm_update_ad(
                    &operation->ctx.gcm,
                    input,
                    input_size) )
                != PSA_SUCCESS) {
                return ret;
            }
            break;
#endif /* CC3XX_CONFIG_SUPPORT_GCM */
#if defined(CC3XX_CONFIG_SUPPORT_CCM)
        case PSA_ALG_CCM:
            if (( ret = cc3xx_ccm_update_ad(
                    &operation->ctx.ccm,
                    input,
                    input_size) )
                != PSA_SUCCESS) {
                return ret;
            }
            break;
#endif /* CC3XX_CONFIG_SUPPORT_CCM */
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }
        break;

    case PSA_KEY_TYPE_CHACHA20:
        switch (operation->alg) {
#if defined(CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305)
        case PSA_ALG_CHACHA20_POLY1305:
            ret = cc3xx_chacha20_poly1305_update_ad(
                    &operation->ctx.chachapoly,
                    input,
                    input_size);
            if (ret != PSA_SUCCESS) {
                return ret;
            }
            break;
#endif /* CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305 */
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }
        break;

    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }

    return ret;
}

psa_status_t cc3xx_aead_update(
        cc3xx_aead_operation_t *operation,
        const uint8_t *input,
        size_t input_length,
        uint8_t *output,
        size_t output_size,
        size_t *output_length)
{
    psa_status_t ret = PSA_ERROR_CORRUPTION_DETECTED;

    if (output_length == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    *output_length = 0;

    /* There is no data to process */
    if (input_length == 0) {
        return PSA_SUCCESS;
    }

    if (output_size < input_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    switch (operation->key_type) {
    case PSA_KEY_TYPE_AES:
        switch (operation->alg) {
#if defined(CC3XX_CONFIG_SUPPORT_GCM)
        case PSA_ALG_GCM:
            if (( ret = cc3xx_gcm_update(
                    &operation->ctx.gcm,
                    input_length,
                    input,
                    output ))
                != PSA_SUCCESS) {
                return ret;
            }
            *output_length = input_length;
            break;
#endif /* CC3XX_CONFIG_SUPPORT_GCM */
#if defined(CC3XX_CONFIG_SUPPORT_CCM)
        case PSA_ALG_CCM:
            if (( ret = cc3xx_ccm_update(
                    &operation->ctx.ccm,
                    input_length,
                    input,
                    output ))
                != PSA_SUCCESS) {
                return ret;
            }
            *output_length = input_length;
            break;
#endif /* CC3XX_CONFIG_SUPPORT_CCM */
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }
        break;

    case PSA_KEY_TYPE_CHACHA20:
        switch (operation->alg) {
#if defined(CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305)
        case PSA_ALG_CHACHA20_POLY1305:
            ret = cc3xx_chacha20_poly1305_update(
                    &operation->ctx.chachapoly,
                    input, input_length,
                    output, output_size, output_length);
            if (ret != PSA_SUCCESS) {
                return ret;
            }
            break;
#endif /* CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305 */
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }
        break;

    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }

    return ret;
}

psa_status_t cc3xx_aead_finish(
        cc3xx_aead_operation_t *operation,
        uint8_t *ciphertext,
        size_t ciphertext_size,
        size_t *ciphertext_length,
        uint8_t *tag,
        size_t tag_size,
        size_t *tag_length)
{
    psa_status_t ret = PSA_ERROR_CORRUPTION_DETECTED;

    (void)ciphertext;
    (void)ciphertext_size;
    *ciphertext_length = 0;
    if (operation->tag_length > tag_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    switch (operation->key_type) {
    case PSA_KEY_TYPE_AES:
        switch (operation->alg) {
#if defined(CC3XX_CONFIG_SUPPORT_GCM)
        case PSA_ALG_GCM:
            if (( ret = cc3xx_gcm_finish(&operation->ctx.gcm,
                            tag, tag_size, tag_length)) != PSA_SUCCESS) {
                return ret;
            }
            break;
#endif /* CC3XX_CONFIG_SUPPORT_GCM */
#if defined(CC3XX_CONFIG_SUPPORT_CCM)
        case PSA_ALG_CCM:
            if (( ret = cc3xx_ccm_finish(&operation->ctx.ccm,
                            tag, tag_size, tag_length)) != PSA_SUCCESS) {
                return ret;
            }
            break;
#endif /* CC3XX_CONFIG_SUPPORT_CCM */
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }
        break;

    case PSA_KEY_TYPE_CHACHA20:
        switch (operation->alg) {
#if defined(CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305)
        case PSA_ALG_CHACHA20_POLY1305:
            ret = cc3xx_chacha20_poly1305_finish(&operation->ctx.chachapoly,
                                                 tag, tag_size, tag_length);
            if (ret != PSA_SUCCESS) {
                return ret;
            }
            break;
#endif /* CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305 */
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }
        break;

    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }

    return ret;
}

psa_status_t cc3xx_aead_verify(
        cc3xx_aead_operation_t *operation,
        uint8_t *plaintext,
        size_t plaintext_size,
        size_t *plaintext_length,
        const uint8_t *tag,
        size_t tag_size)
{
    psa_status_t ret = PSA_ERROR_CORRUPTION_DETECTED;

    (void)plaintext;
    (void)plaintext_size;
    *plaintext_length = 0;

    size_t tag_length = 0;

    switch (operation->key_type) {
    case PSA_KEY_TYPE_AES:
        switch (operation->alg) {
#if defined(CC3XX_CONFIG_SUPPORT_GCM)
        case PSA_ALG_GCM:
            if (( ret = cc3xx_gcm_finish(
                    &operation->ctx.gcm,
                    (uint8_t *)tag,
                    tag_size, &tag_length))
                != PSA_SUCCESS) {
                return ret;
            }
            break;
#endif /* CC3XX_CONFIG_SUPPORT_GCM */
#if defined(CC3XX_CONFIG_SUPPORT_CCM)
        case PSA_ALG_CCM:
            if (( ret = cc3xx_ccm_finish(
                    &operation->ctx.ccm,
                    (uint8_t *)tag,
                    tag_size, &tag_length))
                != PSA_SUCCESS) {
                return ret;
            }
            break;
#endif /* CC3XX_CONFIG_SUPPORT_CCM */
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }
        break;

    case PSA_KEY_TYPE_CHACHA20:
        switch (operation->alg) {
#if defined(CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305)
        case PSA_ALG_CHACHA20_POLY1305:
            ret = cc3xx_chacha20_poly1305_verify(&operation->ctx.chachapoly,
                                                 tag, tag_size);
            if (ret != PSA_SUCCESS) {
                return ret;
            }
            break;
#endif /* CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305 */
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }
        break;

    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }

    return ret;
}

psa_status_t cc3xx_aead_abort(cc3xx_aead_operation_t *operation)
{
    switch (operation->key_type) {
    case PSA_KEY_TYPE_AES:
        switch (operation->alg) {
#if defined(CC3XX_CONFIG_SUPPORT_GCM)
        case PSA_ALG_GCM:
            cc3xx_gcm_free(&operation->ctx.gcm);
            break;
#endif /* CC3XX_CONFIG_SUPPORT_GCM */
#if defined(CC3XX_CONFIG_SUPPORT_CCM)
        case PSA_ALG_CCM:
            cc3xx_ccm_free(&operation->ctx.ccm);
            break;
#endif /* CC3XX_CONFIG_SUPPORT_CCM */
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }
        break;

    case PSA_KEY_TYPE_CHACHA20:
        switch (operation->alg) {
#if defined(CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305)
        case PSA_ALG_CHACHA20_POLY1305:
            cc3xx_chacha20_free(&operation->ctx.chachapoly.chacha);
            /* Nothing specific for the Poly1305 state, it just gets set to
             * zero when memory is cleared before leaving this function
             */
            break;
#endif /* CC3XX_CONFIG_SUPPORT_CHACHA20_POLY1305 */
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }
        break;

    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }

    CC_PalMemSetZero(operation, sizeof(cc3xx_aead_operation_t));

    return PSA_SUCCESS;
}
/** @} */ // end of psa_aead
