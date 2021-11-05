/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/** \file cc3xx_internal_chacha20.h
 *
 * This file contains the declarations of the internal functions to
 * perform symmetric encryption and decryption using the Chacha20
 * algorithm
 *
 */

#ifndef CC3XX_INTERNAL_CHACHA20_H
#define CC3XX_INTERNAL_CHACHA20_H

#include "psa/crypto.h"
#include "chacha_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief   Initialize the specified ChaCha20 context.
 */
void cc3xx_chacha20_init(ChachaContext_t *ctx);

/**
 * \brief   Release and clear the specified ChaCha20 context.
 */
void cc3xx_chacha20_free(ChachaContext_t *ctx);

/**
 * \brief   Set the encryption/decryption key.
 */
psa_status_t cc3xx_chacha20_setkey(
        ChachaContext_t *ctx,
        const uint8_t key[CHACHA_256_BIT_KEY_SIZE]);

/**
 * \brief   Set the nonce and initial counter value.
 */
psa_status_t cc3xx_chacha20_starts(
        ChachaContext_t *ctx,
        const uint8_t nonce[CHACHA_IV_96_SIZE_BYTES],
        uint32_t counter);

/**
 * \brief   Encrypt/decrypt data.
 */
psa_status_t cc3xx_chacha20_update(
        ChachaContext_t *ctx,
        size_t size,
        const uint8_t *input,
        unsigned char *output);

#ifdef __cplusplus
}
#endif
#endif /* CC3XX_INTERNAL_CHACHA20_H */
