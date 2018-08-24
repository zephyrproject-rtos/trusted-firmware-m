/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa_audit_api.h"
#include "tfm_audit_veneers.h"
#include "audit_wrappers.h"

__attribute__((section("SFN")))
enum psa_audit_err psa_audit_retrieve_record(const uint32_t record_index,
                                             const uint32_t buffer_size,
                                             const uint8_t *token,
                                             const uint32_t token_size,
                                             uint8_t *buffer,
                                             uint32_t *record_size)
{
    const struct audit_core_retrieve_input input_s =
                                       {.record_index = record_index,
                                        .buffer_size = buffer_size,
                                        .token = token,
                                        .token_size = token_size};

    struct audit_core_retrieve_output output_s = {.buffer = buffer,
                                                  .record_size = record_size};

    return tfm_audit_veneer_retrieve_record(&input_s, &output_s);
}

__attribute__((section("SFN")))
enum psa_audit_err psa_audit_get_info(uint32_t *num_records, uint32_t *size)
{
    return tfm_audit_veneer_get_info(num_records, size);
}

__attribute__((section("SFN")))
enum psa_audit_err psa_audit_get_record_info(const uint32_t record_index,
                                             uint32_t *size)
{
    return tfm_audit_veneer_get_record_info(record_index, size);
}

__attribute__((section("SFN")))
enum psa_audit_err psa_audit_delete_record(const uint32_t record_index,
                                           const uint8_t *token,
                                           const uint32_t token_size)
{
    return tfm_audit_veneer_delete_record(record_index, token, token_size);
}

__attribute__((section("SFN")))
enum psa_audit_err psa_audit_add_record(const struct psa_audit_record *record)
{
    return tfm_audit_veneer_add_record(record);
}
