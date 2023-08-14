/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "provisioning_bundle.h"
#include "psa/crypto.h"
#include "config_tfm.h"

const struct provisioning_data_t data = {
    .bl2_assembly_and_test_prov_data = {
#if (MCUBOOT_SIGN_RSA_LEN == 2048)
            /* bl2 rotpk 0 */
            {
                BL2_ROTPK_0_2048
            },
            /* bl2 rotpk 1 */
            {
                BL2_ROTPK_1_2048
            },
#if (MCUBOOT_IMAGE_NUMBER > 2)
            /* bl2 rotpk 2 */
            {
                BL2_ROTPK_2_2048
            },
#endif /* MCUBOOT_IMAGE_NUMBER > 2 */
#if (MCUBOOT_IMAGE_NUMBER > 3)
            /* bl2 rotpk 3 */
            {
                BL2_ROTPK_3_2048
            },
#endif /* MCUBOOT_IMAGE_NUMBER > 3 */
#elif (MCUBOOT_SIGN_RSA_LEN == 3072)
            /* bl2 rotpk 0 */
            {
                BL2_ROTPK_0_3072
            },
            /* bl2 rotpk 1 */
            {
                BL2_ROTPK_1_3072
            },
#if (MCUBOOT_IMAGE_NUMBER > 2)
            /* bl2 rotpk 2 */
            {
                BL2_ROTPK_2_3072
            },
#endif /* MCUBOOT_IMAGE_NUMBER > 2 */
#if (MCUBOOT_IMAGE_NUMBER > 3)
            /* bl2 rotpk 3 */
            {
                BL2_ROTPK_3_3072
            },
#endif /* MCUBOOT_IMAGE_NUMBER > 3 */
#else
#error "No public key available for given signing algorithm."
#endif /* MCUBOOT_SIGN_RSA_LEN */

#ifdef PLATFORM_PSA_ADAC_SECURE_DEBUG
            {
                SECURE_DEBUG_PK
            },
#endif /* PLATFORM_PSA_ADAC_SECURE_DEBUG */
    },
    .assembly_and_test_prov_data = {
        /* HUK */
        {
            HUK
        },
    },
    .psa_rot_prov_data = {
            /* IAK */
            {
                IAK
            },
            /* IAK len */
            IAK_LEN,
#ifdef SYMMETRIC_INITIAL_ATTESTATION
            /* IAK type */
            PSA_ALG_HMAC(PSA_ALG_SHA_256),
#else
            /* IAK type */
            PSA_ECC_FAMILY_SECP_R1,
#endif /* SYMMETRIC_INITIAL_ATTESTATION */
#if ATTEST_INCLUDE_COSE_KEY_ID
            /* IAK id */
            "kid@trustedfirmware.example",
#endif /* ATTEST_INCLUDE_COSE_KEY_ID */
            /* boot seed */
            {
                BOOT_SEED
            },
            /* implementation id */
            {
                IMPLEMENTATION_ID
            },
            /* certification reference */
            CERTIFICATION_REFERENCE,
            /* verification_service_url */
            VERIFICATION_SERVICE_URL,
            /* attestation_profile_definition */
#if ATTEST_TOKEN_PROFILE_PSA_IOT_1
            "PSA_IOT_PROFILE_1",
#elif ATTEST_TOKEN_PROFILE_PSA_2_0_0
            "http://arm.com/psa/2.0.0",
#elif ATTEST_TOKEN_PROFILE_ARM_CCA
            "http://arm.com/CCA-SSD/1.0.0",
#else
#ifdef TFM_PARTITION_INITIAL_ATTESTATION
#error "Attestation token profile is incorrect"
#else
            "UNDEFINED",
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */
#endif
            /* Entropy seed */
            {
                ENTROPY_SEED
            },
    }
};
