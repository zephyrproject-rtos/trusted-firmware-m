/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "config_crypto.h"

#include "crypto_library.h"

#include "mbedtls/build_info.h"

#ifndef MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER
#error "MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER must be selected in Mbed TLS config file"
#endif

/* Mbed TLS is guaranteed not to have a version string longer than 18 bytes */
static char mbedtls_version_full[18];

/*!
 * \defgroup tfm_crypto_library Set of functions implementing the abstractions of the underlying cryptographic
 *                              library that implements the PSA Crypto APIs to provide the PSA Crypto core
 *                              functionality to the TF-M Crypto service. Currently it supports only an
 *                              mbed TLS based abstraction.
 */
/*!@{*/
tfm_crypto_library_key_id_t tfm_crypto_library_key_id_init(int32_t owner, psa_key_id_t key_id)
{
    return mbedtls_svc_key_id_make(owner, key_id);
}

char *tfm_crypto_library_get_info(void)
{
    memcpy(mbedtls_version_full, MBEDTLS_VERSION_STRING_FULL, sizeof(MBEDTLS_VERSION_STRING_FULL));
    return mbedtls_version_full;
}
/*!@}*/
