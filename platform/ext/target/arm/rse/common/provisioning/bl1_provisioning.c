/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_plat_provisioning.h"

#include "region_defs.h"
#include "rse_provisioning_message_handler.h"
#include "rse_provisioning_comms.h"
#include "device_definition.h"
#include "tfm_log.h"
#include "tfm_hal_platform.h"
#include "rse_otp_dev.h"
#include "tfm_plat_otp.h"
#include "rse_kmu_slot_ids.h"
#include "rse_kmu_keys.h"
#include "rse_provisioning_rotpk.h"
#include "fatal_error.h"
#include "rse_rotpk_policy.h"
#include "rse_rotpk_mapping.h"
#include "crypto.h"
#include "cc3xx_drv.h"

#include <string.h>

#define PROVISIONING_MESSAGE_START (VM0_BASE_S + OTP_DMA_ICS_SIZE)
#define RSE_PROVISIONING_MESSAGE_MAX_SIZE (VM1_BASE_S - PROVISIONING_MESSAGE_START)

static const struct rse_provisioning_message_t *provisioning_message =
(const struct rse_provisioning_message_t *)PROVISIONING_MESSAGE_START;

static inline bool blob_is_combined(const struct rse_provisioning_message_blob_t *blob)
{
    return (((blob->purpose >> RSE_PROVISIONING_BLOB_PURPOSE_TYPE_OFFSET)
             & RSE_PROVISIONING_BLOB_PURPOSE_TYPE_MASK)
            == RSE_PROVISIONING_BLOB_TYPE_COMBINED_LCS_PROVISIONING);
}

static enum tfm_plat_err_t aes_setup_key(const struct rse_provisioning_message_blob_t *blob,
                                         uint32_t *key_id)
{
    enum lcm_error_t lcm_err;
    enum tfm_plat_err_t err;
    enum lcm_lcs_t lcs;
    uint8_t *label;
    uint8_t label_len = 0;
    uint8_t *context;
    size_t context_len = 0;

    lcm_err = lcm_get_lcs(&LCM_DEV_S, &lcs);
    if (lcm_err != LCM_ERROR_NONE) {
        return (enum tfm_plat_err_t)lcm_err;
    }

    if (lcs != LCM_LCS_CM && lcs != LCM_LCS_DM) {
        FATAL_ERR(TFM_PLAT_ERR_PROVISIONING_DERIVATION_INVALID_LCS);
        return TFM_PLAT_ERR_PROVISIONING_DERIVATION_INVALID_LCS;
    }

    if (lcs == LCM_LCS_CM || blob_is_combined(blob)) {
        label = (uint8_t *)"KMASTER_CM";
        label_len = sizeof("KMASTER_CM");
        context = (uint8_t *)&blob->batch_id;
        context_len = sizeof(blob->batch_id);
    } else {
        label = (uint8_t *)"KMASTER_DM";
        label_len = sizeof("KMASTER_DM");
        context = (uint8_t *)&blob->dm_number;
        context_len = sizeof(blob->dm_number);
    }

    err = rse_setup_master_key(label, label_len, context, context_len);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    *key_id = RSE_KMU_SLOT_PROVISIONING_KEY;

    if (lcs == LCM_LCS_CM || blob_is_combined(blob)) {
        label = (uint8_t *)"KPROV_CM";
        label_len = sizeof("KPROV_CM");
    } else {
        label = (uint8_t *)"KPROV_DM";
        label_len = sizeof("KPROV_DM");
    }

    return rse_setup_provisioning_key(label, label_len, NULL, 0);
}

