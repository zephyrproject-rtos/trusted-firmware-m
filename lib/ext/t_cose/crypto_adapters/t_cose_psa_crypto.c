/*
 * t_cose_psa_crypto.c
 *
 * Copyright 2019-2023, Laurence Lundblade
 * Copyright (c) 2020-2021, Arm Limited. All rights reserved
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */


/**
 * \file t_cose_psa_crypto.c
 *
 * \brief Crypto Adaptation for t_cose to use ARM's PSA Crypto APIs.
 *
 * This connects up the abstract interface in t_cose_crypto.h to the
 * implementations of ECDSA signing, hashing and HMAC in ARM's PSA crypto
 * library.
 *
 * This adapter layer doesn't bloat the implementation as everything
 * here had to be done anyway -- the mapping of algorithm IDs, the
 * data format rearranging, the error code translation.
 *
 * This code should just work out of the box if compiled and linked
 * against ARM's PSA crypto. No preprocessor #defines are needed.
 *
 * You can disable SHA-384 and SHA-512 to save code and space by
 * defining T_COSE_DISABLE_ES384 or T_COSE_DISABLE_ES512. This saving
 * is most in stack space in the main t_cose implementation. (It seems
 * likely that changes to PSA itself would be needed to remove the
 * SHA-384 and SHA-512 implementations to save that code. Lack of
 * reference and dead stripping the executable won't do it).
 */


#include "t_cose_crypto.h"  /* The interface this implements */
#include "psa/crypto.h"     /* PSA Crypto Interface to mbed crypto or such */


/* Here's the auto-detect and manual override logic for managing PSA
 * Crypto API compatibility.
 *
 * PSA_GENERATOR_UNBRIDLED_CAPACITY happens to be defined in MBed
 * Crypto 1.1 and not in MBed Crypto 2.0 so it is what auto-detect
 * hinges off of.
 *
 * T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO20 can be defined to force
 * setting to MBed Crypto 2.0
 *
 * T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11 can be defined to force
 * setting to MBed Crypt 1.1. It is also what the code below hinges
 * on.
 */
#if defined(PSA_GENERATOR_UNBRIDLED_CAPACITY) && !defined(T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO20)
#define T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11
#endif


/* Avoid compiler warning due to unused argument */
#define ARG_UNUSED(arg) (void)(arg)

#ifndef T_COSE_DISABLE_SIGN1
/**
 * \brief Map a COSE signing algorithm ID to a PSA signing algorithm ID
 *
 * \param[in] cose_alg_id  The COSE algorithm ID.
 *
 * \return The PSA algorithm ID or 0 if this doesn't map the COSE ID.
 */
static psa_algorithm_t cose_alg_id_to_psa_alg_id(int32_t cose_alg_id)
{
    /* The #ifdefs save a little code when algorithms are disabled */

    return cose_alg_id == COSE_ALGORITHM_ES256 ? PSA_ALG_ECDSA(PSA_ALG_SHA_256) :
#ifndef T_COSE_DISABLE_ES384
           cose_alg_id == COSE_ALGORITHM_ES384 ? PSA_ALG_ECDSA(PSA_ALG_SHA_384) :
#endif
#ifndef T_COSE_DISABLE_ES512
           cose_alg_id == COSE_ALGORITHM_ES512 ? PSA_ALG_ECDSA(PSA_ALG_SHA_512) :
#endif
                                                 0;
    /* psa/crypto_values.h doesn't seem to define a "no alg" value,
     * but zero seems OK for that use in the ECDSA context. */
}


/**
 * \brief Map a PSA error into a t_cose error for signing.
 *
 * \param[in] err   The PSA status.
 *
 * \return The \ref t_cose_err_t.
 */
static enum t_cose_err_t psa_status_to_t_cose_error_signing(psa_status_t err)
{
    /* Intentionally keeping to fewer mapped errors to save object code */
    return (err == PSA_SUCCESS  )                 ? T_COSE_SUCCESS :
           (err == PSA_ERROR_INVALID_SIGNATURE)   ? T_COSE_ERR_SIG_VERIFY :
           (err == PSA_ERROR_NOT_SUPPORTED)       ? T_COSE_ERR_UNSUPPORTED_SIGNING_ALG:
           (err == PSA_ERROR_INSUFFICIENT_MEMORY) ? T_COSE_ERR_INSUFFICIENT_MEMORY :
           (err == PSA_ERROR_CORRUPTION_DETECTED) ? T_COSE_ERR_TAMPERING_DETECTED :
                                                  T_COSE_ERR_SIG_FAIL;
}


