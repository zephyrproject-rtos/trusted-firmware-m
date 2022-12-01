/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tfm_sp_log.h"

#include "config_crypto.h"
#include "psa/crypto.h"
#include "crypto_library.h"

/*
 * \brief This Mbed TLS include is needed to initialise the memory allocator
 *        of the library used for internal allocations
 */
#include "mbedtls/memory_buffer_alloc.h"
/*
 * \brief This Mbed TLS include is needed to set the mbedtls_printf to the
 *        function required by the TF-M framework in order to be able to
 *        print to terminal through mbedtls_printf
 */
#include "mbedtls/platform.h"
/*
 * \brief This Mbed TLS include is needed to retrieve version information for
 *        display
 */
#include "mbedtls/build_info.h"

#ifndef MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER
#error "MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER must be selected in Mbed TLS config file"
#endif

/**
 * \brief Static buffer containing the string describing the mbed TLS version. mbed TLS
 *        guarantees that the string will never be greater than 18 bytes
 */
static char mbedtls_version_full[18];

/**
 * \brief Static buffer to be used by Mbed Crypto for memory allocations
 *
 */
static uint8_t mbedtls_mem_buf[CRYPTO_ENGINE_BUF_SIZE] = {0};

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

psa_status_t tfm_crypto_core_library_init(void)
{
    /* Initialise the Mbed Crypto memory allocator to use static memory
     * allocation from the provided buffer instead of using the heap
     */
    mbedtls_memory_buffer_alloc_init(mbedtls_mem_buf,
                                     CRYPTO_ENGINE_BUF_SIZE);

    /* mbedtls_printf is used to print messages including error information. */
#if (TFM_PARTITION_LOG_LEVEL >= TFM_PARTITION_LOG_LEVEL_ERROR)
    mbedtls_platform_set_printf(printf);
#endif

    return PSA_SUCCESS;
}
/*!@}*/
