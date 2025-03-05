/*
 * Copyright (c) 2023-2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/** @file cc3xx_psa_random.c
 *
 * This file contains the implementations of the entry points associated to the
 * random generation capability as described by the PSA Cryptoprocessor Driver
 * interface specification.
 */
#ifndef CC3XX_PSA_API_CONFIG_FILE
#include "cc3xx_psa_api_config.h"
#else
#include CC3XX_PSA_API_CONFIG_FILE
#endif

#include "cc3xx_psa_random.h"
#include "cc3xx_psa_entropy.h"
#include "cc3xx_misc.h"
#include "cc3xx_error.h"
#include "cc3xx_drbg.h"
#include "cc3xx_rng.h"

/* Include the definition of the context types */
#include "cc3xx_crypto_primitives_private.h"

#if defined(CC3XX_CONFIG_ENABLE_RANDOM_HASH_DRBG) + \
    defined(CC3XX_CONFIG_ENABLE_RANDOM_HMAC_DRBG) + \
    defined(CC3XX_CONFIG_ENABLE_RANDOM_CTR_DRBG) != 1
#error "CC3XX configuration must have a single DRBG construction enabled"
#endif

struct random_context_params_t {
    cc3xx_drbg_id_t type;
    size_t initial_entropy_size;
    size_t reseed_entropy_size;
};

/* Keep the context private to this module */
static struct {
    cc3xx_random_context_t ctx;
    bool isInitialized;
} cc3xx_psa_random_state;

/** @defgroup psa_random PSA driver entry points for collecting random
 *                       numbers generated using an underlying DRBG
 *                       mechanism
 *
 *  Entry points random init, add_entropy and collection as described by
 *  the PSA Cryptoprocessor Driver interface specification.
 *
 *  @{
 */
psa_status_t cc3xx_init_random(cc3xx_random_context_t *context)
{
    cc3xx_err_t err;
    const struct random_context_params_t conf = {
#if defined(CC3XX_CONFIG_ENABLE_RANDOM_CTR_DRBG)
    .type = CC3XX_DRBG_CTR, .initial_entropy_size = CC3XX_DRBG_CTR_SEEDLEN, .reseed_entropy_size = CC3XX_DRBG_CTR_SEEDLEN
#elif defined(CC3XX_CONFIG_ENABLE_RANDOM_HMAC_DRBG)
    .type = CC3XX_DRBG_HMAC, .initial_entropy_size = 32, .reseed_entropy_size = 32
#elif defined(CC3XX_CONFIG_ENABLE_RANDOM_HASH_DRBG)
    .type = CC3XX_DRBG_HASH, .initial_entropy_size = 32, .reseed_entropy_size = 32
#endif
    };
    uint8_t initial_entropy[conf.initial_entropy_size];

    CC3XX_ASSERT(context != NULL);

    err = cc3xx_get_entropy(0, NULL, initial_entropy, sizeof(initial_entropy));
    if (err != CC3XX_ERR_SUCCESS) {
        return cc3xx_to_psa_err(err);
    }

    err = cc3xx_lowlevel_drbg_init(conf.type, &(context->state),
                                   initial_entropy, sizeof(initial_entropy),
                                   NULL, 0, NULL, 0);
    if (err != CC3XX_ERR_SUCCESS) {
        return cc3xx_to_psa_err(err);
    }

    return PSA_SUCCESS;
}

psa_status_t cc3xx_add_entropy(
    cc3xx_random_context_t *context,
    const uint8_t *entropy,
    size_t entropy_size)
{
#ifdef CC3XX_CONFIG_ENABLE_RANDOM_CTR_DRBG
    if (entropy_size != CC3XX_DRBG_CTR_SEEDLEN) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
#endif

    /* Probably not needed to implement the logic of add_entropy as the expectation
     * is that the driver will work as a FULL RNG driver, i.e. it takes care on its
     * own of seeding and reseeding, which means its external initial_entropy_size
     * and reseed_entropy_size properties will be set to 0, hence the core in that
     * case would never call this entry point. In case the design changes, this
     * entry point must just interface with cc3xx_drbg_reseed()
     */
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t cc3xx_get_random(cc3xx_random_context_t *context,
    uint8_t *output,
    size_t output_size,
    size_t *output_length)
{
    cc3xx_err_t err;

    CC3XX_ASSERT(context != NULL);
    CC3XX_ASSERT(output != NULL);
    CC3XX_ASSERT(output_length != NULL);

    err = cc3xx_lowlevel_drbg_generate(&(context->state), output_size * 8, output, NULL, 0);
    if (err != CC3XX_ERR_SUCCESS) {
        *output_length = 0;
        return cc3xx_to_psa_err(err);
    }

    *output_length = output_size;
    return PSA_SUCCESS;
}

psa_status_t cc3xx_internal_get_random(uint8_t *output, size_t output_size, size_t *output_length)
{
    psa_status_t status;

    CC3XX_ASSERT(output != NULL);
    CC3XX_ASSERT(output_length != NULL);

    if (!cc3xx_psa_random_state.isInitialized) {
        status = cc3xx_init_random(&cc3xx_psa_random_state.ctx);
        if (status == PSA_SUCCESS) {
            cc3xx_psa_random_state.isInitialized = true;
        } else {
            return status;
        }
    }

    return cc3xx_get_random(&cc3xx_psa_random_state.ctx,
                            output, output_size, output_length);
}

#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
/* As of mbed TLS 3.5, there is no support in the Core for the random entry points,
 * so the integration happens through the definition of MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG
 * as the function that mbed TLS uses to retrieve random numbers from an external
 * generator. Note that we don't rely on redefining the type
 * mbedtls_psa_external_random_context_t available to the PSA Crypto core to make
 * keep integration simple, as there is no real gain in doing that.
 */
psa_status_t mbedtls_psa_external_get_random(
    mbedtls_psa_external_random_context_t *context,
    uint8_t *output, size_t output_size, size_t *output_length)
{
    (void)context; /* The driver keeps the state internal for simplicity */

    return cc3xx_internal_get_random(output, output_size, output_length);
}
#endif /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */
/** @} */ // end of psa_random
