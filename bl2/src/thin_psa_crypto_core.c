/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/**
 * \note This source file is derivative work of psa_crypto.c from the Mbed TLS project
 */
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "common.h"
#include "psa/crypto.h"
#include "psa_crypto_driver_wrappers.h"
#include "psa_crypto_driver_wrappers_no_static.h"
#ifdef MCUBOOT_BUILTIN_KEY
#include "tfm_plat_crypto_keys.h"
#include "tfm_plat_otp.h"
#endif /* MCUBOOT_BUILTIN_KEY */

/* For mbedtls_psa_ecdsa_verify_hash() */
#include "psa_crypto_ecp.h"

/* A few Mbed TLS definitions */
#include "mbedtls/entropy.h"
#include "mbedtls/ecp.h"

/* Definitions required for the MCUBOOT_BUILTIN_KEY case */
#if defined(MCUBOOT_BUILTIN_KEY)
#define MCUBOOT_BUILTIN_KEY_ID_MIN   (0x1)
#define MCUBOOT_BUILTIN_KEY_ID_MAX   (MCUBOOT_IMAGE_NUMBER)
#if defined(MCUBOOT_SIGN_EC256)
#define PUBKEY_DATA_SIZE             (65)
#define PUBKEY_BUF_SIZE              (68) /* Must be aligned to 4 Bytes */
#elif defined(MCUBOOT_SIGN_EC384)
#define PUBKEY_DATA_SIZE             (97)
#define PUBKEY_BUF_SIZE              (100) /* Must be aligned to 4 Bytes */
#endif /* MCUBOOT_SIGN_EC256 */
#endif /* MCUBOOT_BUILTIN_KEY */

/**
 * @note MCUboot uses the keys by importing them after parsing and then using
 *       them right away in the following call, so it makes sense to avoid a copy
 *       of data and just directly use the pointer to key material from the
 *       caller. The psa_import_key() call does not cross any security boundary.
 */

/**
 * @brief A structure describing the contents of a thin key slot, which
 *        holds key material and metadata following a psa_import_key() call
 */
struct thin_key_slot_s {
    const uint8_t *buf;        /*!< Pointer to the buffer holding the key material */
    size_t len;                /*!< Size in bytes of the \a buf buffer */
    psa_key_attributes_t attr; /*!< Attributes of the key */
    psa_key_id_t key_id;       /*!< Key ID assigned to the key */
    bool is_valid;             /*!< Boolean value, true if the key material is valid */
};

#if defined(MCUBOOT_BUILTIN_KEY)
/**
 * @brief Static local buffer that holds enough data for the key material
 *        provisioned bundle to be retrieved from the platform
 */
static uint8_t g_pubkey_data[PUBKEY_BUF_SIZE];
#endif /* MCUBOOT_BUILTIN_KEY */

/**
 * @brief This static global variable holds the key slot. The thin PSA Crypto core
 *        supports only a single key to be imported at any given time. Importing a
 *        new key will just cause the existing key to be forgotten
 */
static struct thin_key_slot_s g_key_slot =  {
#if !defined(MCUBOOT_BUILTIN_KEY)
    .buf = NULL,
    .len = 0,
    .attr = PSA_KEY_ATTRIBUTES_INIT,
    .key_id = PSA_KEY_ID_NULL,
    .is_valid = false,
#else
    .buf  = g_pubkey_data,
    .len  = sizeof(g_pubkey_data),
    .attr = PSA_KEY_ATTRIBUTES_INIT,
    /* .key_id   - not used
     * .is_valid - not used
     */
#endif /* !MCUBOOT_BUILTIN_KEY */
};

/**
 * @brief Context required by the RNG function, mocked
 *
 */
static mbedtls_psa_external_random_context_t *g_ctx = NULL;

