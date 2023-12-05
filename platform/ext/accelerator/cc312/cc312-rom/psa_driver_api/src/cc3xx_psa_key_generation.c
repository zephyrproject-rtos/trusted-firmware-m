/*
 * Copyright (c) 2021-2024, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/** @file cc3xx_psa_key_generation.c
 *
 * This file contains the implementation of the entry points associated to the
 * key generation (i.e. random generation and extraction of public keys) as
 * described by the PSA Cryptoprocessor Driver interface specification
 *
 */

#include "cc3xx_psa_key_generation.h"
#include "cc3xx_misc.h"

/* ToDo: This needs to be sorted out at TF-M level
 * To be able to include the PSA style configuration
 */
#include "mbedtls/build_info.h"

/** @defgroup psa_key_generation PSA driver entry points for key handling
 *
 *  Entry points for random key generation and key format manipulation and
 *  translation as described by the PSA Cryptoprocessor Driver interface
 *  specification
 *
 *  @{
 */
psa_status_t cc3xx_generate_key(const psa_key_attributes_t *attributes,
                                uint8_t *key_buffer, size_t key_buffer_size,
                                size_t *key_buffer_length)
{
    psa_key_type_t key_type = psa_get_key_type(attributes);
    psa_status_t err = PSA_ERROR_NOT_SUPPORTED;

    CC3XX_ASSERT(attributes != NULL);
    CC3XX_ASSERT(key_buffer != NULL);
    CC3XX_ASSERT(key_buffer_length != NULL);

    /* Initialise the return value to 0 */
    *key_buffer_length = 0;

    if (PSA_KEY_TYPE_IS_UNSTRUCTURED(key_type)) {
        err = mbedtls_psa_external_get_random(NULL, key_buffer, key_buffer_size, key_buffer_length);
    }

    return err;
}

psa_status_t cc3xx_export_public_key(const psa_key_attributes_t *attributes,
                                     const uint8_t *key_buffer,
                                     size_t key_buffer_size, uint8_t *data,
                                     size_t data_size, size_t *data_length)
{
    return PSA_ERROR_NOT_SUPPORTED;
}
/** @} */ // end of psa_key_generation
