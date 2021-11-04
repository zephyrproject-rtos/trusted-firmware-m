/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_H
#define CC3XX_H

/** \file cc3xx.h
 *
 * This file includes each module of the cc3xx driver that complies with the
 * PSA Cryptoprocessor Driver interface specification. the list of the
 * available modules is:
 *
 *  - Symmetric ciphers:                      cc3xx_psa_cipher.h
 *  - Access to TRNG for entropy extraction:  cc3xx_psa_entropy.h
 *  - Hashing:                                cc3xx_psa_hash.h
 *  - MAC signing/verification:               cc3xx_psa_mac.h
 *  - Authenticated Encryption w Assoc. Data: cc3xx_psa_aead.h
 *  - Raw key agreement:                      cc3xx_psa_key_agreement.h
 *  - Random key generation and key handling: cc3xx_psa_key_generation.h
 *  - Asymmetric signature schemes:           cc3xx_psa_asymmetric_signature.h
 *  - Asymmetric encryption schemes:          cc3xx_psa_asymmetric_encryption.h
 *
 */

#include "cc3xx_psa_cipher.h"
#include "cc3xx_psa_entropy.h"
#include "cc3xx_psa_hash.h"
#include "cc3xx_psa_mac.h"
#include "cc3xx_psa_aead.h"
#include "cc3xx_psa_key_agreement.h"
#include "cc3xx_psa_key_generation.h"
#include "cc3xx_psa_asymmetric_signature.h"
#include "cc3xx_psa_asymmetric_encryption.h"

#endif /* CC3XX_H */
