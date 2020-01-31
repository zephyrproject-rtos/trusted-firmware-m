/*
 *  t_cose_util.c
 *
 * Copyright 2019, Laurence Lundblade
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md.
 */

#include "t_cose_util.h"
#include "qcbor.h"
#include "t_cose_defines.h"
#include "t_cose_common.h"
#include "t_cose_crypto.h"


/**
 * \file t_cose_util.c
 *
 * \brief Implementation of t_cose utility functions.
 *
 * These are some functions common to signing and
 * verification.
 */


/*
 * Public function. See t_cose_util.h
 */
int32_t hash_alg_id_from_sig_alg_id(int32_t cose_sig_alg_id)
{
    /* If other hashes, particularly those that output bigger hashes
     * are added here, various other parts of this code have to be
     * changed to have larger buffers.
     */
    switch(cose_sig_alg_id) {
    case COSE_ALGORITHM_ES256:
        return COSE_ALG_SHA256_PROPRIETARY;

    default:
        return INT32_MAX;
    }
}


/*
 * Format of to-be-signed bytes used by create_tbs_hash().
 * This is defined in COSE (RFC 8152). It is the input
 * to the hash.
 *
 * Sig_structure = [
 *    context : "Signature" / "Signature1" / "CounterSignature",
 *    body_protected : empty_or_serialized_map,
 *    ? sign_protected : empty_or_serialized_map,
 *    external_aad : bstr,
 *    payload : bstr
 * ]
 *
 * body_protected refers to the protected headers from the
 * main COSE_Sign1 structure. This is a little hard to
 * to understand in the spec.
 *
 * sign_protected is not used with COSE_Sign1 since
 * there is no signer chunk.
 *
 * external_aad allows external data to be covered
 * by the hash, but is not supported by this implementation.
 */


/**
 * This is the size of the first part of the CBOR encoded TBS
 * bytes. It is around 30 bytes. See create_tbs_hash().
 */
#define T_COSE_SIZE_OF_TBS \
    1 + /* For opening the array */ \
    sizeof(COSE_SIG_CONTEXT_STRING_SIGNATURE1) + /* "Signature1" */ \
    2 + /* Overhead for encoding string */ \
    T_COSE_SIGN1_MAX_PROT_HEADER + /* entire protected headers */ \
    1 + /* Empty bstr for absent external_aad */ \
    9 /* The max CBOR length encoding for start of payload */


/*
 * Public function. See t_cose_util.h
 */
enum t_cose_err_t create_tbs_hash(int32_t cose_alg_id,
                                  struct q_useful_buf buffer_for_hash,
                                  struct q_useful_buf_c *hash,
                                  struct q_useful_buf_c protected_headers,
                                  enum t_cose_tbs_hash_mode_t payload_mode,
                                  struct q_useful_buf_c payload)
{
    /* approximate stack use on 32-bit machine:
     * local use: 210
     * total with calls: 250
     * Can be another 128 bytes or so depending on
     * t_cose_crypto_hash implementation. It sometimes
     * includes the full hashing context.
     */
    enum t_cose_err_t           return_value;
    QCBOREncodeContext          cbor_encode_ctx;
    UsefulBuf_MAKE_STACK_UB(    buffer_for_TBS_first_part, T_COSE_SIZE_OF_TBS);
    struct q_useful_buf_c       tbs_first_part;
    QCBORError                  qcbor_result;
    struct t_cose_crypto_hash   hash_ctx;
    int32_t                     hash_alg_id;
    size_t                      bytes_to_omit;

    /* This builds the CBOR-format to-be-signed bytes */
    QCBOREncode_Init(&cbor_encode_ctx, buffer_for_TBS_first_part);
    QCBOREncode_OpenArray(&cbor_encode_ctx);
    /* context */
    QCBOREncode_AddSZString(&cbor_encode_ctx,
                            COSE_SIG_CONTEXT_STRING_SIGNATURE1);
    /* body_protected */
    QCBOREncode_AddBytes(&cbor_encode_ctx,
                         protected_headers);
    /* sign_protected is not used for Sign1 */

    /* external_aad. There is none so an empty bstr */
    QCBOREncode_AddBytes(&cbor_encode_ctx, NULL_Q_USEFUL_BUF_C);