/*
 * See documentation in t_cose_crypto.h
 */
enum t_cose_err_t
t_cose_crypto_pub_key_verify(int32_t               cose_algorithm_id,
                             struct t_cose_key     verification_key,
                             struct q_useful_buf_c kid,
                             struct q_useful_buf_c hash_to_verify,
                             struct q_useful_buf_c signature)
{
    psa_algorithm_t   psa_alg_id;
    psa_status_t      psa_result;
    enum t_cose_err_t return_value;
    psa_key_handle_t  verification_key_psa;

    /* This implementation does no look up keys by kid in the key
     * store */
    ARG_UNUSED(kid);

    /* Convert to PSA algorithm ID scheme */
    psa_alg_id = cose_alg_id_to_psa_alg_id(cose_algorithm_id);

    /* This implementation supports ECDSA and only ECDSA. The
     * interface allows it to support other, but none are implemented.
     * This implementation works for different keys lengths and
     * curves. That is the curve and key length as associated with the
     * signing_key passed in, not the cose_algorithm_id This check
     * looks for ECDSA signing as indicated by COSE and rejects what
     * is not. (Perhaps this check can be removed to save object code
     * if it is the case that psa_verify_hash() does the right
     * checks).
     */
    if(!PSA_ALG_IS_ECDSA(psa_alg_id)) {
        return_value = T_COSE_ERR_UNSUPPORTED_SIGNING_ALG;
        goto Done;
    }

    verification_key_psa = (psa_key_handle_t)verification_key.k.key_handle;

    psa_result = psa_verify_hash(verification_key_psa,
                                 psa_alg_id,
                                 hash_to_verify.ptr,
                                 hash_to_verify.len,
                                 signature.ptr,
                                 signature.len);

    return_value = psa_status_to_t_cose_error_signing(psa_result);

  Done:
    return return_value;
}


/*
 * See documentation in t_cose_crypto.h
 */
enum t_cose_err_t
t_cose_crypto_pub_key_sign(int32_t                cose_algorithm_id,
                           struct t_cose_key      signing_key,
                           struct q_useful_buf_c  hash_to_sign,
                           struct q_useful_buf    signature_buffer,
                           struct q_useful_buf_c *signature)
{
    enum t_cose_err_t return_value;
    psa_status_t      psa_result;
    psa_algorithm_t   psa_alg_id;
    psa_key_handle_t  signing_key_psa;
    size_t            signature_len;

    psa_alg_id = cose_alg_id_to_psa_alg_id(cose_algorithm_id);

    /* This implementation supports ECDSA and only ECDSA. The
     * interface allows it to support other, but none are implemented.
     * This implementation works for different keys lengths and
     * curves. That is the curve and key length as associated with the
     * signing_key passed in, not the cose_algorithm_id This check
     * looks for ECDSA signing as indicated by COSE and rejects what
     * is not. (Perhaps this check can be removed to save object code
     * if it is the case that psa_verify_hash() does the right
     * checks).
     */
    if(!PSA_ALG_IS_ECDSA(psa_alg_id)) {
        return_value = T_COSE_ERR_UNSUPPORTED_SIGNING_ALG;
        goto Done;
    }

    signing_key_psa = (psa_key_handle_t)signing_key.k.key_handle;

    /* It is assumed that this call is checking the signature_buffer
     * length and won't write off the end of it.
     */
    psa_result = psa_sign_hash(signing_key_psa,
                               psa_alg_id,
                               hash_to_sign.ptr,
                               hash_to_sign.len,
                               signature_buffer.ptr, /* Sig buf */
                               signature_buffer.len, /* Sig buf size */
                               &signature_len);      /* Sig length */

    return_value = psa_status_to_t_cose_error_signing(psa_result);

    if(return_value == T_COSE_SUCCESS) {
        /* Success, fill in the return useful_buf */
        signature->ptr = signature_buffer.ptr;
        signature->len = signature_len;
    }

