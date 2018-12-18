/*
 * token_test.c
 *
 * Copyright (c) 2018-2019, Laurence Lundblade. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */

#include "token_test.h"
#include "useful_buf.h"
#include "psa_initial_attestation_api.h"
#include "attest_token.h"

/**
 * \file token_test.c
 *
 * \brief Entry points for very basic attestation token tests.
 */


/**
 * \brief An alternate token_main() that packs the option flags into the nonce.
 *
 * \param[in] option_flags      Flag bits to pack into nonce.
 * \param[in] nonce             Pointer and length of the nonce.
 * \param[in] buffer            Pointer and length of buffer to
 *                              output the token into.
 * \param[out] completed_token  Place to put pointer and length
 *                              of completed token.
 *
 * \return various errors. See \ref attest_token_err_t.
 *
 */
int token_main_alt(uint32_t option_flags,
                   struct useful_buf_c nonce,
                   struct useful_buf buffer,
                   struct useful_buf_c *completed_token)
{
    uint32_t completed_token_len;

    /* 68 = 64 + 4, the max expected nonce and 4 for the options */
    USEFUL_BUF_MAKE_STACK_UB(nonce2_storage, 68);
    struct useful_buf_c nonce2;
    int return_value;

    nonce2 = useful_buf_copy(nonce2_storage, nonce);
    /* Now for the hack... */
    memcpy((uint8_t *)nonce2.ptr + nonce2.len, (uint8_t *) &option_flags, 4);
    nonce2.len += 4;

    completed_token_len = (uint32_t)buffer.len;
    return_value = psa_initial_attest_get_token(nonce2.ptr,
                                                (uint32_t)nonce2.len,
                                                buffer.ptr,
                                                &completed_token_len);

    *completed_token = (struct useful_buf_c) {buffer.ptr, completed_token_len};

    return return_value;
}



/** A fake hard coded nonce for testing. */
static const uint8_t nonce_bytes[] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
};


/**
 * This is the expected output for the minimal test. It is the result
 * of creating a token with \ref TOKEN_OPT_SHORT_CIRCUIT_SIGN and
 * \ref TOKEN_OPT_OMIT_CLAIMS set. The nonce is the above constant string
 * nonce_bytes[].  The token output is completely deterministic.
 *
 * The implementation of TOKEN_OPT_SHORT_CIRCUIT_SIGN always uses the
 * kid
 * EF954B4BD9BDF670D0336082F5EF152AF8F35B6A6C00EFA6A9A71F49517E18C6.
 *
 * 18(
 *   [
 *       h'A10126', // protected headers
 *       { // unprotected headers
 *           4: h'EF954B4BD9BDF670D0336082F5EF152AF8F35B6A6C00EFA6A9
 *                A71F49517E18C6'
 *       },
 *       h'A13A000124FF5820010203040506070801020304050607080102030405
 *         0607080102030405060708',
 *       h'BD0990E025C671BF0FEB35D8908AF9E4F36706D04044BEB7325C2C
 *         A2753E4263BD0990E025C671BF0FEB35D8908AF9E4F36706D04044
 *         BEB7325C2CA2753E4263'
 *   ]
 * )
 *
 * The above is in CBOR Diagnostic notation. See RFC 8152.
 */

static const uint8_t expected_minimal_token_bytes[] = {
    0xD2, 0x84, 0x43, 0xA1, 0x01, 0x26, 0xA1, 0x04,
    0x58, 0x20, 0xEF, 0x95, 0x4B, 0x4B, 0xD9, 0xBD,
    0xF6, 0x70, 0xD0, 0x33, 0x60, 0x82, 0xF5, 0xEF,
    0x15, 0x2A, 0xF8, 0xF3, 0x5B, 0x6A, 0x6C, 0x00,
    0xEF, 0xA6, 0xA9, 0xA7, 0x1F, 0x49, 0x51, 0x7E,
    0x18, 0xC6, 0x58, 0x28, 0xA1, 0x3A, 0x00, 0x01,
    0x24, 0xFF, 0x58, 0x20, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x58, 0x40, 0xBD, 0x09,
    0x90, 0xE0, 0x25, 0xC6, 0x71, 0xBF, 0x0F, 0xEB,
    0x35, 0xD8, 0x90, 0x8A, 0xF9, 0xE4, 0xF3, 0x67,
    0x06, 0xD0, 0x40, 0x44, 0xBE, 0xB7, 0x32, 0x5C,
    0x2C, 0xA2, 0x75, 0x3E, 0x42, 0x63, 0xBD, 0x09,
    0x90, 0xE0, 0x25, 0xC6, 0x71, 0xBF, 0x0F, 0xEB,
    0x35, 0xD8, 0x90, 0x8A, 0xF9, 0xE4, 0xF3, 0x67,
    0x06, 0xD0, 0x40, 0x44, 0xBE, 0xB7, 0x32, 0x5C,
    0x2C, 0xA2, 0x75, 0x3E, 0x42, 0x63
};



/*
 * Public function. See token_test.h
 */
int minimal_test()
{
    int return_value = 0;
    USEFUL_BUF_MAKE_STACK_UB(token_storage,
                             sizeof(expected_minimal_token_bytes));
    struct useful_buf_c completed_token;
    struct useful_buf_c expected_token;

    return_value =
        token_main_alt(TOKEN_OPT_SHORT_CIRCUIT_SIGN |
                           TOKEN_OPT_OMIT_CLAIMS,
                       USEFUL_BUF_FROM_BYTE_ARRAY_LITERAL(nonce_bytes),
                       token_storage,
                       &completed_token);
    if(return_value) {
        goto Done;
    }

    expected_token =
        USEFUL_BUF_FROM_BYTE_ARRAY_LITERAL(expected_minimal_token_bytes);

    if(useful_buf_compare(completed_token,expected_token)) {
       return_value = -3;
    }

Done:
    return return_value;
}


/*
 * Public function. token_test.h
 */
int minimal_get_size_test()
{
    int                 return_value;
    uint32_t            length;
    struct useful_buf_c expected_token;
    struct useful_buf_c nonce;

    return_value = 0;

    nonce = USEFUL_BUF_FROM_BYTE_ARRAY_LITERAL(nonce_bytes);
    expected_token =
        USEFUL_BUF_FROM_BYTE_ARRAY_LITERAL(expected_minimal_token_bytes);


    return_value = psa_initial_attest_get_token_size((uint32_t)nonce.len,
                                                     &length);

    /*
     * It is not possible to predict the size of the token returned
     * here because options like TOKEN_OPT_OMIT_CLAIMS and
     * TOKEN_OPT_SHORT_CIRCUIT_SIGN cannot be passed to limit what it
     * does. Instead check to see if the size is in a reasonable
     * range. The minimal_test() will actually check the size for
     * exactitude because it can pass those options,
     */
    if(length < expected_token.len || length > 10000) {
        return_value = -1;
    }

    return return_value;
}