#if defined(MCUBOOT_BUILTIN_KEY)
static psa_status_t get_builtin_public_key(psa_key_id_t key_id,
                                           psa_algorithm_t alg)
{
    enum tfm_plat_err_t plat_err;

    /* Decrease key ID by MCUBOOT_BUILTIN_KEY_ID_MIN as zero (PSA_KEY_ID_NULL)
     * is reserved and considered invalid.
     */
    plat_err = tfm_plat_otp_read(
            (PLAT_OTP_ID_BL2_ROTPK_0 + (key_id - MCUBOOT_BUILTIN_KEY_ID_MIN)),
            PUBKEY_BUF_SIZE,
            g_pubkey_data);
    if (plat_err == TFM_PLAT_ERR_SUCCESS) {
        g_key_slot.len = PUBKEY_DATA_SIZE;
    } else {
        return PSA_ERROR_GENERIC_ERROR;
    }

    psa_set_key_usage_flags(&g_key_slot.attr, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&g_key_slot.attr, alg);
    psa_set_key_type(&g_key_slot.attr,
                     PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&g_key_slot.attr,
                     PSA_BYTES_TO_BITS((g_key_slot.len - 1)/2));

    return PSA_SUCCESS;
}
#endif /* MCUBOOT_BUILTIN_KEY */

psa_status_t psa_crypto_init(void)
{
    psa_status_t status = psa_driver_wrapper_init();

    /* This will have to perform RNG/DRBG init in case that will be ever required by
     * any API
     */
    return status;
}

psa_status_t psa_hash_abort(psa_hash_operation_t *operation)
{
    /* Aborting a non-active operation is allowed */
    if (operation->id == 0) {
        return PSA_SUCCESS;
    }

    psa_status_t status = psa_driver_wrapper_hash_abort(operation);

    operation->id = 0;

    return status;
}

psa_status_t psa_hash_setup(psa_hash_operation_t *operation,
                            psa_algorithm_t alg)
{
    psa_status_t status;

    /* A context must be freshly initialized before it can be set up. */
    assert(operation->id == 0);
    assert(PSA_ALG_IS_HASH(alg));

    /* Ensure all of the context is zeroized, since PSA_HASH_OPERATION_INIT only
     * directly zeroes the int-sized dummy member of the context union.
     */
    memset(&operation->ctx, 0, sizeof(operation->ctx));

    status = psa_driver_wrapper_hash_setup(operation, alg);

    assert(status == PSA_SUCCESS);

    return status;
}

psa_status_t psa_hash_update(psa_hash_operation_t *operation,
                             const uint8_t *input,
                             size_t input_length)
{
    psa_status_t status;

    assert(operation->id != 0);

    /* Don't require hash implementations to behave correctly on a
     * zero-length input, which may have an invalid pointer.
     */
    if (input_length == 0) {
        return PSA_SUCCESS;
    }

    status = psa_driver_wrapper_hash_update(operation, input, input_length);

    assert(status == PSA_SUCCESS);

    return status;
}

psa_status_t psa_hash_finish(psa_hash_operation_t *operation,
                             uint8_t *hash,
                             size_t hash_size,
                             size_t *hash_length)
{
    *hash_length = 0;
    assert(operation->id != 0);

    psa_status_t status = psa_driver_wrapper_hash_finish(
        operation, hash, hash_size, hash_length);
    psa_hash_abort(operation);
    return status;
}

