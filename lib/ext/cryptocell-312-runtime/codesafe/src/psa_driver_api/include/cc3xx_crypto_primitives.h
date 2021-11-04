/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/** \file cc3xx_crypto_primitives.h
 *
 * This file contains the declaration of public types associated
 * to the CC3XX driver
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

/*!
 * \struct cc3xx_hash_operation_s
 *
 * \brief A structure holding state information for an Hash operation
 */
struct cc3xx_hash_operation_s {
    HashContext_t ctx; /*!< Low-level hash context */
};

typedef struct cc3xx_hash_operation_s cc3xx_hash_operation_t;

/*!
 * \struct cc3xx_cipher_operation_s
 *
 * \brief A structure holding state information for an Cipher operation
 */
struct cc3xx_cipher_operation_s {
    psa_algorithm_t alg;          /*!< Cipher algorithm used in this context */
    psa_key_type_t key_type;      /*!< Key type associated to the context */
    psa_encrypt_or_decrypt_t dir; /*!< Encrypting or decrypting direction */
    size_t block_size;            /*!< Block size of the underlying cipher */

    psa_status_t(*add_padding)(uint8_t *, size_t, size_t); /*!< Padding func */

    uint8_t unprocessed_data[AES_BLOCK_SIZE]; /*!< Any data still to process */
    size_t  unprocessed_size; /*!< Size of the data in unprocesse_data */
    uint8_t iv[AES_IV_SIZE];  /*!< Initialisation Vector */
    size_t  iv_size;          /*!< Size of the IV */

    union {
        AesContext_t    aes;     /*!< Low-level AES context */
        AesGcmContext_t aes_gcm; /*!< Low-level AES GCM context */
        AesCcmContext_t aes_ccm; /*!< Low-level AES CCM context */
        ChachaContext_t chacha;  /*!< Low-level Chacha20 context */
    } ctx;
};

typedef struct cc3xx_cipher_operation_s cc3xx_cipher_operation_t;

/*!
 * \struct cc3xx_mac_operation_s
 *
 * \brief A structure holding state information for a MAC operation
 */
struct cc3xx_mac_operation_s {
    psa_algorithm_t alg; /*!< MAC algorithm used in this context */
    union {
        cc3xx_cipher_operation_t cmac; /*!< Underlying cipher op for CMAC */
        cc3xx_hash_operation_t hmac;   /*!< Underlying hash op for HMAC */
    };
    /* Only for HMAC */
    uint8_t opad[PSA_HMAC_MAX_HASH_BLOCK_SIZE]; /*!< Opad as RFC-2104 */
};

typedef struct cc3xx_mac_operation_s cc3xx_mac_operation_t;

#ifdef __cplusplus
}
#endif
#endif /* CC3XX_CRYPTO_PRIMITIVES_H */
