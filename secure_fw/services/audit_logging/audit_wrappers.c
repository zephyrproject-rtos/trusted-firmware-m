/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "audit_core.h"
#include "audit_wrappers.h"
#include "psa_audit_defs.h"

/*!
 * \defgroup public Public functions, TF-M compatible wrappers
 *
 */

/*!@{*/
enum psa_audit_err audit_core_retrieve_record_wrapper(
                                const struct audit_core_retrieve_input *input_s,
                                    struct audit_core_retrieve_output *output_s)
{
    return audit_core_retrieve_record(input_s->record_index,
                                      input_s->buffer_size,
                                      input_s->token,
                                      input_s->token_size,
                                      output_s->buffer,
                                      output_s->record_size);
}
/*!@}*/
