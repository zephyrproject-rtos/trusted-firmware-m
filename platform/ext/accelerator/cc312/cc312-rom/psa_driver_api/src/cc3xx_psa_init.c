/*
 * Copyright (c) 2022-2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/** @file cc3xx_psa_init.c
 *
 * This file contains the implementation of the entry points associated to the
 * mac capability (single-part and multipart) as described by the PSA
 * Cryptoprocessor Driver interface specification
 *
 */

#include "psa/crypto.h"
#include "cc3xx_init.h"
#include "cc3xx_error.h"
#include "cc3xx_misc.h"

/** @defgroup psa_init PSA driver entry points for driver initialisation
 *                     and de-initialisation operations
 *
 *  Entry points driver init and de-init as described by the PSA
 *  Cryptoprocessor Driver interface specification.
 *
 *  @{
 */
psa_status_t cc3xx_init(void)
{
    cc3xx_err_t err;

    err = cc3xx_lowlevel_init();

    return cc3xx_to_psa_err(err);
}

psa_status_t cc3xx_free(void)
{
    cc3xx_err_t err;

    err = cc3xx_lowlevel_uninit();

    return cc3xx_to_psa_err(err);
}
/** @} */ // end of psa_init
