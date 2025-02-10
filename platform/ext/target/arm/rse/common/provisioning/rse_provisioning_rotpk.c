/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "rse_provisioning_rotpk.h"

#include "device_definition.h"
#include "rse_provisioning_config.h"
#include "crypto.h"
#include "cc3xx_drv.h"
#include "rse_rotpk_policy.h"
#include "rse_rotpk_mapping.h"

#ifdef RSE_PROVISIONING_ENABLE_ECDSA_SIGNATURES

#if defined(RSE_PROVISIONING_CURVE_P384)
/* bl2/ext/mcuboot/root-EC-P384.pem */
const uint8_t __ALIGNED(sizeof(uint32_t)) tci_rotpk_x[] = {
    0x0c, 0x76, 0xca, 0xae, 0x72, 0x3a, 0xa5, 0xe8,
    0xf0, 0xd4, 0xf1, 0x16, 0xb5, 0x02, 0xef, 0x77,
    0xa1, 0x1b, 0x93, 0x61, 0x78, 0xc0, 0x09, 0x26,
    0x7b, 0x3b, 0x40, 0x9c, 0xee, 0x49, 0x85, 0xe0,
    0xc9, 0x4f, 0xe7, 0xf2, 0xba, 0x97, 0x6c, 0xf3,
    0x82, 0x65, 0x14, 0x2c, 0xf5, 0x0c, 0x73, 0x33,
};

const uint8_t __ALIGNED(sizeof(uint32_t)) tci_rotpk_y[] = {
    0x4d, 0x32, 0xe7, 0x9b, 0xd3, 0x42, 0xcc, 0x95,
    0x5a, 0xe5, 0xe2, 0xf5, 0xf4, 0x6e, 0x45, 0xe0,
    0xed, 0x20, 0x35, 0x5c, 0xaf, 0x52, 0x35, 0x81,
    0xd4, 0xdc, 0x9c, 0xe3, 0x9e, 0x22, 0x3e, 0xfb,
    0x3f, 0x22, 0x10, 0xda, 0x70, 0x03, 0x37, 0xad,
    0xa8, 0xf2, 0x48, 0xfe, 0x3a, 0x60, 0x69, 0xa5
};
#elif defined(RSE_PROVISIONING_CURVE_P256)
const uint8_t __ALIGNED(sizeof(uint32_t)) tci_rotpk_x[] = {
  0x2a, 0xcb, 0x40, 0x3c, 0xe8, 0xfe, 0xed, 0x5b,
  0xa4, 0x49, 0x95, 0xa1, 0xa9, 0x1d, 0xae, 0xe8,
  0xdb, 0xbe, 0x19, 0x37, 0xcd, 0x14, 0xfb, 0x2f,
  0x24, 0x57, 0x37, 0xe5, 0x95, 0x39, 0x88, 0xd9,
};

const uint8_t __ALIGNED(sizeof(uint32_t)) tci_rotpk_y[] = {
  0x94, 0xb9, 0xd6, 0x5a, 0xeb, 0xd7, 0xcd, 0xd5,
  0x30, 0x8a, 0xd6, 0xfe, 0x48, 0xb2, 0x4a, 0x6a,
  0x81, 0x0e, 0xe5, 0xf0, 0x7d, 0x8b, 0x68, 0x34,
  0xcc, 0x3a, 0x6a, 0xfc, 0x53, 0x8e, 0xfa, 0xc1
};
#else
#error No key data for provisioning curve
#endif

const size_t tci_rotpk_x_len = sizeof(tci_rotpk_x);
const size_t tci_rotpk_y_len = sizeof(tci_rotpk_y);

#ifdef TFM_DUMMY_PROVISIONING

/* bl2/ext/mcuboot/root-EC-P384.pem */
const uint8_t *pci_rotpk_x = tci_rotpk_x;
const uint8_t *pci_rotpk_y = tci_rotpk_y;
const size_t pci_rotpk_x_len = sizeof(tci_rotpk_x);
const size_t pci_rotpk_y_len = sizeof(tci_rotpk_y);

#else

extern const uint8_t pci_rotpk_x[];
extern const uint8_t pci_rotpk_y[];

extern const size_t pci_rotpk_x_len;
extern const size_t pci_rotpk_y_len;

#endif

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
        return TFM_PLAT_ERR_PROVISIONING_BLOB_INVALID_HASH_SIZE;
    }

    if (memcmp(key_hash_from_otp, key_hash_from_blob, key_hash_from_otp_size)) {
        return TFM_PLAT_ERR_PROVISIONING_BLOB_INVALID_HASH_VALUE;
    }

    *public_key_x = (uint32_t *)blob->public_key;
    *public_key_x_size = point_size;
    *public_key_y = (uint32_t *)(blob->public_key + point_size);
    *public_key_y_size = point_size;

    return TFM_PLAT_ERR_SUCCESS;
}

static enum tfm_plat_err_t get_rom_rotpk(uint32_t **public_key_x,
                                        size_t *public_key_x_size,
                                        uint32_t **public_key_y,
                                        size_t *public_key_y_size)
{
    enum lcm_tp_mode_t tp_mode;

    lcm_get_tp_mode(&LCM_DEV_S, &tp_mode);

    switch(tp_mode) {
    case LCM_TP_MODE_TCI:
        *public_key_x = (uint32_t *)tci_rotpk_x;
        *public_key_y = (uint32_t *)tci_rotpk_y;
        *public_key_x_size = tci_rotpk_x_len;
        *public_key_y_size = tci_rotpk_y_len;
        return TFM_PLAT_ERR_SUCCESS;
    default:
        *public_key_x = (uint32_t *)pci_rotpk_x;
        *public_key_y = (uint32_t *)pci_rotpk_y;
        *public_key_x_size = pci_rotpk_x_len;
        *public_key_y_size = pci_rotpk_y_len;
        return TFM_PLAT_ERR_SUCCESS;
    }
}

enum tfm_plat_err_t provisioning_rotpk_get(const struct rse_provisioning_message_blob_t *blob,
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

    return get_rom_rotpk(public_key_x,
                         public_key_x_size,
                         public_key_y,
                         public_key_y_size);
}
#endif
