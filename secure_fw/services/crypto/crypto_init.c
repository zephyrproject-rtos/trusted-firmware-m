/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_crypto_api.h"
#include "crypto_engine.h"

static psa_status_t tfm_crypto_module_init(void)
{
    psa_status_t status = PSA_SUCCESS;

    /* Init the Key module */
    status = tfm_crypto_init_key();
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Init the Alloc module */
    return tfm_crypto_init_alloc();
}

psa_status_t tfm_crypto_init(void)
{
    psa_status_t status;

    /* Initialise other modules of the service */
    status = tfm_crypto_module_init();
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Initialise the engine interface module */
    status = tfm_crypto_engine_init();
    if (status != PSA_SUCCESS) {
        /* FIXME: For the time being, keep returning success even if the engine
         * is not initialised correctly. This can be used to test corner cases
         * without triggering any TF-M recovery mechanism during boot-up if it
         * recognises that a service has not completed booting correctly.
         */
        return PSA_SUCCESS;
    }

    return PSA_SUCCESS;
}
