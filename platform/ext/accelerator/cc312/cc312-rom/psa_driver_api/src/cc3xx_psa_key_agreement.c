/*
 * Copyright (c) 2021-2024, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/** @file cc3xx_psa_key_agreement.c
 *
 * This file contains the implementation of the entry points associated to the
 * raw key agreement (i.e. ECDH) as described by the PSA Cryptoprocessor Driver
 * interface specification
 *
 */

#include "cc3xx_psa_key_agreement.h"

/* ToDo: This needs to be sorted out at TF-M level
 * To be able to include the PSA style configuration
 */
#include "mbedtls/build_info.h"

/** @defgroup psa_key_agreement PSA driver entry points for raw key agreement
 *
 *  Entry points for raw key agreement as described by the PSA Cryptoprocessor
 *  Driver interface specification
 *
 *  @{
 */
psa_status_t cc3xx_key_agreement(
        const psa_key_attributes_t *attributes,
        const uint8_t *priv_key, size_t priv_key_size,
        const uint8_t *publ_key, size_t publ_key_size,
        uint8_t *output, size_t output_size, size_t *output_length,
        psa_algorithm_t alg)
{
    return PSA_ERROR_NOT_SUPPORTED;
}
/** @} */ // end of psa_key_agreement
