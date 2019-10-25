/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "platform/include/tfm_attest_hal.h"
#include <stdint.h>

#ifdef CRYPTO_HW_ACCELERATOR
#include "crypto_hw.h"
#include "mbedtls_cc_mng_int.h"
#endif /* CRYPTO_HW_ACCELERATOR */

/* Example verification service URL for initial attestation token */
static const char verification_service_url[] = "www.trustedfirmware.org";

/* Example profile definition document for initial attestation token */
static const char attestation_profile_definition[] = "PSA_IOT_PROFILE_1";

#ifdef CRYPTO_HW_ACCELERATOR
static enum tfm_security_lifecycle_t
map_cc312_to_tfm_lifecycle(uint32_t cc312_lcs)
{
    enum tfm_security_lifecycle_t tfm_lcs;

    if (cc312_lcs == CC_MNG_LCS_CM) {
        tfm_lcs = TFM_SLC_ASSEMBLY_AND_TEST;
    } else if (cc312_lcs == CC_MNG_LCS_DM) {
        tfm_lcs = TFM_SLC_PSA_ROT_PROVISIONING;
    } else if (cc312_lcs == CC_MNG_LCS_SEC_ENABLED) {
        tfm_lcs = TFM_SLC_SECURED;
    } else if (cc312_lcs == CC_MNG_LCS_RMA) {
        tfm_lcs = TFM_SLC_DECOMMISSIONED;
    } else {
        tfm_lcs = TFM_SLC_UNKNOWN;
    }

    return tfm_lcs;
}
#endif

enum tfm_security_lifecycle_t tfm_attest_hal_get_security_lifecycle(void)
{
#ifdef CRYPTO_HW_ACCELERATOR
    int rc;
    uint32_t cc312_lcs;
    enum tfm_security_lifecycle_t tfm_lcs;

    rc = crypto_hw_accelerator_get_lcs(&cc312_lcs);
    if (rc) {
        return TFM_SLC_UNKNOWN;
    }

    tfm_lcs = map_cc312_to_tfm_lifecycle(cc312_lcs);

    return tfm_lcs;
#else
    return TFM_SLC_SECURED;
#endif
}

const char *
tfm_attest_hal_get_verification_service(uint32_t *size)
{
    *size = sizeof(verification_service_url) - 1;

    return verification_service_url;
}

const char *
tfm_attest_hal_get_profile_definition(uint32_t *size)
{
    *size = sizeof(attestation_profile_definition) - 1;

    return attestation_profile_definition;
}
