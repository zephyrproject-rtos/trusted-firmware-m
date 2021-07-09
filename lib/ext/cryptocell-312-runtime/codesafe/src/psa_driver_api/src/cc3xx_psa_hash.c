/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cc3xx_psa_hash.h"
#include "cc_pal_log.h"
#include "cc_pal_mem.h"

#define MAX_HASH_CHUNK_SIZE 0xffff

/*!
 * \defgroup private Private functions
 *
 */
/*!@{*/

/**
 * \brief  Update a hash operation
 *
 * In CC312, DMA (i.e. for hash module input ) needs access to physical and
 * continues memory. In order to assure the DMA can access the residue data saved
 * in the hashCtx, it is being to copied to a local stack variable. In case memory
 * is guaranteed to be DMAable, this copy can be removed, and hashCtx->prevData can
 * be used.
 */
static psa_status_t hashUpdate(cc3xx_hash_operation_t *pHashUserCtx,
                               uint8_t *pDataIn, size_t dataInSize)
{
    uint32_t rc       = 0;
    size_t bytesToAdd = 0;
    uint32_t localPrevDataIn[HASH_SHA512_BLOCK_SIZE_IN_WORDS];
    CCBuffInfo_t inBuffInfo;

    /* If pHashUserCtx->prevDataInSize > 0, fill it with with the current data
     */
    bytesToAdd = CC_MIN(
        ((pHashUserCtx->blockSizeInBytes - pHashUserCtx->prevDataInSize) %
         pHashUserCtx->blockSizeInBytes),
        dataInSize);
    if (bytesToAdd > 0) {
        /* add the data to the remaining buffer */
        CC_PalMemCopy(
            &(((uint8_t *)(pHashUserCtx
                               ->prevDataIn))[pHashUserCtx->prevDataInSize]),
            pDataIn, bytesToAdd);
        pHashUserCtx->prevDataInSize += bytesToAdd;
        pDataIn += bytesToAdd;
        dataInSize -= bytesToAdd;
    }

    /* If the remaining buffer is full, process the block (else, the remaining
    buffer will be processed in the next update or finish) */
    if (pHashUserCtx->prevDataInSize == pHashUserCtx->blockSizeInBytes) {
        /* Copy prevDataIn to stack, in order to ensure continues and physical
        memory access. That way, DMA will be able to access the data on any
        platform.*/
        CC_PalMemCopy(localPrevDataIn, pHashUserCtx->prevDataIn,
                      CC_MIN(HASH_SHA512_BLOCK_SIZE_IN_WORDS * sizeof(uint32_t),
                             pHashUserCtx->prevDataInSize));

        rc = SetDataBuffersInfo((uint8_t *)localPrevDataIn,
                                pHashUserCtx->blockSizeInBytes, &inBuffInfo,
                                NULL, 0, NULL);
        if (rc != 0) {
            CC_PAL_LOG_ERR("illegal data buffers\n");
            return PSA_ERROR_GENERIC_ERROR;
        }

        rc = ProcessHashDrv(pHashUserCtx, &inBuffInfo,
                            pHashUserCtx->blockSizeInBytes);
        if (rc != CC_OK) {
            CC_PAL_LOG_ERR("ProcessHashDrv failed, ret = %d\n", rc);
            return PSA_ERROR_GENERIC_ERROR;
        }
        pHashUserCtx->prevDataInSize = 0;
    }

    /* Process all the blocks that remain in the data */
    bytesToAdd = (dataInSize / pHashUserCtx->blockSizeInBytes) *
                 pHashUserCtx->blockSizeInBytes;
    if (bytesToAdd > 0) {

        rc =
            SetDataBuffersInfo(pDataIn, bytesToAdd, &inBuffInfo, NULL, 0, NULL);
        if (rc != 0) {
            CC_PAL_LOG_ERR("illegal data buffers\n");
            return PSA_ERROR_GENERIC_ERROR;
        }

        rc = ProcessHashDrv(pHashUserCtx, &inBuffInfo, bytesToAdd);
        if (rc != CC_OK) {
            CC_PAL_LOG_ERR("ProcessHashDrv failed, ret = %d\n", rc);
            return PSA_ERROR_GENERIC_ERROR;
        }
        pDataIn += bytesToAdd;
        dataInSize -= bytesToAdd;
    }

    /* Copy the remaining partial block to prevDataIn */
    bytesToAdd = dataInSize;
    if (bytesToAdd > 0) {
        CC_PalMemCopy(
            (uint8_t *)&(
                (pHashUserCtx->prevDataIn)[pHashUserCtx->prevDataInSize]),
            pDataIn, bytesToAdd);
        pHashUserCtx->prevDataInSize += bytesToAdd;
    }
    return PSA_SUCCESS;
}

