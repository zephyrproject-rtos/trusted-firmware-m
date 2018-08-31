/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "tfm_crypto_defs.h"

#include "psa_crypto.h"

#include "crypto_psa_wrappers.h"

#include "tfm_crypto_api.h"

/*!
 * \defgroup public Public functions, TF-M compatible wrappers
 *
 */

/*!@{*/
enum tfm_crypto_err_t tfm_crypto_cipher_update_wrapper(
                                              psa_cipher_operation_t *operation,
                                        struct psa_cipher_update_input *input_s,
                                      struct psa_cipher_update_output *output_s)
{
    /* Extract the following fields from the input and output structures */

    const uint8_t *input = input_s->input;
    size_t input_length = input_s->input_length;

    unsigned char *output = output_s->output;
    size_t output_size = output_s->output_size;
    size_t *output_length = output_s->output_length;

    return tfm_crypto_cipher_update(operation, input, input_length,
                                    output, output_size, output_length);
}
/*!@}*/
