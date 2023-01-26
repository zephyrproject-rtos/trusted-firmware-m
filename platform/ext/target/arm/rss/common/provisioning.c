/*
 * Copyright (c) 2021-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_plat_provisioning.h"

#include "cmsis_compiler.h"
#include "config_attest.h"
#include "tfm_plat_otp.h"
#include "tfm_attest_hal.h"
#include "psa/crypto.h"
#include "tfm_spm_log.h"

#include <string.h>

#define ASSEMBLY_AND_TEST_PROV_DATA_MAGIC 0xC0DEFEED
#define PSA_ROT_PROV_DATA_MAGIC           0xBEEFFEED

__PACKED_STRUCT tfm_assembly_and_test_provisioning_data_t {
    uint32_t magic;
    uint8_t huk[32];
};

__PACKED_STRUCT tfm_psa_rot_provisioning_data_t {
    uint32_t magic;
    uint8_t host_rotpk_s[96];
    uint8_t host_rotpk_ns[96];
    uint8_t host_rotpk_cca[96];

    uint8_t boot_seed[32];
    uint8_t implementation_id[32];
    uint8_t cert_ref[32];
    uint8_t verification_service_url[32];
    uint8_t profile_definition[32];
    uint32_t cca_system_properties;
};

#ifdef TFM_DUMMY_PROVISIONING
static const struct tfm_assembly_and_test_provisioning_data_t assembly_and_test_prov_data = {
    ASSEMBLY_AND_TEST_PROV_DATA_MAGIC,
    /* HUK */
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    },
};

static const struct tfm_psa_rot_provisioning_data_t psa_rot_prov_data = {
    PSA_ROT_PROV_DATA_MAGIC,
    /* HOST_ROTPK_S */
    {
        0x09, 0x20, 0x59, 0xde, 0xc5, 0x1b, 0xe2, 0x96,
        0xfe, 0x4b, 0xa0, 0x16, 0x20, 0xac, 0xd7, 0xce,
        0xe2, 0x1e, 0xd5, 0xbf, 0x74, 0x4f, 0xe4, 0x47,
        0xab, 0x1f, 0xe4, 0xcb, 0x91, 0x52, 0x94, 0xb2,
        0xf2, 0xff, 0xaf, 0x3a, 0x47, 0x26, 0x0e, 0x13,
        0x4f, 0x8f, 0x2c, 0x1b, 0x5e, 0xde, 0xe8, 0x9e,
        0xdd, 0x2e, 0x1c, 0xf1, 0x0d, 0x3c, 0xc1, 0xee,
        0x32, 0x92, 0x9d, 0x05, 0xca, 0x57, 0x0d, 0x0e,
        0xbc, 0xd1, 0x72, 0x32, 0xf4, 0x1f, 0x1c, 0xe4,
        0x48, 0xd8, 0x79, 0x87, 0xfc, 0x3b, 0x2f, 0xf4,
        0x79, 0xe2, 0xf1, 0x03, 0x1f, 0xf3, 0x4d, 0xbc,
        0x76, 0x8a, 0x81, 0x19, 0x4a, 0x95, 0x4d, 0xac
    },
    /* HOST_ROTPK_NS */
    {
        0x09, 0x20, 0x59, 0xde, 0xc5, 0x1b, 0xe2, 0x96,
        0xfe, 0x4b, 0xa0, 0x16, 0x20, 0xac, 0xd7, 0xce,
        0xe2, 0x1e, 0xd5, 0xbf, 0x74, 0x4f, 0xe4, 0x47,
        0xab, 0x1f, 0xe4, 0xcb, 0x91, 0x52, 0x94, 0xb2,
        0xf2, 0xff, 0xaf, 0x3a, 0x47, 0x26, 0x0e, 0x13,
        0x4f, 0x8f, 0x2c, 0x1b, 0x5e, 0xde, 0xe8, 0x9e,
        0xdd, 0x2e, 0x1c, 0xf1, 0x0d, 0x3c, 0xc1, 0xee,
        0x32, 0x92, 0x9d, 0x05, 0xca, 0x57, 0x0d, 0x0e,
        0xbc, 0xd1, 0x72, 0x32, 0xf4, 0x1f, 0x1c, 0xe4,
        0x48, 0xd8, 0x79, 0x87, 0xfc, 0x3b, 0x2f, 0xf4,
        0x79, 0xe2, 0xf1, 0x03, 0x1f, 0xf3, 0x4d, 0xbc,
        0x76, 0x8a, 0x81, 0x19, 0x4a, 0x95, 0x4d, 0xac
    },
    /* HOST_ROTPK_CCA */
    {
        0x09, 0x20, 0x59, 0xde, 0xc5, 0x1b, 0xe2, 0x96,
        0xfe, 0x4b, 0xa0, 0x16, 0x20, 0xac, 0xd7, 0xce,
        0xe2, 0x1e, 0xd5, 0xbf, 0x74, 0x4f, 0xe4, 0x47,
        0xab, 0x1f, 0xe4, 0xcb, 0x91, 0x52, 0x94, 0xb2,
        0xf2, 0xff, 0xaf, 0x3a, 0x47, 0x26, 0x0e, 0x13,
        0x4f, 0x8f, 0x2c, 0x1b, 0x5e, 0xde, 0xe8, 0x9e,
        0xdd, 0x2e, 0x1c, 0xf1, 0x0d, 0x3c, 0xc1, 0xee,
        0x32, 0x92, 0x9d, 0x05, 0xca, 0x57, 0x0d, 0x0e,
        0xbc, 0xd1, 0x72, 0x32, 0xf4, 0x1f, 0x1c, 0xe4,
        0x48, 0xd8, 0x79, 0x87, 0xfc, 0x3b, 0x2f, 0xf4,
        0x79, 0xe2, 0xf1, 0x03, 0x1f, 0xf3, 0x4d, 0xbc,
        0x76, 0x8a, 0x81, 0x19, 0x4a, 0x95, 0x4d, 0xac
    },
    /* boot seed */
    {
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
        0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
        0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    },
    /* implementation id */
    {
        0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
        0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB,
        0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
        0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD,
    },
    /* certification reference */
    "0604565272829-10010",
    /* verification_service_url */
    "www.trustedfirmware.org",
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
    /* CCA system properties placeholder */
    0xDEADBEEF,
};
#else
static struct tfm_assembly_and_test_provisioning_data_t assembly_and_test_prov_data;
static struct tfm_psa_rot_provisioning_data_t psa_rot_prov_data;
#endif /* TFM_DUMMY_PROVISIONING */

