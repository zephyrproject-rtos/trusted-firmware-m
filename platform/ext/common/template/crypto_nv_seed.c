/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include "tfm_plat_crypto_nv_seed.h"
#include "psa/internal_trusted_storage.h"

int tfm_plat_crypto_nv_seed_read(unsigned char *buf, size_t buf_len)
{
    psa_storage_uid_t uid = NV_SEED_FILE_ID;
    psa_status_t status;
    size_t data_length = 0;

    status = psa_its_get(uid, 0, buf_len, buf, &data_length);

    if (status == PSA_SUCCESS && data_length == buf_len) {
        return TFM_CRYPTO_NV_SEED_SUCCESS;
    } else {
        return TFM_CRYPTO_NV_SEED_FAILED;
    }
}

int tfm_plat_crypto_nv_seed_write(const unsigned char *buf, size_t buf_len)
{
    psa_storage_uid_t uid = NV_SEED_FILE_ID;
    psa_status_t status;

    status = psa_its_set(uid, buf_len, buf, 0);

    if (status == PSA_SUCCESS) {
        return TFM_CRYPTO_NV_SEED_SUCCESS;
    } else {
        return TFM_CRYPTO_NV_SEED_FAILED;
    }
}
