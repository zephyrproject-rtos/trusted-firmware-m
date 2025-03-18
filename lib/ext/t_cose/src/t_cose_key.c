/*
 * t_cose_key.c
 *
 * Copyright 2023, Laurence Lundblade
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Created by Laurence Lundblade on 2/6/23.
 *
 * See BSD-3-Clause license in README.md
 */

#include "t_cose_key.h"
#include "t_cose_crypto.h"
#include "t_cose_util.h"
#include "qcbor/qcbor_encode.h"
#include "qcbor/qcbor_spiffy_decode.h"


enum t_cose_err_t
t_cose_key_decode(struct q_useful_buf_c cbor_encoded,
                  struct t_cose_key     *key)
{
    QCBORDecodeContext     cbor_decoder;
    int64_t                kty;
    int64_t                curve;
    struct q_useful_buf_c  x;
    struct q_useful_buf_c  y_string;
    bool                   y_bool;
    QCBORItem              y;
    enum t_cose_err_t      result;


    QCBORDecode_Init(&cbor_decoder, cbor_encoded, 0);

    QCBORDecode_EnterMap(&cbor_decoder, NULL);

    QCBORDecode_GetInt64InMapN(&cbor_decoder, COSE_KEY_COMMON_KTY, &kty);
    QCBORDecode_GetInt64InMapN(&cbor_decoder, COSE_KEY_PARAM_CRV, &curve);
    QCBORDecode_GetByteStringInMapN(&cbor_decoder, COSE_KEY_PARAM_X_COORDINATE, &x);
    QCBORDecode_GetItemInMapN(&cbor_decoder, COSE_KEY_PARAM_Y_COORDINATE, QCBOR_TYPE_ANY, &y);

    QCBORDecode_ExitMap(&cbor_decoder);
    if(QCBORDecode_GetError(&cbor_decoder)) {
        return T_COSE_ERR_FAIL; // TODO: is this right?
    }

    if (kty != COSE_KEY_TYPE_EC2) {
        return T_COSE_ERR_WRONG_TYPE_OF_KEY;
    }

    /* If y is a bool, then point compression is used and y is a boolean
     * indicating the sign. If not then it is a byte string with the y.
     * Anything else is an error. See RFC 9053 7.1.1.
     */
    switch(y.uDataType) {
        case QCBOR_TYPE_BYTE_STRING:
            y_string = y.val.string;
            y_bool = true; /* Unused. Only here to avoid compiler warning */
            break;

        case QCBOR_TYPE_TRUE:
            y_bool = true;
            y_string = NULL_Q_USEFUL_BUF_C;
            break;

        case QCBOR_TYPE_FALSE:
            y_bool = true;
            y_string = NULL_Q_USEFUL_BUF_C;
            break;

        default:
            return 77; // TODO: error code
    }

    /* Turn it into a t_cose_key that is imported into the library */

    if(curve > INT32_MAX || curve < INT32_MIN) {
        // Make sure cast is safe
        return T_COSE_ERR_FAIL; // TODO: error
    }
    result = t_cose_crypto_import_ec2_pubkey((int32_t)curve,
                                 x,
                                 y_string,
                                 y_bool,
                                 key);

    return result;
}

/*
 * Public function. See t_cose_key.h
 */
enum t_cose_err_t
t_cose_key_encode(struct t_cose_key      key,
                  struct q_useful_buf    key_buf,
                  struct q_useful_buf_c *cbor_encoded)
{
    enum t_cose_err_t      result;
    int32_t                cose_curve;
    MakeUsefulBufOnStack(  x_coord_buf, T_COSE_BITS_TO_BYTES(T_COSE_ECC_MAX_CURVE_BITS));
    MakeUsefulBufOnStack(  y_coord_buf, T_COSE_BITS_TO_BYTES(T_COSE_ECC_MAX_CURVE_BITS));
    struct q_useful_buf_c  x_coord;
    struct q_useful_buf_c  y_coord;
    bool                   y_sign;
    QCBOREncodeContext     cbor_encoder;
    QCBORError             qcbor_result;

    result = t_cose_crypto_export_ec2_key(key,
                                          &cose_curve,
                                          x_coord_buf,
                                          &x_coord,
                                          y_coord_buf,
                                          &y_coord,
                                          &y_sign);
    if (result != T_COSE_SUCCESS) {
        return result;
    }

    QCBOREncode_Init(&cbor_encoder, key_buf);

    QCBOREncode_OpenMap(&cbor_encoder);

    QCBOREncode_AddInt64ToMapN(&cbor_encoder, COSE_KEY_COMMON_KTY, COSE_KEY_TYPE_EC2);
    QCBOREncode_AddInt64ToMapN(&cbor_encoder, COSE_KEY_PARAM_CRV, cose_curve);
    QCBOREncode_AddBytesToMapN(&cbor_encoder, COSE_KEY_PARAM_X_COORDINATE, x_coord);
    if (q_useful_buf_c_is_null(y_coord)) {
        QCBOREncode_AddBoolToMapN(&cbor_encoder, COSE_KEY_PARAM_Y_COORDINATE, y_sign);
    } else {
        QCBOREncode_AddBytesToMapN(&cbor_encoder, COSE_KEY_PARAM_Y_COORDINATE, y_coord);
    }

    QCBOREncode_CloseMap(&cbor_encoder);

    qcbor_result = QCBOREncode_Finish(&cbor_encoder, cbor_encoded);
    if (qcbor_result != QCBOR_SUCCESS) {
        /* Mainly means that the COSE_Key was too big for key_buf */
        return qcbor_encode_error_to_t_cose_error(&cbor_encoder);
    }

    return T_COSE_SUCCESS;
}