void tfm_plat_provisioning_check_for_dummy_keys(void)
{
    uint64_t host_rotpk_s_start;

    tfm_plat_otp_read(PLAT_OTP_ID_HOST_ROTPK_S, sizeof(host_rotpk_s_start),
                      (uint8_t*)&host_rotpk_s_start);

    if(host_rotpk_s_start == 0xA4906F6DB254B4A9) {
        SPMLOG_ERRMSG("[WRN]\033[1;31m ");
        SPMLOG_ERRMSG("This device was provisioned with dummy keys. ");
        SPMLOG_ERRMSG("This device is \033[1;1mNOT SECURE");
        SPMLOG_ERRMSG("\033[0m\r\n");
    }

    memset(&host_rotpk_s_start, 0, sizeof(host_rotpk_s_start));
}

int tfm_plat_provisioning_is_required(void)
{
    enum tfm_plat_err_t err;
    enum plat_otp_lcs_t lcs;

    err = tfm_plat_otp_read(PLAT_OTP_ID_LCS, sizeof(lcs), (uint8_t*)&lcs);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    return lcs == PLAT_OTP_LCS_ASSEMBLY_AND_TEST
        || lcs == PLAT_OTP_LCS_PSA_ROT_PROVISIONING;
}

enum tfm_plat_err_t provision_assembly_and_test(void)
{
    enum tfm_plat_err_t err;
    uint32_t new_lcs;

    err = tfm_plat_otp_write(PLAT_OTP_ID_HUK, sizeof(assembly_and_test_prov_data.huk),
                             assembly_and_test_prov_data.huk);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    new_lcs = PLAT_OTP_LCS_PSA_ROT_PROVISIONING;
    err = tfm_plat_otp_write(PLAT_OTP_ID_LCS, sizeof(new_lcs),
                             (uint8_t*)&new_lcs);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    return err;
}