    /* The short fake payload. */
    if(payload_mode == T_COSE_TBS_PAYLOAD_IS_BSTR_WRAPPED) {
        /* Fake payload is just an empty bstr. It is here only
         * to make the array count right. It must be ommitted
         * in the actual hashing below
         */
        bytes_to_omit = 1;
        QCBOREncode_AddBytes(&cbor_encode_ctx, NULL_Q_USEFUL_BUF_C);
    } else {
        /* Fake payload is the type and length of the wrapping
         * bstr. It gets hashed with the first part, so no
         * bytes to omit.
         */
        bytes_to_omit = 0;
        QCBOREncode_AddBytesLenOnly(&cbor_encode_ctx, payload);
    }
    /* Cleverness only works because the payload is last in the array */

    /* Close of the array */
    QCBOREncode_CloseArray(&cbor_encode_ctx);

    /* get the encoded results, except for payload */
    qcbor_result = QCBOREncode_Finish(&cbor_encode_ctx, &tbs_first_part);
    if(qcbor_result) {
        /* Mainly means that the protected_headers were too big
         * (which should never happen) */
        return_value = T_COSE_ERR_SIG_STRUCT;
        goto Done;
    }

    /* Start the hashing */
    hash_alg_id = hash_alg_id_from_sig_alg_id(cose_alg_id);
    /* Don't check hash_alg_id for failure. t_cose_crypto_hash_start()
     * will handle it properly.
     */
    return_value = t_cose_crypto_hash_start(&hash_ctx, hash_alg_id);
    if(return_value) {
        goto Done;
    }

    /* This structure is hashed in two parts. The first part is
     * the CBOR-formatted array with protected headers and such.
     * The last part is the actual bytes of the payload. Doing it
     * this way avoids having to allocate a big buffer to hold
     * these two parts together.  It avoids having two copies of
     * the payload in the implementaiton as the payload as formatted
     * in the output buffer can be what is hashed. They payload
     * is the largest memory use, so this saves a lot.
     *
     * This is further complicated because the the payload
     * does have to be wrapped in a bstr. It is done one way
     * when signing and another when verifying.
     */

    /* This is hashing of the first part, all the CBOR
     * except the payload.
     */
    t_cose_crypto_hash_update(&hash_ctx,
                              q_useful_buf_head(tbs_first_part,
                                        tbs_first_part.len - bytes_to_omit));

    /* Hash the payload, the second part. This may or may not
     * have the bstr wrapping. If not, it was hashed above.
     */
    t_cose_crypto_hash_update(&hash_ctx, payload);

    /* Finish the hash and set up to return it */
    return_value = t_cose_crypto_hash_finish(&hash_ctx,
                                             buffer_for_hash,
                                             hash);
Done:
    return return_value;
}


#ifdef INCLUDE_TEST_CODE_AND_KEY_ID /* Remove them from release build */
/*
 * Public function. See t_cose_util.h
 */
enum t_cose_err_t
get_short_circuit_kid(struct q_useful_buf buffer_for_kid,
                      struct q_useful_buf_c *kid)
{
    /* This is a random hard coded key ID that is used to indicate
     * short-circuit signing. It is OK to hard code this as the
     * probability of collision with this ID is very low and the same
     * as for collision between any two key IDs of any sort.
     */
    uint8_t defined_short_circuit_kid[] = {
        0xef, 0x95, 0x4b, 0x4b, 0xd9, 0xbd, 0xf6, 0x70,
        0xd0, 0x33, 0x60, 0x82, 0xf5, 0xef, 0x15, 0x2a,
        0xf8, 0xf3, 0x5b, 0x6a, 0x6c, 0x00, 0xef, 0xa6,
        0xa9, 0xa7, 0x1f, 0x49, 0x51, 0x7e, 0x18, 0xc6};

    /* Prevent a dumb error where the size constant in the header is
     * wrong.This check will be evaluated at compile time and optimize
     * out when all is correct.
     */
    if(sizeof(defined_short_circuit_kid) != T_COSE_SHORT_CIRCUIT_KID_SIZE) {
        return T_COSE_ERR_BAD_SHORT_CIRCUIT_KID;
    }

    *kid = q_useful_buf_copy(buffer_for_kid,
                             Q_USEFUL_BUF_FROM_BYTE_ARRAY_LITERAL(
                                 defined_short_circuit_kid));

    return q_useful_buf_c_is_null(*kid) ?
              T_COSE_ERR_KEY_BUFFER_SIZE :
              T_COSE_SUCCESS;
}
#endif /* INCLUDE_TEST_CODE_AND_KEY_ID */
