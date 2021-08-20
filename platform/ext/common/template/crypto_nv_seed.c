/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include "tfm_plat_crypto_nv_seed.h"
#include "psa/internal_trusted_storage.h"

#ifndef TFM_PSA_API
#include "mbedtls/entropy.h"

static unsigned char seed_buf[MBEDTLS_ENTROPY_BLOCK_SIZE];

/*
 \brief Copy the seed to the destination buffer

 \param[out]  p_dst  Pointer to buffer where to store the seed
 \param[in]   p_src  Pointer to the seed
 \param[in]   size   Length of the seed
*/
static inline void copy_seed(uint8_t *p_dst, const uint8_t *p_src, size_t size)
{
    uint32_t i;

    for (i = size; i > 0; i--) {
        *p_dst = *p_src;
        p_src++;
        p_dst++;
    }
}
#endif

int tfm_plat_crypto_nv_seed_read(unsigned char *buf, size_t buf_len)
{
#ifdef TFM_PSA_API
    psa_storage_uid_t uid = NV_SEED_FILE_ID;
    psa_status_t status;
    size_t data_length = 0;

    status = psa_its_get(uid, 0, buf_len, buf, &data_length);

    if (status == PSA_SUCCESS && data_length == buf_len) {
        return TFM_CRYPTO_NV_SEED_SUCCESS;
    } else {
        return TFM_CRYPTO_NV_SEED_FAILED;
    }
#else
    if (buf_len != MBEDTLS_ENTROPY_BLOCK_SIZE) {
        return TFM_CRYPTO_NV_SEED_FAILED;
    } else {
        copy_seed(buf, seed_buf, buf_len);
        return TFM_CRYPTO_NV_SEED_SUCCESS;
    }
#endif
}

int tfm_plat_crypto_nv_seed_write(const unsigned char *buf, size_t buf_len)
{
#ifdef TFM_PSA_API
    psa_storage_uid_t uid = NV_SEED_FILE_ID;
    psa_status_t status;

    status = psa_its_set(uid, buf_len, buf, 0);

    if (status == PSA_SUCCESS) {
        return TFM_CRYPTO_NV_SEED_SUCCESS;
    } else {
        return TFM_CRYPTO_NV_SEED_FAILED;
    }
#else
    if (buf_len != MBEDTLS_ENTROPY_BLOCK_SIZE) {
        return TFM_CRYPTO_NV_SEED_FAILED;
    } else {
        copy_seed(seed_buf, buf, buf_len);
        return TFM_CRYPTO_NV_SEED_SUCCESS;
    }
#endif
}
