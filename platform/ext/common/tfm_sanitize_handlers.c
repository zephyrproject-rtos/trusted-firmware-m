/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

struct source_location {
    const char *file_name;
    uint32_t line;
    uint32_t column;
};

struct type_descriptor {
    uint16_t type_kind;
    uint16_t type_info;
    char type_name[1];
};

struct type_mismatch_data {
    struct source_location loc;
    struct type_descriptor *type;
    unsigned long alignment;
    unsigned char type_check_kind;
};

struct type_mismatch_data_v1 {
    struct source_location loc;
    struct type_descriptor *type;
    unsigned char log_alignment;
    unsigned char type_check_kind;
};

struct overflow_data {
    struct source_location loc;
    struct type_descriptor *type;
};

struct shift_out_of_bounds_data {
    struct source_location loc;
    struct type_descriptor *lhs_type;
    struct type_descriptor *rhs_type;
};

struct out_of_bounds_data {
    struct source_location loc;
    struct type_descriptor *array_type;
    struct type_descriptor *index_type;
};

struct unreachable_data {
    struct source_location loc;
};

struct vla_bound_data {
    struct source_location loc;
    struct type_descriptor *type;
};

struct invalid_value_data {
    struct source_location loc;
    struct type_descriptor *type;
};

struct nonnull_arg_data {
    struct source_location loc;
};

void __ubsan_handle_type_mismatch_v1(struct type_mismatch_data_v1 *data,
                                     unsigned long ptr)
{
    while(1);
}

void __ubsan_handle_add_overflow(struct overflow_data *data, unsigned long lhs,
                                 unsigned long rhs)
{
    while(1);
}

void __ubsan_handle_sub_overflow(struct overflow_data *data, unsigned long lhs,
                                 unsigned long rhs)
{
    while(1);
}

void __ubsan_handle_mul_overflow(struct overflow_data *data, unsigned long lhs,
                                 unsigned long rhs)
{
    while(1);
}

void __ubsan_handle_negate_overflow(struct overflow_data *data,
                                    unsigned long old_val)
{
    while(1);
}

void __ubsan_handle_divrem_overflow(struct overflow_data *data,
                                    unsigned long lhs, unsigned long rhs)
{
    while(1);
}

void __ubsan_handle_pointer_overflow(struct overflow_data *data,
                                    unsigned long lhs, unsigned long rhs)
{
    while(1);
}

void __ubsan_handle_shift_out_of_bounds(struct shift_out_of_bounds_data *data,
                                        unsigned long lhs, unsigned long rhs)
{
    while(1);
}

void __ubsan_handle_out_of_bounds(struct out_of_bounds_data *data,
                  unsigned long idx)
{
    while(1);
}

void __ubsan_handle_unreachable(struct unreachable_data *data)
{
    while(1);
}

void __ubsan_handle_missing_return(struct unreachable_data *data)
{
    while(1);
}

void __ubsan_handle_vla_bound_not_positive(struct vla_bound_data *data,
                                           unsigned long bound)
{
    while(1);
}

void __ubsan_handle_load_invalid_value(struct invalid_value_data *data,
                                       unsigned long val)
{
    while(1);
}

void __ubsan_handle_invalid_builtin(struct invalid_value_data *data,
                                    unsigned long val)
{
    while(1);
}

void __ubsan_handle_nonnull_arg(struct nonnull_arg_data *data)
{
    while(1);
}