enum tfm_plat_err_t provision_psa_rot(void)
{
    enum tfm_plat_err_t err;
    uint32_t new_lcs;

    err = tfm_plat_otp_write(PLAT_OTP_ID_HOST_ROTPK_S,
                             sizeof(psa_rot_prov_data.host_rotpk_s),
                             psa_rot_prov_data.host_rotpk_s);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_HOST_ROTPK_NS,
                             sizeof(psa_rot_prov_data.host_rotpk_ns),
                             psa_rot_prov_data.host_rotpk_ns);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_HOST_ROTPK_CCA,
                             sizeof(psa_rot_prov_data.host_rotpk_cca),
                             psa_rot_prov_data.host_rotpk_cca);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_BOOT_SEED,
                             sizeof(psa_rot_prov_data.boot_seed),
                             psa_rot_prov_data.boot_seed);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    err = tfm_plat_otp_write(PLAT_OTP_ID_IMPLEMENTATION_ID,
                             sizeof(psa_rot_prov_data.implementation_id),
                             psa_rot_prov_data.implementation_id);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    err = tfm_plat_otp_write(PLAT_OTP_ID_CERT_REF,
                             sizeof(psa_rot_prov_data.cert_ref),
                             psa_rot_prov_data.cert_ref);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    err = tfm_plat_otp_write(PLAT_OTP_ID_VERIFICATION_SERVICE_URL,
                             sizeof(psa_rot_prov_data.verification_service_url),
                             psa_rot_prov_data.verification_service_url);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    err = tfm_plat_otp_write(PLAT_OTP_ID_PROFILE_DEFINITION,
                             sizeof(psa_rot_prov_data.profile_definition),
                             psa_rot_prov_data.profile_definition);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_CCA_SYSTEM_PROPERTIES,
                             sizeof(psa_rot_prov_data.cca_system_properties),
                             (uint8_t*)&psa_rot_prov_data.cca_system_properties);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    new_lcs = PLAT_OTP_LCS_SECURED;
    err = tfm_plat_otp_write(PLAT_OTP_ID_LCS,
                             sizeof(new_lcs),
                             (uint8_t*)&new_lcs);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    return err;
}

enum tfm_plat_err_t tfm_plat_provisioning_perform(void)
{
    enum tfm_plat_err_t err;
    enum plat_otp_lcs_t lcs;

    err = tfm_plat_otp_read(PLAT_OTP_ID_LCS, sizeof(lcs), (uint8_t*)&lcs);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    SPMLOG_INFMSG("[INF] Beginning TF-M provisioning\r\n");

#ifdef TFM_DUMMY_PROVISIONING
    SPMLOG_ERRMSG("[WRN]\033[1;31m ");
    SPMLOG_ERRMSG("TFM_DUMMY_PROVISIONING is not suitable for production! ");
    SPMLOG_ERRMSG("This device is \033[1;1mNOT SECURE");
    SPMLOG_ERRMSG("\033[0m\r\n");
#endif /* TFM_DUMMY_PROVISIONING */

    if (lcs == PLAT_OTP_LCS_ASSEMBLY_AND_TEST) {
        if (assembly_and_test_prov_data.magic != ASSEMBLY_AND_TEST_PROV_DATA_MAGIC) {
            SPMLOG_ERRMSG("No valid ASSEMBLY_AND_TEST provisioning data found\r\n");
            return TFM_PLAT_ERR_INVALID_INPUT;
        }

        err = provision_assembly_and_test();
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }
    }

    err = tfm_plat_otp_read(PLAT_OTP_ID_LCS, sizeof(lcs), (uint8_t*)&lcs);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    if (lcs == PLAT_OTP_LCS_PSA_ROT_PROVISIONING) {
        if (psa_rot_prov_data.magic != PSA_ROT_PROV_DATA_MAGIC) {
            SPMLOG_ERRMSG("No valid PSA_ROT provisioning data found\r\n");
            return TFM_PLAT_ERR_INVALID_INPUT;
        }

        err = provision_psa_rot();
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }
    }

    return TFM_PLAT_ERR_SUCCESS;
}
