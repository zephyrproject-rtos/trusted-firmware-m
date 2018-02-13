/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_TESTS_H__
#define __SST_TESTS_H__

#include "test/framework/test_framework.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Register testsuite for the sst interface tests.
 *
 * \param[in] p_test_suite  The test suite to be executed.
 */
void register_testsuite_s_sst_sec_interface(struct test_suite_t *p_test_suite);

/**
 * \brief Register testsuite for the sst reliability tests.
 *
 * \param[in] p_test_suite  The test suite to be executed.
 */
void register_testsuite_s_sst_reliability(struct test_suite_t *p_test_suite);

#ifdef __cplusplus
}
#endif

#endif /* __SST_TESTS_H__ */
