/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "aesgcm_driver.h"
#include "cc_common.h"
#include "cc_pal_abort.h"
#include "cc_pal_mem.h"
#include "cc_pal_types.h"
#include <psa/crypto.h>

#include "internal/cc3xx_psa_aead_gcm.h"

#define CC3XX_ERR_GCM_API_IS_NOT_SUPPORTED                                     \
    -0x0016 /**< API is NOT supported.                                         \
             */

/*! AES GCM data in maximal size in bytes. */
#define CC3XX_AESGCM_DATA_IN_MAX_SIZE_BYTES 0xFFFF // (64KB - 1)
/*! AES GCM IV maximal size in bytes. */
#define CC3XX_AESGCM_IV_MAX_SIZE_BYTES 0xFFFF // (64KB - 1)
/*! AES GCM AAD maximal size in bytes. */
#define CC3XX_AESGCM_AAD_MAX_SIZE_BYTES 0xFFFF // (64KB - 1)

/*! AES GCM 96 bits size IV. */
#define CC3XX_AESGCM_IV_96_BITS_SIZE_BYTES 12

/*! AES GCM Tag size: 4 bytes. */
#define CC3XX_AESGCM_TAG_SIZE_4_BYTES 4
/*! AES GCM Tag size: 8 bytes. */
#define CC3XX_AESGCM_TAG_SIZE_8_BYTES 8
/*! AES GCM Tag size: 12 bytes. */
#define CC3XX_AESGCM_TAG_SIZE_12_BYTES 12
/*! AES GCM Tag size: 13 bytes. */
#define CC3XX_AESGCM_TAG_SIZE_13_BYTES 13
/*! AES GCM Tag size: 14 bytes. */
#define CC3XX_AESGCM_TAG_SIZE_14_BYTES 14
/*! AES GCM Tag size: 15 bytes. */
#define CC3XX_AESGCM_TAG_SIZE_15_BYTES 15
/*! AES GCM Tag size: 16 bytes. */
#define CC3XX_AESGCM_TAG_SIZE_16_BYTES 16

static void gcm_context_init(AesGcmContext_t *context)
{
    CC_PalMemSetZero(context, sizeof(AesGcmContext_t));
}

static void gcm_context_free(AesGcmContext_t *context)
{
    CC_PalMemSetZero(context, sizeof(AesGcmContext_t));
}

