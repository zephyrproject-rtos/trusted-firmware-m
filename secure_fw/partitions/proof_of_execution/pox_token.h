/*
 * pox_token.h
 *
 * Copyright (c) 2018-2019, Laurence Lundblade.
 *
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */

#ifndef __POX_TOKEN_H__
#define __POX_TOKEN_H__

#include <stdint.h>
#include "qcbor/qcbor.h"
#include "t_cose/t_cose_mac_compute.h"
#include "t_cose/t_cose_sign1_sign.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Error codes returned from PoX token creation.
     */
    enum pox_token_err_t
    {
        POX_TOKEN_ERR_SUCCESS = 0,          /**< Success */
        POX_TOKEN_ERR_TOO_SMALL,            /**< Output buffer too small */
        POX_TOKEN_ERR_CBOR_FORMATTING,      /**< CBOR formatting error */
        POX_TOKEN_ERR_GENERAL,              /**< General error */
        POX_TOKEN_ERR_HASH_UNAVAILABLE,     /**< Required hash function unavailable */
        POX_TOKEN_ERR_CBOR_NOT_WELL_FORMED, /**< CBOR syntax error */
        POX_TOKEN_ERR_CBOR_STRUCTURE,       /**< Incorrect CBOR structure */
        POX_TOKEN_ERR_CBOR_TYPE,            /**< Unexpected CBOR type */
        POX_TOKEN_ERR_INTEGER_VALUE,        /**< Integer value too large */
        POX_TOKEN_ERR_COSE_FORMAT,          /**< Invalid COSE message structure */
        POX_TOKEN_ERR_COSE_VALIDATION,      /**< Invalid COSE signature/authentication tag */
        POX_TOKEN_ERR_UNSUPPORTED_SIG_ALG,  /**< Unsupported signing algorithm */
        POX_TOKEN_ERR_INSUFFICIENT_MEMORY,  /**< Insufficient memory */
        POX_TOKEN_ERR_TAMPERING_DETECTED,   /**< Cryptographic tampering detected */
        POX_TOKEN_ERR_SIGNING_KEY,          /**< Signing key error */
        POX_TOKEN_ERR_VERIFICATION_KEY,     /**< Verification key error */
        POX_TOKEN_ERR_NO_VALID_TOKEN,       /**< No valid token present */
        POX_TOKEN_ERR_NOT_FOUND,            /**< Data item not found */
        POX_TOKEN_ERR_SW_COMPONENTS_MISSING /**< Software component absence error */
    };

    /**
     * @brief PoX token encoding context.
     */
    struct pox_token_encode_ctx
    {
        QCBOREncodeContext cbor_enc_ctx; /**< CBOR encoding context */
        int32_t key_select;              /**< Key selection for signing */
#ifdef SYMMETRIC_PROOF_OF_EXECUTION
        struct t_cose_mac_calculate_ctx mac_ctx; /**< MAC calculation context */
#else
    struct t_cose_sign1_sign_ctx signer_ctx; /**< Signing context */
#endif
    };

    /**
     * @brief Initializes a PoX token encoding context.
     *
     * @param[in] me         Pointer to the encoding context.
     * @param[in] key_select Key selection for signing.
     * @param[in] cose_alg_id Signing algorithm ID.
     * @param[out] out_buf   Output buffer for encoded token.
     *
     * @return One of the @ref pox_token_err_t errors.
     */
    enum pox_token_err_t
    pox_token_encode_start(struct pox_token_encode_ctx *me,
                           int32_t key_select,
                           int32_t cose_alg_id,
                           const struct q_useful_buf *out_buf);

    /**
     * @brief Borrows the CBOR encoding context for direct encoding.
     *
     * @param[in] me Pointer to the encoding context.
     *
     * @return Pointer to the CBOR encoding context.
     */
    QCBOREncodeContext *
    pox_token_encode_borrow_cbor_cntxt(struct pox_token_encode_ctx *me);

    /**
     * @brief Adds a 64-bit integer claim to the PoX token.
     *
     * @param[in] me    Pointer to the encoding context.
     * @param[in] label Claim label.
     * @param[in] value Integer claim value.
     */
    void pox_token_encode_add_integer(struct pox_token_encode_ctx *me,
                                      int32_t label,
                                      int64_t value);

    /**
     * @brief Adds a binary string claim to the PoX token.
     *
     * @param[in] me    Pointer to the encoding context.
     * @param[in] label Claim label.
     * @param[in] bstr  Binary string claim data.
     */
    void pox_token_encode_add_bstr(struct pox_token_encode_ctx *me,
                                   int32_t label,
                                   const struct q_useful_buf_c *bstr);

    /**
     * @brief Adds a text string claim to the PoX token.
     *
     * @param[in] me    Pointer to the encoding context.
     * @param[in] label Claim label.
     * @param[in] tstr  Text string claim data.
     */
    void pox_token_encode_add_tstr(struct pox_token_encode_ctx *me,
                                   int32_t label,
                                   const struct q_useful_buf_c *tstr);

    /**
     * @brief Adds pre-encoded CBOR data as a claim to the PoX token.
     *
     * @param[in] me      Pointer to the encoding context.
     * @param[in] label   Claim label.
     * @param[in] encoded Encoded CBOR data.
     */
    void pox_token_encode_add_cbor(struct pox_token_encode_ctx *me,
                                   int32_t label,
                                   const struct q_useful_buf_c *encoded);

    /**
     * @brief Completes the PoX token encoding process.
     *
     * @param[in] me                Pointer to the encoding context.
     * @param[out] completed_token  Pointer to store the final token.
     *
     * @return One of the @ref pox_token_err_t errors.
     */
    enum pox_token_err_t
    pox_token_encode_finish(struct pox_token_encode_ctx *me,
                            struct q_useful_buf_c *completed_token);

#ifdef __cplusplus
}
#endif

#endif /* __POX_TOKEN_H__ */