  Done:
     return return_value;
}


/*
 * See documentation in t_cose_crypto.h
 */
enum t_cose_err_t t_cose_crypto_sig_size(int32_t           cose_algorithm_id,
                                         struct t_cose_key signing_key,
                                         size_t           *sig_size)
{
    enum t_cose_err_t return_value;
    psa_key_handle_t  signing_key_psa;
    size_t            key_len_bits;
    size_t            key_len_bytes;

    /* If desperate to save code, this can return the constant
     * T_COSE_MAX_SIG_SIZE instead of doing an exact calculation.  The
     * buffer size calculation will return too large of a value and
     * waste a little heap / stack, but everything will still work
     * (except the tests that test for exact values will fail). This
     * will save 100 bytes or so of obejct code.
     */

    if(!t_cose_algorithm_is_ecdsa(cose_algorithm_id)) {
        return_value = T_COSE_ERR_UNSUPPORTED_SIGNING_ALG;
        goto Done;
    }

    signing_key_psa = (psa_key_handle_t)signing_key.k.key_handle;

#ifdef T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11
    /* This code is for MBed Crypto 1.1. It uses an older version of
     * the PSA Crypto API that is not compatible with the new
     * versions. When all environments (particularly TF-M) are on the
     * latest API, this code will no longer be necessary.
     */

    psa_key_type_t    key_type;

    psa_status_t status = psa_get_key_information(signing_key_psa,
                                                  &key_type,
                                                  &key_len_bits);

    (void)key_type; /* Avoid unused parameter error */

#else /* T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11 */
    /* This code is for Mbed Crypto 2.0 circa 2019. The PSA Crypto API
     * is supposed to be offically locked down in 2020 and should be
     * very close to this, so this is likely the code to use with MBed
     * Crypto going forward.
     */

    psa_key_attributes_t key_attributes = psa_key_attributes_init();

    psa_status_t status = psa_get_key_attributes(signing_key_psa, &key_attributes);

    key_len_bits = psa_get_key_bits(&key_attributes);

#endif /* T_COSE_USE_PSA_CRYPTO_FROM_MBED_CRYPTO11 */

    return_value = psa_status_to_t_cose_error_signing(status);
    if(return_value == T_COSE_SUCCESS) {
        /* Calculation of size per RFC 8152 section 8.1 -- round up to
         * number of bytes. */
        key_len_bytes = key_len_bits / 8;
        if(key_len_bits % 8) {
            key_len_bytes++;
        }
        /* Double because signature is made of up r and s values */
        *sig_size = key_len_bytes * 2;
    }

    return_value = T_COSE_SUCCESS;
Done:
    return return_value;
}
#endif /* !T_COSE_DISABLE_SIGN1 */




#if !defined(T_COSE_DISABLE_SHORT_CIRCUIT_SIGN) || \
    !defined(T_COSE_DISABLE_SIGN1)
/**
 * \brief Convert COSE hash algorithm ID to a PSA hash algorithm ID
 *
 * \param[in] cose_hash_alg_id   The COSE-based ID for the
 *
 * \return PSA-based hash algorithm ID, or USHRT_MAX on error.
 *
 */
static inline psa_algorithm_t
cose_hash_alg_id_to_psa(int32_t cose_hash_alg_id)
{
    return cose_hash_alg_id == COSE_ALGORITHM_SHA_256 ? PSA_ALG_SHA_256 :
#ifndef T_COSE_DISABLE_ES384
           cose_hash_alg_id == COSE_ALGORITHM_SHA_384 ? PSA_ALG_SHA_384 :
#endif
#ifndef T_COSE_DISABLE_ES512
           cose_hash_alg_id == COSE_ALGORITHM_SHA_512 ? PSA_ALG_SHA_512 :
#endif
                                                        UINT16_MAX;
}


/**
 * \brief Map a PSA error into a t_cose error for hashes.
 *
 * \param[in] status   The PSA status.
 *
 * \return The \ref t_cose_err_t.
 */