static inline bool use_cm_rotpk(const struct rse_provisioning_message_blob_t *blob)
{
    bool rotpk_not_in_rom, use_cm_rotpk;

    rotpk_not_in_rom = ((blob->metadata >> RSE_PROVISIONING_BLOB_DETAILS_SIGNATURE_OFFSET)
                       & RSE_PROVISIONING_BLOB_DETAILS_SIGNATURE_MASK)
                       == RSE_PROVISIONING_BLOB_SIGNATURE_ROTPK_NOT_IN_ROM;
    use_cm_rotpk = ((blob->metadata >> RSE_PROVISIONING_BLOB_DETAILS_NON_ROM_PK_TYPE_OFFSET)
                   & RSE_PROVISIONING_BLOB_DETAILS_NON_ROM_PK_TYPE_MASK)
                   == RSE_PROVISIONING_BLOB_DETAILS_NON_ROM_PK_TYPE_CM_ROTPK;

    return rotpk_not_in_rom && use_cm_rotpk;
}

static enum tfm_plat_err_t get_key_hash_from_otp(enum tfm_otp_element_id_t id,
                                                 uint8_t *cm_rotpk_hash,
                                                 size_t *cm_rotpk_hash_size)
{
    enum tfm_plat_err_t err;
    enum rse_rotpk_hash_alg alg;
    bool is_valid_alg;

    err = tfm_plat_otp_read(id, RSE_PROVISIONING_HASH_MAX_SIZE, cm_rotpk_hash);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = rse_rotpk_get_hash_alg(id, &alg);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    /* Algorithm should match that used for provisioning */
    is_valid_alg = ((alg == RSE_ROTPK_HASH_ALG_SHA256)
                   && (RSE_PROVISIONING_HASH_ALG == TFM_BL1_HASH_ALG_SHA256))
                   || ((alg == RSE_ROTPK_HASH_ALG_SHA384)
                   && (RSE_PROVISIONING_HASH_ALG == TFM_BL1_HASH_ALG_SHA384));
    if (!is_valid_alg) {
        FATAL_ERR(TFM_PLAT_ERR_PROVISIONING_BLOB_INVALID_HASH_ALG);
        return TFM_PLAT_ERR_PROVISIONING_BLOB_INVALID_HASH_ALG;
    }

    *cm_rotpk_hash_size = RSE_ROTPK_SIZE_FROM_ALG(alg);

    return err;
}

/* X.509 RFC 5280 object identifier definitions */
#define OID_ID_EC_PUBLIC_KEY "\x2A\x86\x48\xCE\x3D\x02\x01"  /* 1.2.840.10045.2.1 */
#define OID_SECP256R1        "\x2A\x86\x48\xCE\x3D\x03\x01\x07"  /* 1.2.840.10045.3.1.7 (P-256) */
#define OID_SECP384R1        "\x2B\x81\x04\x00\x22"  /* 1.3.132.0.34 (P-384) */

static enum tfm_plat_err_t get_asn1_from_raw_ec(const uint8_t *x, size_t x_size,
                                                const uint8_t *y, size_t y_size,
                                                cc3xx_ec_curve_id_t curve_id,
                                                uint8_t *asn1_key,
                                                size_t *len)
{
    uint8_t *key_ptr = asn1_key;
    const uint8_t *oid_curve;
    size_t oid_curve_len;
    uint8_t *p_len, *p_bitlen, *p_alglen;
    const size_t oid_pub_len = sizeof(OID_ID_EC_PUBLIC_KEY) - 1;

    switch (curve_id) {
    case CC3XX_EC_CURVE_SECP_256_R1:
        oid_curve = (const uint8_t *)OID_SECP256R1;
        oid_curve_len = sizeof(OID_SECP256R1) - 1;
        break;
    case CC3XX_EC_CURVE_SECP_384_R1:
        oid_curve = (const uint8_t *)OID_SECP384R1;
        oid_curve_len = sizeof(OID_SECP384R1) - 1;
        break;
    default:
        FATAL_ERR(TFM_PLAT_ERR_PROVISIONING_BLOB_INVALID_CURVE);
        return TFM_PLAT_ERR_PROVISIONING_BLOB_INVALID_CURVE;
    }

    /* Start SEQUENCE (SubjectPublicKeyInfo) */
    *key_ptr++ = 0x30;
    p_len = key_ptr;
    key_ptr++;

    /* AlgorithmIdentifier SEQUENCE */
    *key_ptr++ = 0x30;
    p_alglen = key_ptr;
    key_ptr++;

    /* OID for id-ecPublicKey */
    *key_ptr++ = 0x06;
    *key_ptr++ = oid_pub_len;
    memcpy(key_ptr, OID_ID_EC_PUBLIC_KEY, oid_pub_len);
    key_ptr += oid_pub_len;

    /* OID for curve (P-256 or P-384) */
    *key_ptr++ = 0x06;
    *key_ptr++ = oid_curve_len;
    memcpy(key_ptr, oid_curve, oid_curve_len);
    key_ptr += oid_curve_len;

    *p_alglen = key_ptr - (p_alglen + 1);

    /* BIT STRING for public key */
    *key_ptr++ = 0x03;
    p_bitlen = key_ptr;
    key_ptr++;
    *key_ptr++ = 0x00;

    /* Uncompressed public key (0x04 || X || Y) */
    *key_ptr++ = 0x04;
    memcpy(key_ptr, x, x_size);
    key_ptr += x_size;
    memcpy(key_ptr, y, y_size);
    key_ptr += y_size;

    *p_bitlen = key_ptr - (p_bitlen + 1);
    *p_len = key_ptr - (p_len + 1);

    *len = key_ptr - asn1_key;

    return TFM_PLAT_ERR_SUCCESS;
}

