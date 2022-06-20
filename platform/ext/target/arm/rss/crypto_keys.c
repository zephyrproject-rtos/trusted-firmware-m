/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_plat_crypto_keys.h"

#include "tfm_builtin_key_ids.h"

#include <stddef.h>
#include <string.h>

#include "region_defs.h"
#include "cmsis_compiler.h"
#include "tfm_plat_otp.h"
#include "psa_manifest/pid.h"
#include "tfm_builtin_key_loader.h"
#include "kmu_drv.h"
#include "device_definition.h"
#include "tfm_plat_otp.h"

#define TFM_NS_PARTITION_ID -1

#ifndef MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER
#error "MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER must be selected in Mbed TLS config file"
#endif

enum tfm_plat_err_t tfm_plat_builtin_key_get_usage(psa_key_id_t key_id,
                                                   mbedtls_key_owner_id_t owner,
                                                   psa_key_usage_t *usage)
{
    *usage = 0;

    switch (key_id) {
    case TFM_BUILTIN_KEY_ID_HUK:
        switch(owner) {
        default:
            /* Allow access to all partitions */
            *usage = PSA_KEY_USAGE_DERIVE;
        }
        break;
    case TFM_BUILTIN_KEY_ID_IAK:
        switch(owner) {
        case TFM_SP_INITIAL_ATTESTATION:
            *usage = PSA_KEY_USAGE_SIGN_HASH;
#ifdef SYMMETRIC_INITIAL_ATTESTATION
            /* Needed to calculate the instance ID */
            *usage |= PSA_KEY_USAGE_EXPORT;
#endif /* SYMMETRIC_INITIAL_ATTESTATION */
            break;
#ifdef TFM_SP_SECURE_TEST_PARTITION
        /* So that the tests can validate created tokens */
        case TFM_SP_SECURE_TEST_PARTITION:
        case TFM_NS_PARTITION_ID:
            *usage = PSA_KEY_USAGE_VERIFY_HASH;
            break;
#endif /* TFM_SP_SECURE_TEST_PARTITION */
        default:
            return TFM_PLAT_ERR_NOT_PERMITTED;
        }
        break;
#ifdef TFM_PARTITION_DELEGATED_ATTESTATION
    case TFM_BUILTIN_KEY_ID_DAK_SEED:
        switch(owner) {
        case TFM_SP_DELEGATED_ATTESTATION:
            *usage = PSA_KEY_USAGE_DERIVE;
            break;
        default:
            return TFM_PLAT_ERR_NOT_PERMITTED;
        }
        break;
#endif /* TFM_PARTITION_DELEGATED_ATTESTATION */
    case TFM_BUILTIN_KEY_ID_HOST_S_ROTPK:
        switch(owner) {
        case TFM_NS_PARTITION_ID:
            *usage = PSA_KEY_USAGE_VERIFY_HASH;
            break;
        default:
            return TFM_PLAT_ERR_NOT_PERMITTED;
        }
        break;
    case TFM_BUILTIN_KEY_ID_HOST_NS_ROTPK:
        switch(owner) {
        case TFM_NS_PARTITION_ID:
            *usage = PSA_KEY_USAGE_VERIFY_HASH;
            break;
        default:
            return TFM_PLAT_ERR_NOT_PERMITTED;
        }
        break;
    case TFM_BUILTIN_KEY_ID_HOST_CCA_ROTPK:
        switch(owner) {
        case TFM_NS_PARTITION_ID:
            *usage = PSA_KEY_USAGE_VERIFY_HASH;
            break;
        default:
            return TFM_PLAT_ERR_NOT_PERMITTED;
        }
        break;
    default:
        return TFM_PLAT_ERR_UNSUPPORTED;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_builtin_key_get_lifetime_and_slot(
    mbedtls_svc_key_id_t key_id,
    psa_key_lifetime_t *lifetime,
    psa_drv_slot_number_t *slot_number)
{
    switch (MBEDTLS_SVC_KEY_ID_GET_KEY_ID(key_id)) {
    case TFM_BUILTIN_KEY_ID_HUK:
        *slot_number = TFM_BUILTIN_KEY_SLOT_HUK;
        *lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_LIFETIME_PERSISTENT,
                                                                   TFM_BUILTIN_KEY_LOADER_KEY_LOCATION);
        break;
    case TFM_BUILTIN_KEY_ID_IAK:
        *slot_number = TFM_BUILTIN_KEY_SLOT_IAK;
        *lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_LIFETIME_PERSISTENT,
                                                                   TFM_BUILTIN_KEY_LOADER_KEY_LOCATION);
        break;
#ifdef TFM_PARTITION_DELEGATED_ATTESTATION
    case TFM_BUILTIN_KEY_ID_DAK_SEED:
        *slot_number = TFM_BUILTIN_KEY_SLOT_DAK_SEED;
        *lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_LIFETIME_PERSISTENT,
                                                                   TFM_BUILTIN_KEY_LOADER_KEY_LOCATION);
        break;
#endif /* TFM_PARTITION_DELEGATED_ATTESTATION */
    case TFM_BUILTIN_KEY_ID_HOST_S_ROTPK:
        *slot_number = TFM_BUILTIN_KEY_SLOT_HOST_S_ROTPK;
        *lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_LIFETIME_PERSISTENT,
                                                                   TFM_BUILTIN_KEY_LOADER_KEY_LOCATION);
        break;
    case TFM_BUILTIN_KEY_ID_HOST_NS_ROTPK:
        *slot_number = TFM_BUILTIN_KEY_SLOT_HOST_NS_ROTPK;
        *lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_LIFETIME_PERSISTENT,
                                                                   TFM_BUILTIN_KEY_LOADER_KEY_LOCATION);
        break;
    case TFM_BUILTIN_KEY_ID_HOST_CCA_ROTPK:
        *slot_number = TFM_BUILTIN_KEY_SLOT_HOST_CCA_ROTPK;
        *lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_LIFETIME_PERSISTENT,
                                                                   TFM_BUILTIN_KEY_LOADER_KEY_LOCATION);
        break;
    default:
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    return PSA_SUCCESS;
}