static enum t_cose_err_t
psa_status_to_t_cose_error_hash(psa_status_t status)
{
    /* Intentionally limited to just this minimum set of errors to
     * save object code as hashes don't really fail much
     */
    return (status == PSA_SUCCESS)                ? T_COSE_SUCCESS :
           (status == PSA_ERROR_NOT_SUPPORTED)    ? T_COSE_ERR_UNSUPPORTED_HASH :
           (status == PSA_ERROR_BUFFER_TOO_SMALL) ? T_COSE_ERR_HASH_BUFFER_SIZE :
                                                  T_COSE_ERR_HASH_GENERAL_FAIL;
}


/*
 * See documentation in t_cose_crypto.h
 */
enum t_cose_err_t t_cose_crypto_hash_start(struct t_cose_crypto_hash *hash_ctx,
                                           int32_t cose_hash_alg_id)
{
    psa_algorithm_t      psa_alg;

    /* Map the algorithm ID */
    psa_alg = cose_hash_alg_id_to_psa(cose_hash_alg_id);

    /* initialize PSA hash context */
    hash_ctx->ctx = psa_hash_operation_init();

    /* Actually do the hash set up */
    hash_ctx->status = psa_hash_setup(&(hash_ctx->ctx), psa_alg);

    /* Map errors and return */
    return psa_status_to_t_cose_error_hash((psa_status_t)hash_ctx->status);
}


/*
 * See documentation in t_cose_crypto.h
 */
void t_cose_crypto_hash_update(struct t_cose_crypto_hash *hash_ctx,
                               struct q_useful_buf_c      data_to_hash)
{
    if(hash_ctx->status != PSA_SUCCESS) {
        /* In error state. Nothing to do. */
        return;
    }

    if(data_to_hash.ptr == NULL) {
        /* This allows for NULL buffers to be passed in all the way at
         * the top of signer or message creator when all that is
         * happening is the size of the result is being computed.
         */
        return;
    }

    /* Actually hash the data */
    hash_ctx->status = psa_hash_update(&(hash_ctx->ctx),
                                       data_to_hash.ptr,
                                       data_to_hash.len);
}


/*
 * See documentation in t_cose_crypto.h
 */
enum t_cose_err_t
t_cose_crypto_hash_finish(struct t_cose_crypto_hash *hash_ctx,
                          struct q_useful_buf        buffer_to_hold_result,
                          struct q_useful_buf_c     *hash_result)
{
    if(hash_ctx->status != PSA_SUCCESS) {
        /* Error state. Nothing to do */
        goto Done;
    }

    /* Actually finish up the hash */
    hash_ctx->status = psa_hash_finish(&(hash_ctx->ctx),
                                         buffer_to_hold_result.ptr,
                                         buffer_to_hold_result.len,
                                       &(hash_result->len));

    hash_result->ptr = buffer_to_hold_result.ptr;

Done:
    return psa_status_to_t_cose_error_hash(hash_ctx->status);
}


/*
 * See documentation in t_cose_crypto.h
 */
enum t_cose_err_t
t_cose_crypto_import_ec2_pubkey(int32_t               cose_ec_curve_id,
                                struct q_useful_buf_c x_coord,
                                struct q_useful_buf_c y_coord,
                                bool                  y_bool,
                                struct t_cose_key    *key_handle)
{
    psa_status_t          status;
    psa_key_attributes_t  attributes;
    psa_key_type_t        type_public;
    psa_algorithm_t       alg;
    struct q_useful_buf_c  import;
    // TODO: really make sure this size is right for the curve types supported
    UsefulOutBuf_MakeOnStack (import_form, T_COSE_EXPORT_PUBLIC_KEY_MAX_SIZE + 5);

