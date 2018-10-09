/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_crypto_defs.h"
#include "crypto_engine.h"
#include "tfm_crypto_api.h"

static enum tfm_crypto_err_t tfm_crypto_module_init(void)
{
    enum tfm_crypto_err_t err;

    /* Init the Key module */
    err = tfm_crypto_init_key();
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* Init the Alloc module */
    err = tfm_crypto_init_alloc();

    return err;
}

enum tfm_crypto_err_t tfm_crypto_init(void)
{
    psa_status_t status;
    enum tfm_crypto_err_t err;

    /* Initialise other modules of the service */
    err = tfm_crypto_module_init();
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* Initialise the engine interface module */
    status = tfm_crypto_engine_init();
    if (status != PSA_SUCCESS) {
        /* FIXME: For the time being, keep returning success even if the engine
         * is not initialised correctly. This can be used to test corner cases
         * without triggering any TF-M recovery mechanism during boot-up if it
         * recognises that a service has not completed booting correctly.
         */
        return TFM_CRYPTO_ERR_PSA_SUCCESS;
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}
