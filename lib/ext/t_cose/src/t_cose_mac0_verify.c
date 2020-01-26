/*
 * Copyright (c) 2018-2019, Laurence Lundblade. All rights reserved.
 * Copyright (c) 2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */

#include "qcbor.h"
#include "t_cose_crypto.h"
#include "t_cose_mac0_verify.h"
#include "t_cose_parameters.h"
#include "t_cose_util.h"

/**
 * \file t_cose_mac0_verify.c
 *
 * \brief This verifies t_cose Mac authentication structure without a recipient
 *        structure.
 *        Only HMAC is supported so far.
 */

/*
 * Public function. See t_cose_mac0.h
 */
enum t_cose_err_t t_cose_mac0_verify(struct t_cose_mac0_verify_ctx *context,
                                     struct q_useful_buf_c          cose_mac0,
                                     struct q_useful_buf_c         *payload,
                                     struct t_cose_parameters      *parameters)
{
    QCBORDecodeContext            decode_context;
    QCBORItem                     item;
    struct q_useful_buf_c         protected_parameters;
    struct t_cose_parameters      parsed_protected_parameters;
    struct t_cose_parameters      unprotected_parameters;
    struct t_cose_label_list      critical_labels;
    struct t_cose_label_list      unknown_labels;
    enum t_cose_err_t             return_value;
    struct q_useful_buf_c         tag;
    struct q_useful_buf_c         tbm_first_part;
    /* Buffer for the ToBeMaced */
    Q_USEFUL_BUF_MAKE_STACK_UB(   tbm_first_part_buf,
                                  T_COSE_SIZE_OF_TBM);
    struct t_cose_crypto_hmac     hmac_ctx;

    *payload = NULL_Q_USEFUL_BUF_C;

    QCBORDecode_Init(&decode_context, cose_mac0, QCBOR_DECODE_MODE_NORMAL);
    /* Calls to QCBORDecode_GetNext() rely on item.uDataType != QCBOR_TYPE_ARRAY
     * to detect decoding errors rather than checking the return code.
     */

    /* --  The array of four -- */
    (void)QCBORDecode_GetNext(&decode_context, &item);
    if(item.uDataType != QCBOR_TYPE_ARRAY) {
        return_value = T_COSE_ERR_MAC0_FORMAT;
        goto Done;
    }

    if((context->option_flags & T_COSE_OPT_TAG_REQUIRED) &&
       !QCBORDecode_IsTagged(&decode_context, &item, CBOR_TAG_COSE_MAC0)) {
        return_value = T_COSE_ERR_INCORRECTLY_TAGGED;
        goto Done;
    }

    /* -- Clear list where unknown labels are accumulated -- */
    clear_label_list(&unknown_labels);

    /* --  Get the protected header parameters -- */
    (void)QCBORDecode_GetNext(&decode_context, &item);
    if(item.uDataType != QCBOR_TYPE_BYTE_STRING) {
        return_value = T_COSE_ERR_MAC0_FORMAT;
        goto Done;
    }

    protected_parameters = item.val.string;

    return_value = parse_protected_header_parameters(protected_parameters,
                                                   &parsed_protected_parameters,
                                                   &critical_labels,
                                                   &unknown_labels);
    if(return_value != T_COSE_SUCCESS) {
        goto Done;
    }

    /* --  Get the unprotected parameters -- */
    return_value = parse_unprotected_header_parameters(&decode_context,
                                                       &unprotected_parameters,
                                                       &unknown_labels);
    if(return_value != T_COSE_SUCCESS) {
        goto Done;
    }
    if((context->option_flags & T_COSE_OPT_REQUIRE_KID) &&
        q_useful_buf_c_is_null(unprotected_parameters.kid)) {
        return_value = T_COSE_ERR_NO_KID;
        goto Done;
    }

    /* -- Check critical parameter labels -- */
    return_value = check_critical_labels(&critical_labels, &unknown_labels);
    if(return_value != T_COSE_SUCCESS) {
        goto Done;
    }

    /* -- Check for duplicate parameters and copy to returned parameters -- */
    return_value = check_and_copy_parameters(&parsed_protected_parameters,
                                             &unprotected_parameters,
                                             parameters);
    if(return_value != T_COSE_SUCCESS) {
        goto Done;
    }

    /* -- Get the payload -- */
    QCBORDecode_GetNext(&decode_context, &item);
    if(item.uDataType != QCBOR_TYPE_BYTE_STRING) {
        return_value = T_COSE_ERR_MAC0_FORMAT;
        goto Done;
    }
    *payload = item.val.string;

    /* -- Get the tag -- */
    QCBORDecode_GetNext(&decode_context, &item);
    if(item.uDataType != QCBOR_TYPE_BYTE_STRING) {
        return_value = T_COSE_ERR_MAC0_FORMAT;
        goto Done;
    }
    tag = item.val.string;

    /* -- Finish up the CBOR decode -- */
    /* This check make sure the array only had the expected four
     * items. Works for definite and indefinite length arrays. Also
     * make sure there were no extra bytes.
     */
    if(QCBORDecode_Finish(&decode_context) != QCBOR_SUCCESS) {
        return_value = T_COSE_ERR_CBOR_NOT_WELL_FORMED;
        goto Done;
    }

    /* -- Skip tag verification if such is requested --*/
    if(context->option_flags & T_COSE_OPT_DECODE_ONLY) {
        return_value = T_COSE_SUCCESS;
        goto Done;
    }

    /* -- Compute the ToBeMaced -- */
    return_value = create_tbm(tbm_first_part_buf,
                              protected_parameters,
                              &tbm_first_part,
                              T_COSE_TBM_BARE_PAYLOAD,
                              *payload);
    if(return_value) {
        goto Done;
    }

    /*
     * Start the HMAC verification.
     * Calculate the tag of the first part of ToBeMaced and the wrapped
     * payload, to save a bigger buffer containing the entire ToBeMaced.
     */
    return_value = t_cose_crypto_hmac_verify_setup(&hmac_ctx,
                                  parsed_protected_parameters.cose_algorithm_id,
                                  context->verification_key);
    if(return_value) {
        goto Done;
    }

    /* Compute the tag of the first part. */
    return_value = t_cose_crypto_hmac_update(&hmac_ctx,
                                         q_useful_buf_head(tbm_first_part,
                                                           tbm_first_part.len));
    if(return_value) {
        goto Done;
    }

    return_value = t_cose_crypto_hmac_update(&hmac_ctx, *payload);
    if(return_value) {
        goto Done;
    }

    return_value = t_cose_crypto_hmac_verify_finish(&hmac_ctx, tag);

Done:
    return return_value;
}