#if !defined(MCUBOOT_BUILTIN_KEY)
/**
 * The key management subsystem is simplified to support only the BL2 required
 * use case for signature verification either with RSA or ECDSA. MCUboot key
 * bundles can be encoded in the SubjectPublicKeyInfo format (RFC 5480):
 *
 * SubjectPublicKeyInfo  ::= SEQUENCE  {
 *     algorithm            AlgorithmIdentifier,
 *     subjectPublicKey     BIT STRING
 * }
 *
 * where, for RSA, the subjectPublicKey is either specified in RFC 3447 (PKCS#1v2.1)
 * or the newest RFC 8017 (PKCS#1v2.2) using RSAPublicKey:
 *
 * RSAPublicKey ::= SEQUENCE {
 *     modulus           INTEGER,  -- n
 *     publicExponent    INTEGER   -- e
 * }
 *
 * or for ECDSA is specified in RFC 5480 itself as ECPoint ::= OCTET STRING
 *
 * For ECDSA, MCUboot passes the uncompressed format (i.e. 0x04 X Y), i.e. the
 * key encoding is parsed by MCUboot itself before being imported. For RSA, the
 * AlgorithmIdentifier is instead specified in RFC 3279 as the value of the OID
 * rsaEncryption: 1.2.840.113549.1.1.1, but MCUboot chooses in this case to
 * pass already the RSAPublicKey structure to the APIs, hence the code below just
 * understands the length of n by inspecting the fields of the ASN.1 encoding
 *
 */
psa_status_t psa_import_key(const psa_key_attributes_t *attributes,
                            const uint8_t *data,
                            size_t data_length,
                            psa_key_id_t *key)
{
#if defined(MCUBOOT_SIGN_RSA)
    /* This is either a 2048, 3072 or 4096 bit RSA key, hence the TLV must place
     * the length at index (6,7) with a leading 0x00. The leading 0x00 is due to
     * the fact that the MSB will always be set for RSA keys where the length is
     * a multiple of 8 bits.
     */
    const size_t bits =
        PSA_BYTES_TO_BITS((((uint16_t)data[6]) << 8) | (uint16_t)data[7]) - 8;

#elif defined(MCUBOOT_SIGN_EC256) || defined(MCUBOOT_SIGN_EC384)
    /* The public key is expected in uncompressed format, i.e. 0x04 X Y
     * for 256 or 384 bit lengths, and the driver wrappers expect to receive
     * it in that format
     */
    assert(data[0] == 0x04);
    const size_t bits = PSA_BYTES_TO_BITS((data_length - 1)/2);
#endif

    g_key_slot.buf = data;
    g_key_slot.len = data_length;

    memcpy(&g_key_slot.attr, attributes, sizeof(psa_key_attributes_t));
    g_key_slot.attr.bits = (psa_key_bits_t)bits;

    /* This signals that a new key has been imported */
    *key = (++g_key_slot.key_id);

    g_key_slot.is_valid = true;

    return PSA_SUCCESS;
}

psa_status_t psa_get_key_attributes(psa_key_id_t key,
                                    psa_key_attributes_t *attributes)
{
    assert(g_key_slot.is_valid && (g_key_slot.key_id == key));
    memcpy(attributes, &g_key_slot.attr, sizeof(psa_key_attributes_t));
    return PSA_SUCCESS;
}
#endif /* !MCUBOOT_BUILTIN_KEY */

psa_status_t psa_destroy_key(psa_key_id_t key)
{
#if !defined(MCUBOOT_BUILTIN_KEY)
    assert(g_key_slot.is_valid && (g_key_slot.key_id == key));

    g_key_slot.buf = NULL;
    g_key_slot.len = 0;
    g_key_slot.attr = psa_key_attributes_init();
    g_key_slot.is_valid = false;

    /* This will keep the value of the key_id so that a new import will
     * just use the next key_id. This allows to keep track of potential
     * clients trying to reuse a deleted key ID
     */
#else /* !MCUBOOT_BUILTIN_KEY */
    memset(g_pubkey_data, 0, sizeof(g_pubkey_data));
    g_key_slot.len = 0;
    g_key_slot.attr = psa_key_attributes_init();
#endif /* !MCUBOOT_BUILTIN_KEY */

    return PSA_SUCCESS;
}

