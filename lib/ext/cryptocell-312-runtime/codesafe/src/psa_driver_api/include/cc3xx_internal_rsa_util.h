/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_INTERNAL_RSA_UTIL_H
#define CC3XX_INTERNAL_RSA_UTIL_H

#include "psa/crypto.h"

#include "cc_common.h"
#include "cc_ecpki_error.h"
#include "cc_pal_abort.h"
#include "cc_pal_mem.h"
#include "cc_pal_types.h"

#include "cc_ecpki_build.h"
#include "cc_ecpki_domain.h"
#include "cc_ecpki_ecdsa.h"
#include "cc_ecpki_kg.h"
#include "cc_ecpki_local.h"
#include "pka_ec_wrst.h"

#include "cc_rsa_types.h"

/* Minimal and maximal size of RSA modulus in bits
 * According to FIPS 186-4 size in bits should be in range [1024...3072] */
#if defined(ARCH_IS_CC310)
#define CC3XX_RSA_MIN_VALID_KEY_SIZE_VALUE_IN_BITS 1024
#define CC3XX_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS 2048
#define CC3XX_RSA_MIN_VALID_KEYGEN_SIZE_VALUE_IN_BITS 1024
#define CC3XX_RSA_MAX_VALID_KEYGEN_SIZE_VALUE_IN_BITS 2048
#else
#define CC3XX_RSA_MIN_VALID_KEY_SIZE_VALUE_IN_BITS 1024
#define CC3XX_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS 4096
#define CC3XX_RSA_MIN_VALID_KEYGEN_SIZE_VALUE_IN_BITS 1024
#define CC3XX_RSA_MAX_VALID_KEYGEN_SIZE_VALUE_IN_BITS 3072
#endif

psa_status_t cc3xx_rsa_cc_error_to_psa_error(CCError_t cc_error);

CCError_t cc3xx_rsa_save_der_priv_key(uint8_t *key_buffer,
                                      size_t key_buffer_size, uint32_t *n,
                                      uint32_t *e, uint32_t *d, uint32_t *p,
                                      uint32_t *q, uint32_t *dP, uint32_t *dQ,
                                      uint32_t *qInv, size_t d_size_bytes);

CCError_t cc3xx_rsa_psa_priv_to_psa_publ(uint8_t *priv_key_buffer,
                                         size_t priv_key_buffer_size,
                                         uint8_t *publ_key_buffer,
                                         size_t publ_key_buffer_size);

CCError_t cc3xx_rsa_psa_priv_to_cc_priv(const uint8_t *psa_priv_key_buffer,
                                        size_t psa_priv_key_buffer_size,
                                        CCRsaUserPrivKey_t *UserPrivKey_ptr);

CCError_t cc3xx_rsa_psa_priv_to_cc_pub(const uint8_t *psa_pub_key_buffer,
                                       size_t psa_pub_key_buffer_size,
                                       CCRsaUserPubKey_t *UserPubKey_ptr);

CCError_t cc3xx_rsa_psa_pub_to_cc_pub(const uint8_t *psa_pub_key_buffer,
                                      size_t psa_pub_key_buffer_size,
                                      CCRsaUserPubKey_t *UserPubKey_ptr);

#endif /* CC3XX_INTERNAL_RSA_UTIL_H */