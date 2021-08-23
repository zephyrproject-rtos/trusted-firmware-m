/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_INTERNAL_DRBG_UTIL_H
#define CC3XX_INTERNAL_DRBG_UTIL_H

#include "cc_ecpki_types.h"
#include "cc_rnd_common.h"
#include "psa/crypto.h"
#include "mbedtls/hmac_drbg.h"

#define CC3XX_CTR_DRBG_INITIALIZED 0x5A44A5A8

psa_status_t cc3xx_ctr_drbg_get_ctx(CCRndContext_t *rnd_ctx);

psa_status_t cc3xx_hmac_drbg_get_ctx(CCRndContext_t *rnd_ctx,
                                     mbedtls_hmac_drbg_context *hmac_drbg_ctx);

psa_status_t cc3xx_hmac_drbg_init_with_params(mbedtls_hmac_drbg_context *hmac_drbg_ctx,
                                              const uint8_t *hash,
                                              size_t hash_len,
                                              const uint8_t *key_buffer,
                                              size_t key_len,
                                              const CCEcpkiDomain_t *pDomain);

#endif /* CC3XX_INTERNAL_DRBG_UTIL_H */
