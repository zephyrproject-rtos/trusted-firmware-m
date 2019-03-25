/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_CRYPTO_DEFS_H__
#define __TFM_CRYPTO_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <limits.h>
#include "tfm_api.h"
#include "psa_crypto.h"

/**
 * \brief This value is used to mark an handle as invalid.
 *
 */
#define TFM_CRYPTO_INVALID_HANDLE (0xFFFFFFFF)

/**
 * \brief Define miscellaneous literal constants that are used in the service
 *
 */
enum {
    TFM_CRYPTO_NOT_IN_USE = 0,
    TFM_CRYPTO_IN_USE = 1
};

/**
 * \brief This type is used to overcome a limitation in the number of maximum
 *        IOVECs that can be used especially in psa_aead_encrypt and
 *        psa_aead_decrypt. To be removed in case the AEAD APIs number of
 *        parameters passed gets restructured
 */
#define TFM_CRYPTO_MAX_NONCE_LENGTH (16u)
struct tfm_crypto_aead_pack_input {
    psa_key_slot_t key;
    psa_algorithm_t alg;
    uint8_t nonce[TFM_CRYPTO_MAX_NONCE_LENGTH];
};

#ifdef __cplusplus
}
#endif

#endif /* __TFM_CRYPTO_DEFS_H__ */
