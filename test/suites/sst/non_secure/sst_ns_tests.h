/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_NS_TESTS_H__
#define __SST_NS_TESTS_H__

#include "test/framework/test_framework.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Register testsuite for the SST tests.
 *
 * \param[in] p_test_suite  The test suite to be executed.
 */
void register_testsuite_ns_sst_interface(struct test_suite_t *p_test_suite);

/**
 * \brief Register testsuite for the non-secure SST referenced access tests.
 *
 * \param[in] p_test_suite  The test suite to be executed.
 */
void register_testsuite_ns_sst_ref_access(struct test_suite_t *p_test_suite);

/**
 * \brief Register testsuite for the SST policy tests.
 *
 * \param[in] p_test_suite  The test suite to be executed.
 */
void register_testsuite_ns_sst_policy(struct test_suite_t *p_test_suite);

#ifdef __cplusplus
}
#endif

#endif /* __SST_NS_TESTS_H__ */