/* Signature verification supports only RSA or ECDSA with P256 or P384 */
psa_status_t psa_verify_hash(psa_key_id_t key,
                             psa_algorithm_t alg,
                             const uint8_t *hash,
                             size_t hash_length,
                             const uint8_t *signature,
                             size_t signature_length)
{
    psa_status_t status;

#if !defined(MCUBOOT_BUILTIN_KEY)
    assert(g_key_slot.is_valid && (g_key_slot.key_id == key));
#else
    assert((key >= MCUBOOT_BUILTIN_KEY_ID_MIN) &&
           (key <= MCUBOOT_BUILTIN_KEY_ID_MAX));

    status = get_builtin_public_key(key, alg);
    if (status != PSA_SUCCESS) {
        return status;
    }
#endif /* !MCUBOOT_BUILTIN_KEY */

    status = psa_driver_wrapper_verify_hash(
                &g_key_slot.attr,
                g_key_slot.buf, g_key_slot.len,
                alg, hash, hash_length,
                signature, signature_length);

    return status;
}

psa_status_t mbedtls_to_psa_error(int ret)
{
    /* We don't require precise error translation */
    if (!ret) {
        return PSA_SUCCESS;
    } else {
        return PSA_ERROR_GENERIC_ERROR;
    }
}

psa_status_t psa_generate_random(uint8_t *output,
                                 size_t output_size)
{
#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)

    size_t output_length = 0;
    psa_status_t status = mbedtls_psa_external_get_random(g_ctx,
                                                          output, output_size,
                                                          &output_length);
    if (status != PSA_SUCCESS) {
        return status;
    }
    /* Breaking up a request into smaller chunks is currently not supported
     * for the external RNG interface.
     */
    if (output_length != output_size) {
        return PSA_ERROR_INSUFFICIENT_ENTROPY;
    }
    return PSA_SUCCESS;

#endif
    return PSA_ERROR_NOT_SUPPORTED;
}

/* This gets linked by the driver wrapper if no driver is present */
psa_status_t psa_verify_hash_builtin(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg, const uint8_t *hash, size_t hash_length,
    const uint8_t *signature, size_t signature_length)
{
#if defined(MCUBOOT_SIGN_RSA)
    if (PSA_KEY_TYPE_IS_RSA(psa_get_key_type(attributes))) {
        if (PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg) ||
            PSA_ALG_IS_RSA_PSS(alg)) {
#if defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PSS)
            return mbedtls_psa_rsa_verify_hash(
                attributes,
                key_buffer, key_buffer_size,
                alg, hash, hash_length,
                signature, signature_length);
#endif /* defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PSS) */
        } else {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
    }
#elif defined(MCUBOOT_SIGN_EC256) || defined(MCUBOOT_SIGN_EC384)
    if (PSA_KEY_TYPE_IS_ECC(psa_get_key_type(attributes))) {
        if (PSA_ALG_IS_ECDSA(alg)) {
#if defined(MBEDTLS_PSA_BUILTIN_ALG_ECDSA) || \
            defined(MBEDTLS_PSA_BUILTIN_ALG_DETERMINISTIC_ECDSA)
            return mbedtls_psa_ecdsa_verify_hash(
                attributes,
                key_buffer, key_buffer_size,
                alg, hash, hash_length,
                signature, signature_length);
#endif /* defined(MBEDTLS_PSA_BUILTIN_ALG_ECDSA) ||
        * defined(MBEDTLS_PSA_BUILTIN_ALG_DETERMINISTIC_ECDSA)
        */
        } else {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
    }
#endif

    (void) key_buffer;
    (void) key_buffer_size;
    (void) hash;
    (void) hash_length;
    (void) signature;
    (void) signature_length;

    return PSA_ERROR_NOT_SUPPORTED;
}

/* We don't need the full driver wrapper, we know the key is already a public key */
psa_status_t psa_driver_wrapper_export_public_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    uint8_t *data, size_t data_size, size_t *data_length)
{
    assert(PSA_KEY_TYPE_IS_PUBLIC_KEY(psa_get_key_type(attributes)));
    assert(key_buffer_size <= data_size);

    memcpy(data, key_buffer, key_buffer_size);
    *data_length = key_buffer_size;

    return PSA_SUCCESS;
}
