/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_PSA_KEY_AGREEMENT_H
#define CC3XX_PSA_KEY_AGREEMENT_H

#include "psa/crypto.h"

#include "cc3xx_internal_ecc.h"
#include "cc3xx_internal_ecdh.h"

psa_status_t cc3xx_key_agreement(
        const psa_key_attributes_t *attributes,
        const uint8_t *priv_key, size_t priv_key_size,
        const uint8_t *publ_key, size_t publ_key_size,
        uint8_t *output, size_t output_size, size_t *output_length,
        psa_algorithm_t alg);

#endif /* CC3XX_PSA_KEY_AGREEMENT_H */