    switch (cose_ec_curve_id) {
    case COSE_ELLIPTIC_CURVE_P_256:
         type_public  = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1);
         alg = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
         break;
    case COSE_ELLIPTIC_CURVE_P_384:
         type_public  = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1);
         alg = PSA_ALG_ECDSA(PSA_ALG_SHA_384);
         break;
    case COSE_ELLIPTIC_CURVE_P_521:
         type_public  = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1);
         alg = PSA_ALG_ECDSA(PSA_ALG_SHA_512);
         break;

    default:
         return T_COSE_ERR_UNSUPPORTED_SIGNING_ALG;
    }

    // TODO: These attributes only suites to ECDSA(...) operations
    attributes = psa_key_attributes_init();
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, type_public);

    /* This converts to a serialized representation of an EC Point
     * described in
     * Certicom Research, "SEC 1: Elliptic Curve Cryptography", Standards for
     * Efficient Cryptography, May 2009, <https://www.secg.org/sec1-v2.pdf>.
     * The description is very mathematical and hard to read for us
     * coder types. It was much easier to understand reading Jim's
     * COSE-C implementation. See mbedtls_ecp_keypair() in COSE-C.
     *
     * This string is the format used by Mbed TLS to import an EC
     * public key.
     *
     * This does implement point compression. The patents for it have
     * run out so it's OK to implement. Point compression is commented
     * out in Jim's implementation, presumably because of the paten
     * issue.
     *
     * A simple English description of the format is this. The first
     * byte is 0x04 for no point compression and 0x02 or 0x03 if there
     * is point compression. 0x02 indicates a positive y and 0x03 a
     * negative y (or is the other way). Following the first byte
     * are the octets of x. If the first byte is 0x04 then following
     * x is the y value.
     *
     * UsefulOutBut is used to safely construct this string.
     */
    uint8_t first_byte;
    if(q_useful_buf_c_is_null(y_coord)) {
        /* This is point compression */
        first_byte = y_bool ? 0x03 : 0x02;
    } else {
        first_byte = 0x04;
    }

    // TODO: is padding of x necessary? Jim's code goes to
    // a lot of trouble to look up the group and get the length.

    UsefulOutBuf_AppendByte(&import_form, first_byte);
    UsefulOutBuf_AppendUsefulBuf(&import_form, x_coord);
    if(first_byte == 0x04) {
        UsefulOutBuf_AppendUsefulBuf(&import_form, y_coord);
    }
    import = UsefulOutBuf_OutUBuf(&import_form);


    status = psa_import_key(&attributes,
                            import.ptr, import.len,
                            (psa_key_handle_t *)(&key_handle->k.key_handle));

    if (status != PSA_SUCCESS) {
        return T_COSE_ERR_FAIL;
    }

    return T_COSE_SUCCESS;
}


enum t_cose_err_t
t_cose_crypto_export_ec2_key(struct t_cose_key      key_handle,
                             int32_t               *curve,
                             struct q_useful_buf    x_coord_buf,
                             struct q_useful_buf_c *x_coord,
                             struct q_useful_buf    y_coord_buf,
                             struct q_useful_buf_c *y_coord,
                             bool                  *y_bool)
{
    psa_status_t          psa_status;
    uint8_t               export_buf[T_COSE_EXPORT_PUBLIC_KEY_MAX_SIZE];
    size_t                export_len;
    struct q_useful_buf_c export;
    size_t                len;
    uint8_t               first_byte;
    psa_key_attributes_t  attributes;

    /* Export public key */
    psa_status = psa_export_public_key((psa_key_handle_t)key_handle.k.key_handle, /* in: Key handle     */
                                        export_buf,     /* in: PK buffer      */
                                        sizeof(export_buf),     /* in: PK buffer size */
                                       &export_len);           /* out: Result length */
    if(psa_status != PSA_SUCCESS) {
        return T_COSE_ERR_FAIL; // TODO: error code
    }
    first_byte = export_buf[0];
    export = (struct q_useful_buf_c){export_buf+1, export_len-1};

    /* export_buf is one first byte, the x-coord and maybe the y-coord
     * per SEC1.
     */

    attributes = psa_key_attributes_init();
    psa_status = psa_get_key_attributes((psa_key_handle_t)key_handle.k.key_handle,
                                        &attributes);
    if(PSA_KEY_TYPE_ECC_GET_FAMILY(psa_get_key_type(&attributes)) != PSA_ECC_FAMILY_SECP_R1) {
        return T_COSE_ERR_FAIL;
    }

    switch(psa_get_key_bits(&attributes)) {
    case 256:
        *curve = COSE_ELLIPTIC_CURVE_P_256;
        break;
    case 384:
        *curve = COSE_ELLIPTIC_CURVE_P_384;
        break;
    case 521:
        *curve = COSE_ELLIPTIC_CURVE_P_521;
        break;
    default:
        return T_COSE_ERR_FAIL;
    }

    switch(first_byte) {
        case 0x04:
            len = (export_len - 1 ) / 2;
            *y_coord = UsefulBuf_Copy(y_coord_buf, UsefulBuf_Tail(export, len));
            break;

        case 0x02:
            len = export_len - 1;
            *y_coord = NULL_Q_USEFUL_BUF_C;
            *y_bool = true;
            break;

        case 0x03:
            len = export_len - 1;
            *y_coord = NULL_Q_USEFUL_BUF_C;
            *y_bool = false;
            break;

        default:
            return T_COSE_ERR_FAIL;
    }

    *x_coord = UsefulBuf_Copy(x_coord_buf, UsefulBuf_Head(export, len));

    if ( q_useful_buf_c_is_null(*x_coord) ||
        (q_useful_buf_c_is_null(*y_coord) && first_byte == 0x04)) {
        return T_COSE_ERR_TOO_SMALL;
    }

    return T_COSE_SUCCESS;
}

