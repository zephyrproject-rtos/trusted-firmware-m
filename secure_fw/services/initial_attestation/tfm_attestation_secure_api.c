/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa_initial_attestation_api.h"
#include "tfm_initial_attestation_veneers.h"
#include "secure_utilities.h"
#include "psa_client.h"
#include "tfm_secure_api.h"
#include <string.h>

/* FIXME: If iovec will be supported by SPM then remove the usage of
 * scratch area.
 */
extern uint8_t *tfm_scratch_area;

__attribute__((section("SFN")))
enum psa_attest_err_t
psa_initial_attest_get_token(const uint8_t *challenge_obj,
                             uint32_t       challenge_size,
                             uint8_t       *token,
                             uint32_t      *token_size)
{
    enum psa_attest_err_t err;
    psa_invec *in_vec;
    psa_outvec *out_vec;
    uint8_t *challenge_buff;
    uint8_t *token_buff;

    if (tfm_core_set_buffer_area(TFM_BUFFER_SHARE_SCRATCH) != TFM_SUCCESS) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    /*
     * Scratch area layout
     * -------------------------------------------------------
     * |in_vec[0] | out_vec[0] | challenge_buff | token_buff |
     * -------------------------------------------------------
     */

    in_vec  = (psa_invec  *)(tfm_scratch_area);
    out_vec = (psa_outvec *)(in_vec + 1);

    challenge_buff = (uint8_t *)(out_vec + 1);
    token_buff     = (uint8_t *)(challenge_buff + challenge_size);

    /* Copy challenge object to scratch area */
    tfm_memcpy(challenge_buff, challenge_obj, challenge_size);


    in_vec[0].base = challenge_buff;
    in_vec[0].len  = challenge_size;

    out_vec[0].base = token_buff;
    out_vec[0].len  = *token_size;

    err = tfm_attest_veneer_get_token(in_vec, 1, out_vec, 1);
    if (err != PSA_ATTEST_ERR_SUCCESS) {
        return err;
    }

    /* Copy output token to local buffer */
    tfm_memcpy(token, out_vec[0].base, out_vec[0].len);
    *token_size = out_vec[0].len;

    return err;
}
