/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "crypto_hw.h"

#include "cc_lib.h"
#include "cc_pal_buff_attr.h"
#include "cc_rnd_common.h"
#include "mbedtls/platform.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls_cc_mng_int.h"
#include "arm_cmse.h"
#include "mbedtls_cc_util_key_derivation.h"
#include "tfm_attest_hal.h"

#define CC312_NULL_CONTEXT "NO SALT!"

CCRndContext_t*           CC312_pRndCtx         = NULL;
CCRndWorkBuff_t*          CC312_pRndWorkBuff    = NULL;
mbedtls_ctr_drbg_context* CC312_pRndState       = NULL;
mbedtls_entropy_context*  CC312_pMbedtlsEntropy = NULL;

CCError_t CC_PalDataBufferAttrGet(const unsigned char *pDataBuffer,
                                  size_t buffSize, uint8_t buffType,
                                  uint8_t *pBuffNs)
{
    CC_UNUSED_PARAM(buffType);

    *pBuffNs = DATA_BUFFER_IS_SECURE;
    if (cmse_check_address_range((void*)pDataBuffer, buffSize, CMSE_NONSECURE)) {
        *pBuffNs = DATA_BUFFER_IS_NONSECURE;
    }

    return CC_OK;
}

/*
 * \brief Initialize the CC312 crypto accelerator
 */

int crypto_hw_accelerator_init(void)
{
    int ret = 0;

    /* Allocate memory on heap */
    CC312_pRndCtx         = mbedtls_calloc(1, sizeof(CCRndContext_t));
    CC312_pRndWorkBuff    = mbedtls_calloc(1, sizeof(CCRndWorkBuff_t));
    CC312_pRndState       = mbedtls_calloc(1, sizeof(mbedtls_ctr_drbg_context));
    CC312_pMbedtlsEntropy = mbedtls_calloc(1, sizeof(mbedtls_entropy_context));

    /* Check if memory allocation was successful */
    if ( !CC312_pRndCtx || !CC312_pRndWorkBuff
      || !CC312_pRndState || !CC312_pMbedtlsEntropy) {
        mbedtls_free(CC312_pRndCtx);
        mbedtls_free(CC312_pRndWorkBuff);
        mbedtls_free(CC312_pRndState);
        mbedtls_free(CC312_pMbedtlsEntropy);

        return -1;
    }

    /* Init Rnd context's inner members */
    CC312_pRndCtx->rndState   = CC312_pRndState;
    CC312_pRndCtx->entropyCtx = CC312_pMbedtlsEntropy;

    /* Initialise CryptoCell library */
    ret = CC_LibInit(CC312_pRndCtx, CC312_pRndWorkBuff);
    if (ret != CC_LIB_RET_OK) {
        mbedtls_free(CC312_pRndCtx);
        mbedtls_free(CC312_pRndWorkBuff);
        mbedtls_free(CC312_pRndState);
        mbedtls_free(CC312_pMbedtlsEntropy);

        return ret;
    }

    return 0;
}

/*
 * \brief Deallocate the CC312 crypto accelerator
 */
int crypto_hw_accelerator_finish(void)
{
    int ret = 0;

    ret = CC_LibFini(CC312_pRndCtx);
    if(ret != CC_LIB_RET_OK) {
        return ret;
    }

    mbedtls_free(CC312_pRndCtx);
    mbedtls_free(CC312_pRndWorkBuff);
    mbedtls_free(CC312_pRndState);
    mbedtls_free(CC312_pMbedtlsEntropy);

    return 0;
}

int crypto_hw_accelerator_get_lcs(uint32_t *lcs)
{
    return mbedtls_mng_lcsGet(lcs);
}

int crypto_hw_accelerator_huk_derive_key(const uint8_t *label,
                                         size_t label_size,
                                         const uint8_t *context,
                                         size_t context_size,
                                         uint8_t *key,
                                         size_t key_size)
{

    if (context == NULL || context_size == 0) {
        /* The CC312 requires the context to not be null, so a default
         * is given.
         */
        context = (const uint8_t *)CC312_NULL_CONTEXT;
        context_size = sizeof(CC312_NULL_CONTEXT);
    }

    return mbedtls_util_key_derivation_cmac(CC_UTIL_ROOT_KEY, NULL,
                                            label, label_size,
                                            context, context_size,
                                            key, key_size);

}

int crypto_hw_accelerator_get_rotpk_hash(uint8_t image_id,
                                         uint8_t *rotpk_hash,
                                         uint32_t *rotpk_hash_size)
{
    int32_t ret;
    mbedtls_mng_pubKeyType_t key_index;
    uint32_t rotpk_hash_size_in_words;

    if (image_id == 0) {
#if (MCUBOOT_IMAGE_NUMBER == 1)
        key_index = CC_MNG_HASH_BOOT_KEY_256B;
        rotpk_hash_size_in_words = 8;
#elif (MCUBOOT_IMAGE_NUMBER == 2)
        key_index = CC_MNG_HASH_BOOT_KEY_0_128B;
        rotpk_hash_size_in_words = 4;
    } else if (image_id == 1) {
        key_index = CC_MNG_HASH_BOOT_KEY_1_128B;
        rotpk_hash_size_in_words = 4;
#endif /* MCUBOOT_IMAGE_NUMBER == 1 */
    } else {
        return -1;
    }

    if (*rotpk_hash_size < rotpk_hash_size_in_words * sizeof(uint32_t)) {
        return -1;
    }
    *rotpk_hash_size = rotpk_hash_size_in_words * sizeof(uint32_t);

    ret = mbedtls_mng_pubKeyHashGet(key_index, (uint32_t *)rotpk_hash,
                                    rotpk_hash_size_in_words);
    if (ret) {
        return ret;
    }

    return 0;
}