static enum tfm_plat_err_t tfm_plat_get_huk(uint8_t *buf, size_t buf_len,
                                            size_t *key_len,
                                            size_t *key_bits,
                                            psa_algorithm_t *algorithm,
                                            psa_key_type_t *type)
{
    enum kmu_error_t kmu_err;

    if (buf_len < 32) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    *key_len = 32;
    *key_bits = 256;
    *algorithm = PSA_ALG_HKDF(PSA_ALG_SHA_256);
    *type = PSA_KEY_TYPE_DERIVE;

    kmu_err = kmu_get_key(&KMU_DEV_S, KMU_USER_SLOT_MIN + 0, buf, 32);
    if (kmu_err != KMU_ERROR_NONE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return TFM_PLAT_ERR_SUCCESS;
}


static enum tfm_plat_err_t tfm_plat_get_iak(uint8_t *buf, size_t buf_len,
                                     size_t *key_len,
                                     size_t *key_bits,
                                     psa_algorithm_t *algorithm,
                                     psa_key_type_t *type)
{
    psa_status_t status;
    enum kmu_error_t kmu_err;
    psa_key_attributes_t seed_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t transient_attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_handle_t seed_key = PSA_KEY_HANDLE_INIT;
    psa_key_handle_t transient_key = PSA_KEY_HANDLE_INIT;
    psa_key_derivation_operation_t op = PSA_KEY_DERIVATION_OPERATION_INIT;

    if (buf_len < PSA_KEY_EXPORT_ECC_KEY_PAIR_MAX_SIZE(ATTEST_KEY_BITS)) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    kmu_err = kmu_get_key(&KMU_DEV_S, KMU_USER_SLOT_MIN + 1, buf, 32);
    if (kmu_err != KMU_ERROR_NONE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    psa_set_key_type(&seed_attributes, PSA_KEY_TYPE_DERIVE);
    psa_set_key_algorithm(&seed_attributes, PSA_ALG_HKDF(PSA_ALG_SHA_256));
    psa_set_key_bits(&seed_attributes, 256);
    psa_set_key_usage_flags(&seed_attributes, PSA_KEY_USAGE_DERIVE);

    status = psa_import_key(&seed_attributes, buf, 32, &seed_key);
    if (status != PSA_SUCCESS) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    status = psa_key_derivation_setup(&op, PSA_ALG_HKDF(PSA_ALG_SHA_256));
    if (status != PSA_SUCCESS) {
        goto err_release_seed_key;
    }

    status = psa_key_derivation_input_key(&op, PSA_KEY_DERIVATION_INPUT_SECRET,
                                          seed_key);
    if (status != PSA_SUCCESS) {
        goto err_release_op;
    }

    status = psa_key_derivation_input_bytes(&op, PSA_KEY_DERIVATION_INPUT_INFO,
                                            NULL, 0);
    if (status != PSA_SUCCESS) {
        goto err_release_op;
    }

    *key_bits = ATTEST_KEY_BITS;
    *algorithm = PSA_ALG_ECDSA(PSA_ALG_SHA_384);
    *type = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1);

    psa_set_key_type(&transient_attr, *type);
    psa_set_key_algorithm(&transient_attr, *algorithm);
    psa_set_key_bits(&transient_attr, *key_bits);
    psa_set_key_usage_flags(&transient_attr, PSA_KEY_USAGE_EXPORT);

    status = psa_key_derivation_output_key(&transient_attr, &op, &transient_key);
    if (status != PSA_SUCCESS) {
        goto err_release_op;
    }

    status = psa_export_key(transient_key, buf, buf_len, key_len);
    if (status != PSA_SUCCESS) {
        goto err_release_transient_key;
    }

    status = psa_destroy_key(transient_key);
    if (status != PSA_SUCCESS) {
        goto err_release_op;
    }

    status = psa_key_derivation_abort(&op);
    if (status != PSA_SUCCESS) {
        goto err_release_seed_key;
    }

    status = psa_destroy_key(seed_key);
    if (status != PSA_SUCCESS) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return PSA_SUCCESS;

err_release_transient_key:
    (void)psa_destroy_key(transient_key);

err_release_op:
    (void)psa_key_derivation_abort(&op);

err_release_seed_key:
    (void)psa_destroy_key(seed_key);

    return TFM_PLAT_ERR_SYSTEM_ERR;
}

#ifdef TFM_PARTITION_DELEGATED_ATTESTATION
static enum tfm_plat_err_t tfm_plat_get_dak_seed(uint8_t *buf, size_t buf_len,
                                                 size_t *key_len,
                                                 size_t *key_bits,
                                                 psa_algorithm_t *algorithm,
                                                 psa_key_type_t *type)
{
    enum kmu_error_t kmu_err;

    if (buf_len < 32) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    *key_len = 32;
    *key_bits = 256;
    *algorithm = PSA_ALG_HKDF(PSA_ALG_SHA_256);
    *type = PSA_KEY_TYPE_DERIVE;

    kmu_err = kmu_get_key(&KMU_DEV_S, KMU_USER_SLOT_MIN + 2, buf, 32);
    if (kmu_err != KMU_ERROR_NONE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return TFM_PLAT_ERR_SUCCESS;
}
#endif /* TFM_PARTITION_DELEGATED_ATTESTATION */

static enum tfm_plat_err_t tfm_plat_get_host_s_rotpk(uint8_t *buf, size_t buf_len,
                                                     size_t *key_len,
                                                     size_t *key_bits,
                                                     psa_algorithm_t *algorithm,
                                                     psa_key_type_t *type)
{
    if (buf_len < 96) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* P384 public keys are 96 bytes in length */
    *key_len = 96;
    *key_bits = 384;
    *algorithm = PSA_ALG_ECDSA(PSA_ALG_SHA_384);
    *type = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1);

    return tfm_plat_otp_read(PLAT_OTP_ID_HOST_ROTPK_S, buf_len, buf);
}

static enum tfm_plat_err_t tfm_plat_get_host_ns_rotpk(uint8_t *buf, size_t buf_len,
                                                      size_t *key_len,
                                                      size_t *key_bits,
                                                      psa_algorithm_t *algorithm,
                                                      psa_key_type_t *type)
{
    if (buf_len < 96) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* P384 public keys are 96 bytes in length */
    *key_len = 96;
    *key_bits = 384;
    *algorithm = PSA_ALG_ECDSA(PSA_ALG_SHA_384);
    *type = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1);

    return tfm_plat_otp_read(PLAT_OTP_ID_HOST_ROTPK_NS, buf_len, buf);
}

