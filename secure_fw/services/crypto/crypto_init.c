/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_crypto_defs.h"
#include "tfm_crypto_api.h"

/* Pre include Mbed TLS headers */
#define LIB_PREFIX_NAME __tfm_crypto__
#include "mbedtls_global_symbols.h"

/* Include the Mbed TLS configuration file, the way Mbed TLS does it
 * in each of its header files.
 */
#if !defined(MBEDTLS_CONFIG_FILE)
#include "platform/ext/common/tfm_mbedtls_config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/memory_buffer_alloc.h"

/**
 * \brief Buffer size used by Mbed TLS for its allocations
 */
#define TFM_CRYPTO_MBEDTLS_MEM_BUF_LEN (1024)

/**
 * \brief Static buffer to be used by Mbed TLS for memory allocations
 *
 */
static uint8_t mbedtls_mem_buf[TFM_CRYPTO_MBEDTLS_MEM_BUF_LEN] = {0};

enum tfm_crypto_err_t tfm_crypto_init(void)
{
    /* Initialise everything that is needed */

    /* Initialise the Mbed TLS static memory allocator so that Mbed TLS
     * allocates memory from the provided static buffer instead of from
     * the heap.
     */
    mbedtls_memory_buffer_alloc_init(mbedtls_mem_buf,
                                     TFM_CRYPTO_MBEDTLS_MEM_BUF_LEN);

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}
