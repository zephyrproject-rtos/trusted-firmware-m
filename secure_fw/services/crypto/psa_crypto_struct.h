/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**
 * \file psa_crypto_struct.h
 *
 * \brief PSA cryptography module: Mbed TLS structured type implementations
 */

#ifndef __PSA_CRYPTO_STRUCT_H__
#define __PSA_CRYPTO_STRUCT_H__

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
#include "mbedtls/gcm.h"
#include "mbedtls/md.h"
#include "mbedtls/md2.h"
#include "mbedtls/md4.h"
#include "mbedtls/md5.h"
#include "mbedtls/ripemd160.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"

#if defined(MBEDTLS_SHA512_C)
#define PSA_HASH_MAX_SIZE 64
#define PSA_HMAC_MAX_HASH_BLOCK_SIZE 128
#else
#define PSA_HASH_MAX_SIZE 32
#define PSA_HMAC_MAX_HASH_BLOCK_SIZE 64
#endif

struct psa_hash_operation_s
{
    psa_algorithm_t alg;
    union
    {
        uint32_t dummy; /* Make the union non-empty in any case */
#if defined(MBEDTLS_MD_C)
        mbedtls_md_context_t md;
#endif
    } ctx;
};


typedef struct
{
    /* The hash context. */
    struct psa_hash_operation_s hash_ctx;
    /* The HMAC part of the context. */
    uint8_t opad[PSA_HMAC_MAX_HASH_BLOCK_SIZE];
} psa_hmac_internal_data;


struct psa_mac_operation_s
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
        uint32_t dummy; /* Make the union non-empty in any case */
#if defined(MBEDTLS_MD_C)
        psa_hmac_internal_data hmac;
#endif
#if defined(MBEDTLS_CMAC_C)
        mbedtls_cipher_context_t cmac;
#endif
    } ctx;
};

#define PSA_CIPHER_IV_MAX_SIZE 16

struct psa_cipher_operation_s
{
    psa_algorithm_t alg;
    uint8_t key_set;
    uint8_t iv_required;
    uint8_t iv_set;
    uint8_t iv_size;
    uint8_t block_size;
    psa_key_slot_t key;
    union
    {
        mbedtls_cipher_context_t cipher;
    } ctx;
};

struct psa_key_policy_s
{
    psa_key_usage_t usage;
    psa_algorithm_t alg;
};

#endif /* __PSA_CRYPTO_STRUCT_H__ */