static enum tfm_plat_err_t tfm_plat_get_host_cca_rotpk(uint8_t *buf, size_t buf_len,
                                                       size_t *key_len,
                                                       size_t *key_bits,
                                                       psa_algorithm_t *algorithm,
                                                       psa_key_type_t *type)
{
    if (buf_len < 96) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* P384 public keys are 96 bytes in length */
    *key_len = 96;
    *key_bits = 384;
    *algorithm = PSA_ALG_ECDSA(PSA_ALG_SHA_384);
    *type = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1);

    return tfm_plat_otp_read(PLAT_OTP_ID_HOST_ROTPK_CCA, buf_len, buf);
}

enum tfm_plat_err_t tfm_plat_load_builtin_keys(void)
{
    psa_status_t err;
    mbedtls_svc_key_id_t key_id;
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    enum tfm_plat_err_t plat_err;
    /* The KMU requires word alignment */
    uint8_t __ALIGNED(4) buf[96];
    size_t key_len;
    size_t key_bits;
    psa_algorithm_t algorithm;
    psa_key_type_t type;

    /* HUK */
    plat_err = tfm_plat_get_huk(buf, sizeof(buf), &key_len, &key_bits,
                                &algorithm, &type);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return plat_err;
    }
    key_id.MBEDTLS_PRIVATE(key_id) = TFM_BUILTIN_KEY_ID_HUK;
    key_id.MBEDTLS_PRIVATE(owner) = 0;
    psa_set_key_id(&attr, key_id);
    psa_set_key_bits(&attr, key_bits);
    psa_set_key_algorithm(&attr, algorithm);
    psa_set_key_type(&attr, type);
    err = tfm_builtin_key_loader_load_key(buf, key_len, &attr);
    if (err != PSA_SUCCESS) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* IAK */
    plat_err = tfm_plat_get_iak(buf, sizeof(buf), &key_len, &key_bits,
                                &algorithm, &type);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    key_id.MBEDTLS_PRIVATE(key_id) = TFM_BUILTIN_KEY_ID_IAK;
    key_id.MBEDTLS_PRIVATE(owner) = 0;
    psa_set_key_id(&attr, key_id);
    psa_set_key_bits(&attr, key_bits);
    psa_set_key_algorithm(&attr, algorithm);
    psa_set_key_type(&attr, type);
    err = tfm_builtin_key_loader_load_key(buf, key_len, &attr);
    if (err != PSA_SUCCESS) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

