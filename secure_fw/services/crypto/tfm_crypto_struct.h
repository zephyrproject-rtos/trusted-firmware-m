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
 */

#include "psa_crypto.h"
#include "crypto_engine.h"

#ifndef __TFM_CRYPTO_STRUCT_H__
#define __TFM_CRYPTO_STRUCT_H__

struct tfm_hash_operation_s {

    psa_algorithm_t alg;
    union engine_hash_context engine_ctx;
};

struct tfm_hmac_internal_data_s {

    /* The hash operation. */
    psa_hash_operation_t hash_operation;
    /* The HMAC part of the context. */
    uint8_t opad[PSA_HMAC_MAX_HASH_BLOCK_SIZE];
};

struct tfm_mac_operation_s {

    psa_algorithm_t alg;
    uint8_t key_set;
    uint8_t iv_required;
    uint8_t iv_set;
    uint8_t has_input;
    uint8_t key_usage_sign;
    uint8_t key_usage_verify;
    uint8_t mac_size;
    union {
        struct tfm_hmac_internal_data_s hmac;
        union engine_cmac_context cmac;
    } ctx;
};

#define TFM_CIPHER_IV_MAX_SIZE 16

struct tfm_cipher_operation_s {

    psa_algorithm_t alg;
    uint8_t key_set;
    uint8_t iv_required;
    uint8_t iv_set;
    uint8_t iv_size;
    uint8_t block_size;
    psa_key_slot_t key;
    uint8_t cipher_mode;
    union engine_cipher_context engine_ctx;
};
#endif /* __TFM_CRYPTO_STRUCT_H__ */
