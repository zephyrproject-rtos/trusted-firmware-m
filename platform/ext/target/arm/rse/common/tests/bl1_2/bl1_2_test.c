/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "rse_test_common.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

static struct test_t bl1_2_extra_tests[100];

void register_testsuite_extra_bl1_2(struct test_suite_t *p_test_suite)
{
    set_testsuite("RSE Tests", bl1_2_extra_tests, 0, p_test_suite);
}