static psa_status_t gcm_set_key(AesGcmContext_t *context,
                                const unsigned char *key, unsigned int keybits)
{
    if (context == NULL || key == NULL) {
        CC_PAL_LOG_ERR("Null pointer, ctx or key are NULL\n");
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    switch (keybits) {
    case 128:
        context->keySizeId = KEY_SIZE_128_BIT;
        break;
    case 192:
        context->keySizeId = KEY_SIZE_192_BIT;
        break;
    case 256:
        context->keySizeId = KEY_SIZE_256_BIT;
        break;
    default:
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Copy user key to context */
    CC_PalMemCopy(context->keyBuf, key, PSA_BITS_TO_BYTES(keybits));

    return PSA_SUCCESS;
}

static psa_status_t gcm_calc_h(AesGcmContext_t *context)
{
    drvError_t rc;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    /* Set process mode to 'CalcH' */
    context->processMode = DRV_AESGCM_Process_CalcH;

    /* set data buffers structures */
    rc = SetDataBuffersInfo((uint8_t *)(context->tempBuf),
                            CC_AESGCM_GHASH_DIGEST_SIZE_BYTES, &inBuffInfo,
                            (uint8_t *)(context->H), AES_128_BIT_KEY_SIZE,
                            &outBuffInfo);
    if (rc != 0) {
        CC_PAL_LOG_ERR("illegal data buffers\n");

        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Calculate H */
    rc = ProcessAesGcm(context, &inBuffInfo, &outBuffInfo,
                       CC_AES_BLOCK_SIZE_IN_BYTES);
    if (rc != AES_DRV_OK) {
        CC_PAL_LOG_ERR("calculating H failed with error code %d\n", rc);
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return PSA_SUCCESS;
}

static psa_status_t gcm_init(AesGcmContext_t *context,
                             cryptoDirection_t direction, const uint8_t *pIv,
                             size_t ivSize, const uint8_t *pAad, size_t aadSize,
                             const uint8_t *pDataIn, size_t dataSize,
                             uint8_t *pDataOut, const uint8_t *pTag,
                             size_t tagSize)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    /* Check the Encrypt / Decrypt flag validity */
    if (CRYPTO_DIRECTION_NUM_OF_ENC_MODES <= direction) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Check the data in size validity */
    if (CC3XX_AESGCM_DATA_IN_MAX_SIZE_BYTES < dataSize) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (0 != dataSize) {
        /* Check dataIn pointer */
        if (NULL == pDataIn) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }

        /* Check dataOut pointer */
        if (NULL == pDataOut) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
    }

    /* Check the IV size validity */
    if ((CC3XX_AESGCM_IV_MAX_SIZE_BYTES < ivSize) || (0 == ivSize)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Check iv pointer */
    if (NULL == pIv) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Check the AAD size validity */
    if (CC3XX_AESGCM_AAD_MAX_SIZE_BYTES < aadSize) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Check aad pointer */
    if ((NULL == pAad) && (aadSize != 0)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Check the Tag size validity */
    if ((CC3XX_AESGCM_TAG_SIZE_4_BYTES != tagSize) &&
        (CC3XX_AESGCM_TAG_SIZE_8_BYTES != tagSize) &&
        (CC3XX_AESGCM_TAG_SIZE_12_BYTES != tagSize) &&
        (CC3XX_AESGCM_TAG_SIZE_13_BYTES != tagSize) &&
        (CC3XX_AESGCM_TAG_SIZE_14_BYTES != tagSize) &&
        (CC3XX_AESGCM_TAG_SIZE_15_BYTES != tagSize) &&
        (CC3XX_AESGCM_TAG_SIZE_16_BYTES != tagSize)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Check Tag pointer */
    if (NULL == pTag) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Set direction of operation: enc./dec. */
    context->dir = direction;
    context->dataSize = dataSize;
    context->ivSize = ivSize;
    context->aadSize = aadSize;
    context->tagSize = tagSize;

    /******************************************************/
    /***                Calculate H                     ***/
    /******************************************************/
    status = gcm_calc_h(context);
    if (status != PSA_SUCCESS) {
        CC_PAL_LOG_ERR("calculating H failed with error code %d\n", status);
        return status;
    }

    return status;
}

static psa_status_t gcm_process_j0(AesGcmContext_t *context, const uint8_t *pIv)
{
    drvError_t rc = 0;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    if (CC3XX_AESGCM_IV_96_BITS_SIZE_BYTES == context->ivSize) {
        // Concatenate IV||0(31)||1
        CC_PalMemCopy(context->J0, pIv, CC3XX_AESGCM_IV_96_BITS_SIZE_BYTES);
        context->J0[3] = SWAP_ENDIAN(0x00000001);
    } else {

        /***********************************************/
        /* Calculate GHASH over the first phase buffer */
        /***********************************************/
        /* Set process mode to 'CalcJ0' */
        context->processMode = DRV_AESGCM_Process_CalcJ0_FirstPhase;

        /* set data buffers structures */
        rc = SetDataBuffersInfo(pIv, context->ivSize, &inBuffInfo, NULL, 0,
                                &outBuffInfo);
        if (rc != 0) {
            CC_PAL_LOG_ERR("illegal data buffers\n");
            return PSA_ERROR_DATA_INVALID;
        }

        /* Calculate J0 - First phase */
        rc = ProcessAesGcm(context, &inBuffInfo, &outBuffInfo, context->ivSize);
        if (rc != AES_DRV_OK) {
            CC_PAL_LOG_ERR(
                "calculating J0 (phase 1) failed with error code 0x%X\n", rc);
            return PSA_ERROR_GENERIC_ERROR;
        }

        /*********************************************/
        /* Build & Calculate the second phase buffer */
        /*********************************************/
        CC_PalMemSetZero(context->tempBuf, sizeof(context->tempBuf));
        context->tempBuf[3] =
            (context->ivSize << 3) & BITMASK(CC_BITS_IN_32BIT_WORD);
        context->tempBuf[3] = SWAP_ENDIAN(context->tempBuf[3]);

        /* Set process mode to 'CalcJ0' */
        context->processMode = DRV_AESGCM_Process_CalcJ0_SecondPhase;

        /* set data buffers structures */
        rc = SetDataBuffersInfo((uint8_t *)(context->tempBuf),
                                CC_AESGCM_GHASH_DIGEST_SIZE_BYTES, &inBuffInfo,
                                NULL, 0, &outBuffInfo);
        if (rc != 0) {
            CC_PAL_LOG_ERR("illegal data buffers\n");
            return PSA_ERROR_DATA_INVALID;
        }

        /* Calculate J0 - Second phase  */
        rc = ProcessAesGcm(context, &inBuffInfo, &outBuffInfo,
                           CC_AESGCM_GHASH_DIGEST_SIZE_BYTES);
        if (rc != AES_DRV_OK) {
            CC_PAL_LOG_ERR(
                "calculating J0 (phase 2) failed with error code %d\n", rc);
            return PSA_ERROR_DATA_INVALID;
        }
    }

    if (rc != AES_DRV_OK) {
        CC_PAL_LOG_ERR("gcm process j0 failed with error code %d\n", rc);
        return PSA_ERROR_DATA_INVALID;
    } else {
        return PSA_SUCCESS;
    }
}

static psa_status_t gcm_process_aad(AesGcmContext_t *context,
                                    const uint8_t *pAad)
{
    drvError_t rc = 0;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    /* Clear Ghash result buffer */
    CC_PalMemSetZero(context->ghashResBuf, sizeof(context->ghashResBuf));

    if (0 == context->aadSize) {
        return PSA_SUCCESS;
    }

    /* Set process mode to 'Process_A' */
    context->processMode = DRV_AESGCM_Process_A;

    /* set data buffers structures */
    rc = SetDataBuffersInfo(pAad, context->aadSize, &inBuffInfo, NULL, 0,
                            &outBuffInfo);
    if (rc != 0) {
        CC_PAL_LOG_ERR("illegal data buffers\n");
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Calculate GHASH(A) */
    rc = ProcessAesGcm(context, &inBuffInfo, &outBuffInfo, context->aadSize);
    if (rc != AES_DRV_OK) {
        CC_PAL_LOG_ERR("processing AAD failed with error code %d\n", rc);
        return PSA_ERROR_DATA_INVALID;
    } else {
        return PSA_SUCCESS;
    }
}

static psa_status_t gcm_process_cipher(AesGcmContext_t *context,
                                       const uint8_t *pTextDataIn,
                                       uint8_t *pTextDataOut)
{
    drvError_t rc;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    /* Must NOT perform in this case */
    if (0 == context->dataSize) {
        return PSA_SUCCESS;
    }

    /* Set process mode to 'Process_DataIn' */
    context->processMode = DRV_AESGCM_Process_DataIn;

    /* set data buffers structures */
    rc = SetDataBuffersInfo(pTextDataIn, context->dataSize, &inBuffInfo,
                            pTextDataOut, context->dataSize, &outBuffInfo);
    if (rc != 0) {
        CC_PAL_LOG_ERR("illegal data buffers\n");
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    rc = ProcessAesGcm(context, &inBuffInfo, &outBuffInfo, context->dataSize);
    if (rc != AES_DRV_OK) {
        CC_PAL_LOG_ERR("processing cipher failed with error code %d\n", rc);
        return PSA_ERROR_DATA_INVALID;
    } else {
        return PSA_SUCCESS;
    }
}

static psa_status_t gcm_process_lenA_lenC(AesGcmContext_t *context)
{
    drvError_t rc;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    /* Build buffer */
    context->tempBuf[1] =
        (context->aadSize << 3) & BITMASK(CC_BITS_IN_32BIT_WORD);
    context->tempBuf[1] = SWAP_ENDIAN(context->tempBuf[1]);
    context->tempBuf[0] = 0;
    context->tempBuf[3] =
        (context->dataSize << 3) & BITMASK(CC_BITS_IN_32BIT_WORD);
    context->tempBuf[3] = SWAP_ENDIAN(context->tempBuf[3]);
    context->tempBuf[2] = 0;

    /* Set process mode to 'Process_LenA_LenC' */
    context->processMode = DRV_AESGCM_Process_LenA_LenC;

    /* set data buffers structures */
    rc = SetDataBuffersInfo((uint8_t *)(context->tempBuf),
                            CC_AESGCM_GHASH_DIGEST_SIZE_BYTES, &inBuffInfo,
                            NULL, 0, &outBuffInfo);
    if (rc != 0) {
        CC_PAL_LOG_ERR("illegal data buffers\n");
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Calculate GHASH(LenA || LenC) */
    rc = ProcessAesGcm(context, &inBuffInfo, &outBuffInfo,
                       CC_AESGCM_GHASH_DIGEST_SIZE_BYTES);
    if (rc != AES_DRV_OK) {
        CC_PAL_LOG_ERR(
            "processing Lengths of AAD and Cipher failed with error code %d\n",
            rc);
        return PSA_ERROR_DATA_INVALID;
    } else {
        return PSA_SUCCESS;
    }
}

static psa_status_t gcm_finish(AesGcmContext_t *context, uint8_t *pTag)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    int rc;
    CCBuffInfo_t inBuffInfo;
    CCBuffInfo_t outBuffInfo;

    /* Set process mode to 'Process_GctrFinal' */
    context->processMode = DRV_AESGCM_Process_GctrFinal;

    /* set data buffers structures */
    rc = SetDataBuffersInfo((uint8_t *)(context->tempBuf),
                            CC_AESGCM_GHASH_DIGEST_SIZE_BYTES, &inBuffInfo,
                            context->preTagBuf,
                            CC_AESGCM_GHASH_DIGEST_SIZE_BYTES, &outBuffInfo);
    if (rc != 0) {
        CC_PAL_LOG_ERR("illegal data buffers\n");
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Calculate Encrypt and Calc. Tag */
    rc = ProcessAesGcm(context, &inBuffInfo, &outBuffInfo,
                       CC_AESGCM_GHASH_DIGEST_SIZE_BYTES);
    if (rc != AES_DRV_OK) {
        CC_PAL_LOG_ERR("Finish operation failed with error code %d\n", rc);
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (CRYPTO_DIRECTION_ENCRYPT == context->dir) {
        CC_PalMemCopy(pTag, context->preTagBuf, context->tagSize);
        status = PSA_SUCCESS;
    } else {
        if (CC_PalMemCmp(context->preTagBuf, pTag, context->tagSize) == 0) {
            status = PSA_SUCCESS;
        } else {
            status = PSA_ERROR_INVALID_ARGUMENT;
        }
    }

    return status;
}

/* TODO: Figure out best strategy for cc310 vs cc312 */
static psa_status_t gcm_crypt_and_tag(
    cryptoDirection_t direction, const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size, psa_algorithm_t alg,
    const uint8_t *nonce, size_t nonce_length, const uint8_t *additional_data,
    size_t additional_data_length, uint8_t *tag, size_t tag_length,
    const uint8_t *input, size_t input_length, uint8_t *output,
    size_t *output_length)
{
    psa_status_t status = PSA_ERROR_NOT_SUPPORTED;
    keySizeId_t temp_keySizeId;

    /* TODO: Do we want this on the stack? */
    AesGcmContext_t context;
    gcm_context_init(&context);

    status = gcm_set_key(&context, key_buffer, key_buffer_size * 8);
    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    /* Aes-GCM Initialization function */
    status = gcm_init(&context, direction, nonce, nonce_length, additional_data,
                      additional_data_length, input, input_length, output, tag,
                      tag_length);

    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    /* Aes-GCM Process J0 function */
    status = gcm_process_j0(&context, nonce);
    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    /* Aes-GCM Process AAD function */
    status = gcm_process_aad(&context, additional_data);
    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    /* Aes-GCM Process Cipher function */
    status = gcm_process_cipher(&context, input, output);
    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    /* Aes-GCM Process LenA||LenC function */
    status = gcm_process_lenA_lenC(&context);
    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    status = gcm_finish(&context, tag);
    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    /* TODO: Is there any more "correct" place to put this? */
    if (CRYPTO_DIRECTION_ENCRYPT == context.dir) {
        /* Ciphertext length should include tag */
        *output_length = input_length + tag_length;
    } else {
        /* Plaintext length should not include tag */
        *output_length = input_length;
    }

cleanup:
    if ((CRYPTO_DIRECTION_DECRYPT == context.dir) && (status != PSA_SUCCESS)) {
        CC_PalMemSetZero(output, context.dataSize);
    }

    /* Clear working context, except for keyBuf and keySizeId*/
    temp_keySizeId = context.keySizeId;
    /* Key is the first thing in the context, erase the rest*/
    CC_PalMemSetZero(context.H,
                     (sizeof(AesGcmContext_t) - offsetof(AesGcmContext_t, H)));
    /* copy back the key size */
    context.keySizeId = temp_keySizeId;

    /* TODO: We simply clear the context here, so is the above work necessary?
     */
    gcm_context_free(&context);

    return status;
}

/* TODO: Figure out best strategy for cc310 vs cc312 */
psa_status_t cc3xx_psa_aead_encrypt_gcm(
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg, const uint8_t *nonce,
    size_t nonce_length, const uint8_t *additional_data,
    size_t additional_data_length, const uint8_t *plaintext,
    size_t plaintext_length, uint8_t *ciphertext, size_t ciphertext_size,
    size_t *ciphertext_length)
{
    psa_status_t status = PSA_ERROR_NOT_SUPPORTED;

    psa_key_type_t key_type = psa_get_key_type(attributes);
    psa_key_bits_t key_bits = psa_get_key_bits(attributes);
    psa_algorithm_t key_alg = psa_get_key_algorithm(attributes);

    size_t tag_length = PSA_AEAD_TAG_LENGTH(key_type, key_bits, key_alg);

    /*
     * The ciphertext will be of the same length as the plaintext input,
     * and the tag will be placed after that.
     */
    uint8_t *tag = ciphertext + plaintext_length;

    status = gcm_crypt_and_tag(
        CRYPTO_DIRECTION_ENCRYPT, attributes, key_buffer, key_buffer_size, alg,
        nonce, nonce_length, additional_data, additional_data_length, tag,
        tag_length, plaintext, plaintext_length, ciphertext, ciphertext_length);

    return status;
}

psa_status_t cc3xx_psa_aead_decrypt_gcm(
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg, const uint8_t *nonce,
    size_t nonce_length, const uint8_t *additional_data,
    size_t additional_data_length, const uint8_t *ciphertext,
    size_t ciphertext_length, uint8_t *plaintext, size_t plaintext_size,
    size_t *plaintext_length)
{
    psa_status_t status = PSA_ERROR_NOT_SUPPORTED;

    uint8_t local_tag_buffer[PSA_AEAD_TAG_MAX_SIZE];

    psa_key_type_t key_type = psa_get_key_type(attributes);
    psa_key_bits_t key_bits = psa_get_key_bits(attributes);
    psa_algorithm_t key_alg = psa_get_key_algorithm(attributes);

    size_t tag_length = PSA_AEAD_TAG_LENGTH(key_type, key_bits, key_alg);

    size_t ciphertext_length_without_tag = ciphertext_length - tag_length;
    const uint8_t *tag = ciphertext + ciphertext_length_without_tag;

    CC_PalMemCopy(local_tag_buffer, tag, tag_length);

    status = gcm_crypt_and_tag(
        CRYPTO_DIRECTION_DECRYPT, attributes, key_buffer, key_buffer_size, alg,
        nonce, nonce_length, additional_data, additional_data_length,
        local_tag_buffer, tag_length, ciphertext, ciphertext_length_without_tag,
        plaintext, plaintext_length);

    return status;
}
