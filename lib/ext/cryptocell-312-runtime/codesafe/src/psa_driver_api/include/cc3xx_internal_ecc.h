/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_INTERNAL_ECC_H
#define CC3XX_INTERNAL_ECC_H

#include "psa/crypto.h"

#include "cc_ecpki_types.h"

/**
 * @brief   Extract domain ID from PSA
 *
 * @param[in]  curve         Curve extracted from EC key type
 * @param[in]  key_bits      Size of key in bits
 * @param[out] pDomainId     EC domain identifier
 *
 * @retval PSA_SUCCESS
 * @retval PSA_ERROR_NOT_SUPPORTED
 */
psa_status_t cc3xx_ecc_psa_domain_to_cc_domain(
        psa_ecc_family_t curve, psa_key_bits_t key_bits,
        CCEcpkiDomainID_t *pDomainId);

/**
 * @brief   Import private key using a domain ID and key buffer
 *
 * @param[in]  DomainId      EC domain identifier
 * @param[in]  priv_key      Private key buffer
 * @param[in]  priv_key_size Private key buffer size
 * @param[out] pUserPrivKey  ECPKI private key
 *
 * @retval PSA_SUCCESS
 * @retval PSA_ERROR_NOT_SUPPORTED
 * @retval PSA_ERROR_INVALID_ARGUMENT
 */
psa_status_t cc3xx_ecc_psa_priv_to_cc_priv(
        CCEcpkiDomainID_t DomainId,
        const uint8_t *priv_key, size_t priv_key_size,
        CCEcpkiUserPrivKey_t *pUserPrivKey);

/**
 * @brief   Import public key using a domain ID and key buffer
 *
 * @param[in]  DomainId      EC domain identifier
 * @param[in]  publ_key      Public key buffer
 * @param[in]  publ_key_size Public key buffer size
 * @param[out] pUserPublKey  ECPKI public key
 *
 * @retval PSA_SUCCESS
 * @retval PSA_ERROR_NOT_SUPPORTED
 * @retval PSA_ERROR_INVALID_ARGUMENT
 */
psa_status_t cc3xx_ecc_psa_publ_to_cc_publ(
        CCEcpkiDomainID_t DomainId,
        const uint8_t *publ_key, size_t publ_key_size,
        CCEcpkiUserPublKey_t *pUserPrivKey);

#endif /* CC3XX_INTERNAL_ECC_H */
