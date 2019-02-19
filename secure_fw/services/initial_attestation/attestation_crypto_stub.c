/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_memory_utils.h"
#include "tfm_crypto_defs.h"
#include "psa_crypto.h"

/**
 * Stub implementation to substitute missing API from Crypto service. It just
 * copies twice the incoming hash value to the signature buffer. In this way it
 * simulates a real ECDSA P-256 over SHA256 signature generation. The size of
 * the signature will be equal with a real one.
 */
psa_status_t psa_asymmetric_sign(psa_key_slot_t key,
                                 psa_algorithm_t alg,
                                 const uint8_t *hash,
                                 size_t hash_length,
                                 uint8_t *signature,
                                 size_t signature_size,
                                 size_t *signature_length)
{
    if (2 * hash_length > signature_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    tfm_memcpy(signature, hash, hash_length);
    tfm_memcpy(signature + hash_length, hash, hash_length);
    *signature_length = hash_length;

    return PSA_SUCCESS;
}