#endif /* !T_COSE_DISABLE_SHORT_CIRCUIT_SIGN || !T_COSE_DISABLE_SIGN1 */

#ifndef T_COSE_DISABLE_MAC0
/**
 * \brief Convert COSE algorithm ID to a PSA HMAC algorithm ID
 *
 * \param[in] cose_hmac_alg_id   The COSE-based ID for the
 *
 * \return PSA-based MAC algorithm ID, or a vendor flag in the case of error.
 *
 */
static inline psa_algorithm_t cose_hmac_alg_id_to_psa(int32_t cose_hmac_alg_id)
{
    switch(cose_hmac_alg_id) {
    case T_COSE_ALGORITHM_HMAC256:
        return PSA_ALG_HMAC(PSA_ALG_SHA_256);
    case T_COSE_ALGORITHM_HMAC384:
        return PSA_ALG_HMAC(PSA_ALG_SHA_384);
    case T_COSE_ALGORITHM_HMAC512:
        return PSA_ALG_HMAC(PSA_ALG_SHA_512);
    default:
        return PSA_ALG_VENDOR_FLAG;
    }
}

/**
 * \brief Map a PSA error into a t_cose error for HMAC.
 *
 * \param[in] status   The PSA status.
 *
 * \return The \ref t_cose_err_t.
 */
static enum t_cose_err_t
psa_status_to_t_cose_error_hmac(psa_status_t status)
{
    /* Intentionally limited to just this minimum set of errors to
     * save object code as hashes don't really fail much
     */
    return (status == PSA_SUCCESS)                   ? T_COSE_SUCCESS :
           (status == PSA_ERROR_NOT_SUPPORTED)       ? T_COSE_ERR_UNSUPPORTED_HASH :
           (status == PSA_ERROR_INVALID_ARGUMENT)    ? T_COSE_ERR_INVALID_ARGUMENT :
           (status == PSA_ERROR_INSUFFICIENT_MEMORY) ? T_COSE_ERR_INSUFFICIENT_MEMORY :
           (status == PSA_ERROR_BUFFER_TOO_SMALL)    ? T_COSE_ERR_TOO_SMALL :
           (status == PSA_ERROR_INVALID_SIGNATURE)   ? T_COSE_ERR_SIG_VERIFY :
                                                     T_COSE_ERR_FAIL;
}

/*
 * See documentation in t_cose_crypto.h
 */
enum t_cose_err_t
t_cose_crypto_hmac_sign_setup(struct t_cose_crypto_hmac *hmac_ctx,
                              struct t_cose_key          signing_key,
                              const int32_t              cose_alg_id)
{
    psa_algorithm_t psa_alg;
    psa_status_t psa_ret;

    if(!hmac_ctx) {
        return T_COSE_ERR_INVALID_ARGUMENT;
    }

    /* Map the algorithm ID */
    psa_alg = cose_hmac_alg_id_to_psa(cose_alg_id);
    if(!PSA_ALG_IS_MAC(psa_alg)) {
        return T_COSE_ERR_UNSUPPORTED_SIGNING_ALG;
    }

    /*
     * Verify if HMAC algorithm is valid.
     * According to COSE (RFC 8152), only SHA-256, SHA-384 and SHA-512 are
     * supported in COSE_Mac0 with HMAC.
     */
    if((psa_alg != PSA_ALG_HMAC(PSA_ALG_SHA_256)) && \
       (psa_alg != PSA_ALG_HMAC(PSA_ALG_SHA_384)) && \
       (psa_alg != PSA_ALG_HMAC(PSA_ALG_SHA_512))) {
        return T_COSE_ERR_UNSUPPORTED_SIGNING_ALG;
    }

    hmac_ctx->op_ctx = psa_mac_operation_init();

    psa_ret = psa_mac_sign_setup(&hmac_ctx->op_ctx,
                                 (psa_key_handle_t)signing_key.k.key_handle,
                                 psa_alg);

    return psa_status_to_t_cose_error_hmac(psa_ret);
}

