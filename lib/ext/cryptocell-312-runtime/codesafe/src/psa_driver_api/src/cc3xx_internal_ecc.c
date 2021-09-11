/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cc3xx_internal_ecc.h"

#include <cc_ecpki_domain.h>
#include <cc_ecpki_build.h>
#include <cc_pal_log.h>
#include <cc_pal_mem.h>

psa_status_t cc3xx_ecc_psa_domain_to_cc_domain(
        psa_ecc_family_t curve, psa_key_bits_t key_bits,
        CCEcpkiDomainID_t *pDomainId)
{
    switch (curve) {
    case PSA_ECC_FAMILY_SECP_K1:
        switch (key_bits) {
        case 192:
            *pDomainId = CC_ECPKI_DomainID_secp192k1;
            break;
        case 224:
            *pDomainId = CC_ECPKI_DomainID_secp224k1;
            break;
        case 256:
            *pDomainId = CC_ECPKI_DomainID_secp256k1;
            break;
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }
        break;
    case PSA_ECC_FAMILY_SECP_R1:
        switch (key_bits) {
        case 192:
            *pDomainId = CC_ECPKI_DomainID_secp192r1;
            break;
        case 224:
            *pDomainId = CC_ECPKI_DomainID_secp224r1;
            break;
        case 256:
            *pDomainId = CC_ECPKI_DomainID_secp256r1;
            break;
        case 384:
            *pDomainId = CC_ECPKI_DomainID_secp384r1;
            break;
        case 521:
            *pDomainId = CC_ECPKI_DomainID_secp521r1;
            break;
        default:
            return PSA_ERROR_NOT_SUPPORTED;
        }
        break;
    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }

    return PSA_SUCCESS;
}

psa_status_t cc3xx_ecc_psa_priv_to_cc_priv(
        CCEcpkiDomainID_t DomainId,
        const uint8_t *priv_key, size_t priv_key_size,
        CCEcpkiUserPrivKey_t *pUserPrivKey)
{
    CCError_t status;
    const CCEcpkiDomain_t *pDomain;

    if (NULL == priv_key || NULL == pUserPrivKey) {
        CC_PAL_LOG_ERR("Null pointer exception\n");
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    pDomain = CC_EcpkiGetEcDomain(DomainId);
    if (NULL == pDomain) {
        CC_PAL_LOG_ERR("Domain ID %d is not supported\n", DomainId);
        return PSA_ERROR_NOT_SUPPORTED;
    }

    status = CC_EcpkiPrivKeyBuild(pDomain,
                                  priv_key,
                                  priv_key_size,
                                  pUserPrivKey);
    if (status != CC_OK) {
        CC_PAL_LOG_ERR("Error building private key\n");
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return PSA_SUCCESS;
}

psa_status_t cc3xx_ecc_psa_publ_to_cc_publ(
        CCEcpkiDomainID_t DomainId,
        const uint8_t *publ_key, size_t publ_key_size,
        CCEcpkiUserPublKey_t *pUserPublKey)
{
    psa_status_t ret = PSA_ERROR_CORRUPTION_DETECTED;
    CCError_t status;
    const CCEcpkiDomain_t *pDomain;
    CCEcpkiBuildTempData_t BuildTempData;

    if (NULL == publ_key || NULL == pUserPublKey) {
        CC_PAL_LOG_ERR("Null pointer exception\n");
        ret = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    pDomain = CC_EcpkiGetEcDomain(DomainId);
    if (NULL == pDomain) {
        CC_PAL_LOG_ERR("Domain ID %d is not supported\n", DomainId);
        ret = PSA_ERROR_NOT_SUPPORTED;
        goto exit;
    }

    status = CC_EcpkiPublKeyBuildAndCheck(pDomain,
                                          (uint8_t *)publ_key,
                                          publ_key_size,
                                          ECpublKeyFullCheck,
                                          pUserPublKey,
                                          &BuildTempData);
    if (status != CC_OK) {
        CC_PAL_LOG_ERR("Error building public key\n");
        ret = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    ret = PSA_SUCCESS;

exit:
    CC_PalMemSetZero(&BuildTempData, sizeof(CCEcpkiBuildTempData_t));
    return ret;
}