/*!@}*/

psa_status_t cc3xx_hash_setup(cc3xx_hash_operation_t *operation,
                              psa_algorithm_t alg)
{
    /* Init context */
    HashContext_t *pHashCtx = NULL;

    if (NULL == operation) {
        CC_PAL_LOG_ERR("hash operation is NULL\n");
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    pHashCtx = (HashContext_t *)operation;
    CC_PalMemSetZero(pHashCtx, sizeof(HashContext_t));
    switch (alg) {
#if defined MBEDTLS_SHA1_C
    case PSA_ALG_SHA_1:
        pHashCtx->mode = HASH_SHA1;
        break;
#endif
#if defined MBEDTLS_SHA256_C
    case PSA_ALG_SHA_224:
        pHashCtx->mode = HASH_SHA224;
        break;
    case PSA_ALG_SHA_256:
        pHashCtx->mode = HASH_SHA256;
        break;
#endif
    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }
    pHashCtx->blockSizeInBytes = HASH_BLOCK_SIZE_IN_BYTES;
    drvError_t ret             = InitHashDrv(operation);
    if (ret == HASH_DRV_OK) {
        return PSA_SUCCESS;
    } else { /* change PSA error code? */
        return PSA_ERROR_INVALID_ARGUMENT;
    }
}

psa_status_t cc3xx_hash_clone(const cc3xx_hash_operation_t *source_operation,
                              cc3xx_hash_operation_t *target_operation)
{
    if (source_operation == NULL || target_operation == NULL) {
        CC_PAL_LOG_ERR("source or target operation is NULL\n");
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    CC_PalMemCopy(target_operation, source_operation,
                  sizeof(cc3xx_hash_operation_t));
    return PSA_SUCCESS;
}

psa_status_t cc3xx_hash_update(cc3xx_hash_operation_t *operation,
                               const uint8_t *input, size_t input_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    if (NULL == operation) {
        CC_PAL_LOG_ERR("ctx is NULL\n");
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (0 == input_length) {
        /* This is a valid situation, no need to call hashUpdate.
        HashFinish will produce the result. */
        return PSA_SUCCESS;
    }

    /* if len not zero, but pointer is NULL */
    if (NULL == input) {
        CC_PAL_LOG_ERR("input is NULL\n");
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    while (input_length > MAX_HASH_CHUNK_SIZE) {
        status = hashUpdate(operation, (uint8_t *)input, MAX_HASH_CHUNK_SIZE);

        input_length -= MAX_HASH_CHUNK_SIZE;
        input += MAX_HASH_CHUNK_SIZE;

        if (status != PSA_SUCCESS) {
            CC_PAL_LOG_ERR("hashUpdate failed, status = %d\n", status);
            return status;
        }
    }

    status = hashUpdate(operation, (uint8_t *)input, input_length);
    if (status != PSA_SUCCESS) {
        CC_PAL_LOG_ERR("hashUpdate failed, status = %d\n", status);
        return status;
    }

    return status;
}

psa_status_t cc3xx_hash_finish(cc3xx_hash_operation_t *operation, uint8_t *hash,
                               size_t hash_size, size_t *hash_length)
{
    uint32_t localPrevDataIn[HASH_SHA512_BLOCK_SIZE_IN_WORDS];
    size_t dataInSize = 0;
    drvError_t drvRc  = HASH_DRV_OK;
    CCBuffInfo_t inBuffInfo;
    HashContext_t *pHashCtx = NULL;

    if (operation == NULL || hash == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    pHashCtx = (HashContext_t *)operation;
    if (pHashCtx->prevDataInSize != 0) {
        /* Copy prevDataIn to stack, in order to ensure continues and physical
        memory access. That way, DMA will be able to access the data on any
        platform.*/
        CC_PalMemCopy(localPrevDataIn, pHashCtx->prevDataIn,
                      CC_MIN(HASH_SHA512_BLOCK_SIZE_IN_WORDS * sizeof(uint32_t),
                             pHashCtx->prevDataInSize));
        dataInSize = pHashCtx->prevDataInSize;
    }
    pHashCtx->isLastBlockProcessed = 1;

    drvRc = SetDataBuffersInfo((uint8_t *)localPrevDataIn, dataInSize,
                               &inBuffInfo, NULL, 0, NULL);
    if (drvRc != 0) {
        CC_PAL_LOG_ERR("illegal data buffers\n");
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    drvRc = ProcessHashDrv(pHashCtx, &inBuffInfo, dataInSize);
    if (drvRc != HASH_DRV_OK) {
        CC_PAL_LOG_ERR("ProcessHashDrv failed, ret = %d\n", drvRc);
        return PSA_ERROR_GENERIC_ERROR;
    }
    drvRc = FinishHashDrv(pHashCtx);
    if (drvRc != HASH_DRV_OK) {
        CC_PAL_LOG_ERR("FinishHashDrv failed, ret = %d\n", drvRc);
        return PSA_ERROR_GENERIC_ERROR;
    }
    pHashCtx->prevDataInSize = 0;

    /* Copy the result to the user buffer  */
    switch (pHashCtx->mode) {
    case HASH_SHA1:
        if (SHA1_DIGEST_SIZE_IN_BYTES > hash_size) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }
        CC_PalMemCopy(hash, pHashCtx->digest, SHA1_DIGEST_SIZE_IN_BYTES);
        *hash_length = SHA1_DIGEST_SIZE_IN_BYTES;
        break;
    case HASH_SHA224:
        if (SHA224_DIGEST_SIZE_IN_BYTES > hash_size) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }
        CC_PalMemCopy(hash, pHashCtx->digest, SHA224_DIGEST_SIZE_IN_BYTES);
        *hash_length = SHA224_DIGEST_SIZE_IN_BYTES;
        break;
    case HASH_SHA256:
        if (SHA256_DIGEST_SIZE_IN_BYTES > hash_size) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }
        CC_PalMemCopy(hash, pHashCtx->digest, SHA256_DIGEST_SIZE_IN_BYTES);
        *hash_length = SHA256_DIGEST_SIZE_IN_BYTES;
        break;
    default:
        CC_PAL_LOG_ERR("Unsupported HASH type (%d)\n", pHashCtx->mode);
    }
    return PSA_SUCCESS;
}

psa_status_t cc3xx_hash_abort(cc3xx_hash_operation_t *operation)
{
    /* The object has not been used yet, there is nothing to do */
    if (operation->mode == 0) {
        return PSA_SUCCESS;
    }

    CC_PalMemSetZero(operation, sizeof(cc3xx_hash_operation_t));
    return PSA_SUCCESS;
}

psa_status_t cc3xx_hash_compute(psa_algorithm_t alg, const uint8_t *input,
                                size_t input_length, uint8_t *hash,
                                size_t hash_size, size_t *hash_length)
{
    psa_status_t status              = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t abort_status        = PSA_ERROR_CORRUPTION_DETECTED;
    cc3xx_hash_operation_t operation = {0};

    *hash_length = hash_size;
    status       = cc3xx_hash_setup(&operation, alg);
    if (status != PSA_SUCCESS) {
        goto exit;
    }
    status = cc3xx_hash_update(&operation, input, input_length);
    if (status != PSA_SUCCESS) {
        goto exit;
    }
    status = cc3xx_hash_finish(&operation, hash, hash_size, hash_length);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

exit:
    *hash_length = 0;
    abort_status = cc3xx_hash_abort(&operation);
    if (status == PSA_SUCCESS) {
        return (abort_status);
    } else {
        return (status);
    }
}
