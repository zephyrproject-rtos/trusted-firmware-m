/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**
 * \file tfm_crypto_struct.h
 *
 * \brief Similarly to what psa_crypto_struct.h defines for
 *        the frontend, this header provides Crypto service
 *        specific definitions for operation contexts.
 *        Current implementation is directly based on Mbed TLS.
 */

#ifndef __TFM_CRYPTO_STRUCT_H__
#define __TFM_CRYPTO_STRUCT_H__

/* Include the Mbed TLS configuration file, the way Mbed TLS does it
 * in each of its header files.
 */
#if !defined(MBEDTLS_CONFIG_FILE)
#include "platform/ext/common/tfm_mbedtls_config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/cipher.h"
#include "mbedtls/cmac.h"
#include "mbedtls/md.h"

struct tfm_hash_operation_s
{
    psa_algorithm_t alg;
    mbedtls_md_context_t md;
};

struct tfm_hmac_internal_data_s
{
    /* The hash context. */
    struct psa_hash_operation_s hash_ctx;
    /* The HMAC part of the context. */
    uint8_t opad[PSA_HMAC_MAX_HASH_BLOCK_SIZE];
};

struct tfm_mac_operation_s
{
    psa_algorithm_t alg;
    uint8_t key_set;
    uint8_t iv_required;
    uint8_t iv_set;
    uint8_t has_input;
    uint8_t key_usage_sign;
    uint8_t key_usage_verify;
    uint8_t mac_size;
    union
    {
        struct tfm_hmac_internal_data_s hmac;
        mbedtls_cipher_context_t cmac;
    } ctx;
};

#define PSA_CIPHER_IV_MAX_SIZE 16

struct tfm_cipher_operation_s
{
    psa_algorithm_t alg;
    uint8_t key_set;
    uint8_t iv_required;
    uint8_t iv_set;
    uint8_t iv_size;
    uint8_t block_size;
    psa_key_slot_t key;
    mbedtls_cipher_context_t cipher;
};
#endif /* __TFM_CRYPTO_STRUCT_H__ */