#ifdef TFM_PARTITION_DELEGATED_ATTESTATION
    /* DAK SEED */
    plat_err = tfm_plat_get_dak_seed(buf, sizeof(buf), &key_len, &key_bits,
                                     &algorithm, &type);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return plat_err;
    }
    key_id.MBEDTLS_PRIVATE(key_id) = TFM_BUILTIN_KEY_ID_DAK_SEED;
    key_id.MBEDTLS_PRIVATE(owner) = 0;
    psa_set_key_id(&attr, key_id);
    psa_set_key_bits(&attr, key_bits);
    psa_set_key_algorithm(&attr, algorithm);
    psa_set_key_type(&attr, type);
    err = tfm_builtin_key_loader_load_key(buf, key_len, &attr);
    if (err != PSA_SUCCESS) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
#endif /* TFM_PARTITION_DELEGATED_ATTESTATION */

    /* HOST S ROTPK */
    plat_err = tfm_plat_get_host_s_rotpk(buf, sizeof(buf), &key_len, &key_bits,
                                         &algorithm, &type);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return plat_err;
    }
    key_id.MBEDTLS_PRIVATE(key_id) = TFM_BUILTIN_KEY_ID_HOST_S_ROTPK;
    key_id.MBEDTLS_PRIVATE(owner) = 0;
    psa_set_key_id(&attr, key_id);
    psa_set_key_bits(&attr, key_bits);
    psa_set_key_algorithm(&attr, algorithm);
    psa_set_key_type(&attr, type);
    err = tfm_builtin_key_loader_load_key(buf, key_len, &attr);
    if (err != PSA_SUCCESS) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* HOST NS ROTPK */
    plat_err = tfm_plat_get_host_ns_rotpk(buf, sizeof(buf), &key_len, &key_bits,
                                         &algorithm, &type);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return plat_err;
    }
    key_id.MBEDTLS_PRIVATE(key_id) = TFM_BUILTIN_KEY_ID_HOST_NS_ROTPK;
    key_id.MBEDTLS_PRIVATE(owner) = 0;
    psa_set_key_id(&attr, key_id);
    psa_set_key_bits(&attr, key_bits);
    psa_set_key_algorithm(&attr, algorithm);
    psa_set_key_type(&attr, type);
    err = tfm_builtin_key_loader_load_key(buf, key_len, &attr);
    if (err != PSA_SUCCESS) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* HOST CCA ROTPK */
    plat_err = tfm_plat_get_host_cca_rotpk(buf, sizeof(buf), &key_len, &key_bits,
                                         &algorithm, &type);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return plat_err;
    }
    key_id.MBEDTLS_PRIVATE(key_id) = TFM_BUILTIN_KEY_ID_HOST_CCA_ROTPK;
    key_id.MBEDTLS_PRIVATE(owner) = 0;
    psa_set_key_id(&attr, key_id);
    psa_set_key_bits(&attr, key_bits);
    psa_set_key_algorithm(&attr, algorithm);
    psa_set_key_type(&attr, type);
    err = tfm_builtin_key_loader_load_key(buf, key_len, &attr);
    if (err != PSA_SUCCESS) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return TFM_PLAT_ERR_SUCCESS;
}
