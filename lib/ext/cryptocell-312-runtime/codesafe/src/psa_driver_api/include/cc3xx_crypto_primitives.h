/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_CRYPTO_PRIMITIVES_H
#define CC3XX_CRYPTO_PRIMITIVES_H

#include "psa/crypto_driver_common.h"
#include "psa/crypto_sizes.h"
#include "psa/crypto_types.h"

#include "hash_driver.h"
#include "aes_driver.h"
#include "aesgcm_driver.h"
#include "aesccm_driver.h"
#include "chacha_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    HashContext_t ctx;
} cc3xx_hash_operation_t;

typedef struct {
    psa_algorithm_t alg;
    psa_key_type_t key_type;
    psa_encrypt_or_decrypt_t dir;
    size_t block_size;

    psa_status_t(*add_padding)(uint8_t *, size_t, size_t);

    uint8_t unprocessed_data[AES_BLOCK_SIZE];
    size_t  unprocessed_size;
    uint8_t iv[AES_IV_SIZE];
    size_t  iv_size;

    union {
        AesContext_t    aes;
        AesGcmContext_t aes_gcm;
        AesCcmContext_t aes_ccm;
        ChachaContext_t chacha;
    } ctx;
} cc3xx_cipher_operation_t;

typedef struct {
    psa_algorithm_t alg;
    union {
        cc3xx_cipher_operation_t cmac;
        cc3xx_hash_operation_t hmac;
    };
    /* Only for HMAC */
    uint8_t opad[PSA_HMAC_MAX_HASH_BLOCK_SIZE];
} cc3xx_mac_operation_t;

#ifdef __cplusplus
}
#endif
#endif /* CC3XX_CRYPTO_PRIMITIVES_H */