/*
 * See documentation in t_cose_crypto.h
 */
enum t_cose_err_t
t_cose_crypto_hmac_update(struct t_cose_crypto_hmac *hmac_ctx,
                          struct q_useful_buf_c      payload)
{
    psa_status_t psa_ret;

    if(!hmac_ctx) {
        return T_COSE_ERR_INVALID_ARGUMENT;
    }

    psa_ret = psa_mac_update(&hmac_ctx->op_ctx,
                             payload.ptr, payload.len);

    return psa_status_to_t_cose_error_hmac(psa_ret);
}

/*
 * See documentation in t_cose_crypto.h
 */
enum t_cose_err_t
t_cose_crypto_hmac_sign_finish(struct t_cose_crypto_hmac *hmac_ctx,
                               struct q_useful_buf        tag_buf,
                               struct q_useful_buf_c     *tag)
{
    psa_status_t psa_ret;

    if(!hmac_ctx) {
        return T_COSE_ERR_INVALID_ARGUMENT;
    }

    psa_ret = psa_mac_sign_finish(&hmac_ctx->op_ctx,
                                  tag_buf.ptr, tag_buf.len,
                                  &(tag->len));
    if(psa_ret == PSA_SUCCESS) {
        tag->ptr = tag_buf.ptr;
    }

    return psa_status_to_t_cose_error_hmac(psa_ret);
}

/*
 * See documentation in t_cose_crypto.h
 */
enum t_cose_err_t
t_cose_crypto_hmac_verify_setup(struct t_cose_crypto_hmac *hmac_ctx,
                                const int                  cose_alg_id,
                                struct t_cose_key          verify_key)
{
    psa_algorithm_t psa_alg;
    psa_status_t psa_ret;

    if(!hmac_ctx) {
        return T_COSE_ERR_INVALID_ARGUMENT;
    }

    /* Map the algorithm ID */
    psa_alg = cose_hmac_alg_id_to_psa(cose_alg_id);
    if(!PSA_ALG_IS_MAC(psa_alg)) {
        return T_COSE_ERR_UNSUPPORTED_SIGNING_ALG;
    }

    /*
     * Verify if HMAC algorithm is valid.
     * According to COSE (RFC 8152), only SHA-256, SHA-384 and SHA-512 are
     * supported in HMAC.
     */
    if((psa_alg != PSA_ALG_HMAC(PSA_ALG_SHA_256)) && \
        (psa_alg != PSA_ALG_HMAC(PSA_ALG_SHA_384)) && \
        (psa_alg != PSA_ALG_HMAC(PSA_ALG_SHA_512))) {
        return T_COSE_ERR_UNSUPPORTED_SIGNING_ALG;
    }

    hmac_ctx->op_ctx = psa_mac_operation_init();

    psa_ret = psa_mac_verify_setup(&hmac_ctx->op_ctx,
                                   (psa_key_handle_t)verify_key.k.key_handle,
                                   psa_alg);

    return psa_status_to_t_cose_error_hmac(psa_ret);
}

/*
 * See documentation in t_cose_crypto.h
 */
enum t_cose_err_t
t_cose_crypto_hmac_verify_finish(struct t_cose_crypto_hmac *hmac_ctx,
                                 struct q_useful_buf_c      tag)
{
    psa_status_t psa_ret;

    if(!hmac_ctx) {
        return T_COSE_ERR_INVALID_ARGUMENT;
    }

    psa_ret = psa_mac_verify_finish(&hmac_ctx->op_ctx, tag.ptr, tag.len);

    return psa_status_to_t_cose_error_hmac(psa_ret);
}
#endif /* !T_COSE_DISABLE_MAC0 */