static enum tfm_plat_err_t
calc_key_hash_from_blob(const struct rse_provisioning_message_blob_t *blob,
                        uint8_t *key_hash,
                        size_t *key_hash_size,
                        size_t point_size)
{
    fih_int fih_rc;
    enum tfm_plat_err_t err;
    /* Max size is 120 bytes */
    uint8_t asn1_key[120];
    size_t len;

    /* Currently keys stored in CM ROTPK are in the form of hashed DER encoded,
     * ASN.1 format keys. Therefore we need to convert the key we have found
     * in the blob to this format, before hashing it */
    err = get_asn1_from_raw_ec(blob->public_key,
                               point_size,
                               blob->public_key + point_size,
                               point_size,
                               RSE_PROVISIONING_CURVE,
                               asn1_key,
                               &len);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    FIH_CALL(bl1_hash_compute, fih_rc, RSE_PROVISIONING_HASH_ALG,
                                       asn1_key,
                                       len,
                                       key_hash,
                                       RSE_PROVISIONING_HASH_MAX_SIZE,
                                       key_hash_size);
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        return (enum tfm_plat_err_t)fih_rc;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

static enum tfm_plat_err_t get_check_hash_cm_rotpk(const struct rse_provisioning_message_blob_t *blob,
                                                   uint32_t **public_key_x,
                                                   size_t *public_key_x_size,
                                                   uint32_t **public_key_y,
                                                   size_t *public_key_y_size)
{
    uint16_t cm_rotpk_num;
    enum tfm_otp_element_id_t id;
    enum tfm_plat_err_t err;
    uint8_t key_hash_from_otp[RSE_PROVISIONING_HASH_MAX_SIZE];
    uint8_t key_hash_from_blob[RSE_PROVISIONING_HASH_MAX_SIZE];
    uint32_t point_size;
    size_t key_hash_from_otp_size, key_hash_from_blob_size;

    cm_rotpk_num = (blob->metadata >> RSE_PROVISIONING_BLOB_DETAILS_CM_ROTPK_NUMBER_OFFSET)
                   & RSE_PROVISIONING_BLOB_DETAILS_CM_ROTPK_NUMBER_MASK;
    id = PLAT_OTP_ID_CM_ROTPK + cm_rotpk_num;

    if (id >= PLAT_OTP_ID_CM_ROTPK_MAX) {
        FATAL_ERR(TFM_PLAT_ERR_PROVISIONING_BLOB_INVALID_CM_ROTPK);
        return TFM_PLAT_ERR_PROVISIONING_BLOB_INVALID_CM_ROTPK;
    }

    point_size = cc3xx_lowlevel_ec_get_modulus_size_from_curve(RSE_PROVISIONING_CURVE);

    err = get_key_hash_from_otp(id, key_hash_from_otp, &key_hash_from_otp_size);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = calc_key_hash_from_blob(blob, key_hash_from_blob, &key_hash_from_blob_size, point_size);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    if (key_hash_from_otp_size != key_hash_from_blob_size) {
        FATAL_ERR(TFM_PLAT_ERR_PROVISIONING_BLOB_INVALID_HASH_SIZE);
        return TFM_PLAT_ERR_PROVISIONING_BLOB_INVALID_HASH_SIZE;
    }

    if (memcmp(key_hash_from_otp, key_hash_from_blob, key_hash_from_otp_size)) {
        FATAL_ERR(TFM_PLAT_ERR_PROVISIONING_BLOB_INVALID_HASH_VALUE);
        return TFM_PLAT_ERR_PROVISIONING_BLOB_INVALID_HASH_VALUE;
    }

    *public_key_x = (uint32_t *)blob->public_key;
    *public_key_x_size = point_size;
    *public_key_y = (uint32_t *)(blob->public_key + point_size);
    *public_key_y_size = point_size;

    return TFM_PLAT_ERR_SUCCESS;
}

static enum tfm_plat_err_t rotpk_get(const struct rse_provisioning_message_blob_t *blob,
                                     uint32_t **public_key_x,
                                     size_t *public_key_x_size,
                                     uint32_t **public_key_y,
                                     size_t *public_key_y_size)
{
    enum lcm_error_t lcm_err;
    enum lcm_lcs_t lcs;

    lcm_err = lcm_get_lcs(&LCM_DEV_S, &lcs);
    if (lcm_err != LCM_ERROR_NONE) {
        return (enum tfm_plat_err_t)lcm_err;
    }

    if ((lcs == LCM_LCS_DM) && use_cm_rotpk(blob)) {
        return get_check_hash_cm_rotpk(blob,
                                       public_key_x,
                                       public_key_x_size,
                                       public_key_y,
                                       public_key_y_size);
    }

    return provisioning_rotpk_get(public_key_x,
                                  public_key_x_size,
                                  public_key_y,
                                  public_key_y_size);
}

void tfm_plat_provisioning_check_for_dummy_keys(void)
{
    /* FixMe: Check for dummy key must be implemented */
}

int tfm_plat_provisioning_is_required(void)
{
    enum lcm_error_t err;
    enum lcm_lcs_t lcs;
    bool provisioning_required;

    err = lcm_get_lcs(&LCM_DEV_S, &lcs);
    if (err != LCM_ERROR_NONE) {
        return err;
    }

    provisioning_required = (lcs == LCM_LCS_CM || lcs == LCM_LCS_DM);

    return provisioning_required;
}

enum tfm_plat_err_t tfm_plat_provisioning_perform(void)
{
    INFO("Beginning RSE provisioning\n");
    enum tfm_plat_err_t err;
    size_t msg_len;

    struct provisioning_message_handler_config config = {
        .blob_handler = &default_blob_handler,
    };

    struct default_blob_handler_ctx_t ctx = {
        .setup_aes_key = aes_setup_key,
#ifdef RSE_PROVISIONING_ENABLE_ECDSA_SIGNATURES
        .get_rotpk = rotpk_get,
#endif
        .blob_is_chainloaded = false,
    };

    if (provisioning_message->header.data_length == 0) {
        err = provisioning_comms_receive(provisioning_message,
                                         RSE_PROVISIONING_MESSAGE_MAX_SIZE,
                                         &msg_len);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }
    }

    /* FixMe: Check if the current way of handling blobs can result in running
     *        a malformed blob in case a reset happens in the middle of an
     *        operation which has already written the blob header
     */
    err = handle_provisioning_message(provisioning_message,
                                      RSE_PROVISIONING_MESSAGE_MAX_SIZE,
                                      &config, (void *)&ctx);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        provisioning_comms_return_status(err);
        memset((void *)provisioning_message, 0, RSE_PROVISIONING_MESSAGE_MAX_SIZE);
        return err;
    }

    tfm_hal_system_reset();

    return TFM_PLAT_ERR_SUCCESS;
}
