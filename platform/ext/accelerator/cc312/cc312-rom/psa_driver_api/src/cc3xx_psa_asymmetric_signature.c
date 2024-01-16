/*
 * Copyright (c) 2021-2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/** @file cc3xx_psa_asymmetric_signature.c
 *
 * This file contains the implementation of the entry points associated to the
 * asymmetric signature capability as described by the PSA Cryptoprocessor
 * Driver interface specification
 *
 */

#include "cc3xx_psa_asymmetric_signature.h"

/* ToDo: This needs to be sorted out at TF-M level
 * To be able to include the PSA style configuration
 */
#include "mbedtls/build_info.h"

/** @defgroup psa_asym_sign PSA driver entry points for asymmetric sign/verify
 *
 *  Entry points for asymmetric message signing and signature verification as
 *  described by the PSA Cryptoprocessor Driver interface specification
 *
 *  @{
 */
psa_status_t cc3xx_sign_hash(const psa_key_attributes_t *attributes,
                             const uint8_t *key, size_t key_length,
                             psa_algorithm_t alg, const uint8_t *input,
                             size_t input_length, uint8_t *signature,
                             size_t signature_size, size_t *signature_length)
{
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t cc3xx_verify_hash(const psa_key_attributes_t *attributes,
                               const uint8_t *key, size_t key_length,
                               psa_algorithm_t alg, const uint8_t *hash,
                               size_t hash_length, const uint8_t *signature,
                               size_t signature_length)
{
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t cc3xx_sign_message(const psa_key_attributes_t *attributes,
                                const uint8_t *key, size_t key_length,
                                psa_algorithm_t alg, const uint8_t *input,
                                size_t input_length, uint8_t *signature,
                                size_t signature_size, size_t *signature_length)
{
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t cc3xx_verify_message(const psa_key_attributes_t *attributes,
                                  const uint8_t *key, size_t key_length,
                                  psa_algorithm_t alg, const uint8_t *input,
                                  size_t input_length, const uint8_t *signature,
                                  size_t signature_length)
{
    return PSA_ERROR_NOT_SUPPORTED;
}
/** @} */ // end of psa_asym_sign
