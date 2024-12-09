/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef BL1_1_CRYPTO_H
#define BL1_1_CRYPTO_H

#include <stddef.h>
#include <stdint.h>

#define CTR_IV_LEN 16

#include "psa/crypto.h"
#include "crypto_key_defs.h"
#include "fih.h"

#ifdef __cplusplus
extern "C" {
#endif

enum tfm_bl1_key_type_t {
    TFM_BL1_KEY_TYPE_ECDSA = 0b00,
    TFM_BL1_KEY_TYPE_LMS   = 0b01,
};

enum tfm_bl1_key_policy_t {
    TFM_BL1_KEY_MIGHT_SIGN = 0b00,
    TFM_BL1_KEY_MUST_SIGN  = 0b01,
};

enum tfm_bl1_ecdsa_curve_t {
    TFM_BL1_CURVE_P256,
    TFM_BL1_CURVE_P384,
};

enum tfm_bl1_hash_alg_t {
    TFM_BL1_HASH_ALG_SHA256 = PSA_ALG_SHA_256,
    TFM_BL1_HASH_ALG_SHA384 = PSA_ALG_SHA_384,
};


/* Calculates a hash of the input data */
fih_int bl1_hash_compute(enum tfm_bl1_hash_alg_t alg,
                         const uint8_t *data,
                         size_t data_length,
                         uint8_t *hash,
                         size_t hash_length,
                         size_t *hash_size);

fih_int bl1_hash_init(enum tfm_bl1_hash_alg_t alg);

fih_int bl1_hash_update(const uint8_t *data,
                        size_t data_length);

fih_int bl1_hash_finish(uint8_t *hash,
                        size_t hash_length,
                        size_t *hash_size);


/* Performs AES-256-CTR decryption */
int32_t bl1_aes_256_ctr_decrypt(enum tfm_bl1_key_id_t key_id,
                                const uint8_t *key_material,
                                uint8_t *counter,
                                const uint8_t *ciphertext,
                                size_t ciphertext_length,
                                uint8_t *plaintext);

/* Derives key material from a BL1 key and some label and context. Any
 * cryptographically secure key derivation algorithm is acceptable.
 */
int32_t bl1_derive_key(enum tfm_bl1_key_id_t input_key, const uint8_t *label,
                       size_t label_length, const uint8_t *context,
                       size_t context_length, uint8_t *output_key,
                       size_t output_length);

fih_int bl1_ecdsa_verify(enum tfm_bl1_ecdsa_curve_t curve,
                         uint8_t *key, size_t key_size,
                         const uint8_t *hash,
                         size_t hash_length,
                         const uint8_t *signature,
                         size_t signature_size);

#ifdef __cplusplus
}
#endif

#endif /* BL1_1_CRYPTO_H */
