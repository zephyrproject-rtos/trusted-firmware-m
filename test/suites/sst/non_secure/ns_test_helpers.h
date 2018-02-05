/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NS_TEST_HELPERS_H__
#define __NS_TEST_HELPERS_H__

#include "test/framework/test_framework.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void test_func_t(struct test_result_t *ret);

/**
 * \brief Executes the given test function from the specified thread context.
 *
 * \param[in]  thread_name  Name of the thread to be created for test
 * \param[out] ret          Result of the test
 * \param[in]  test_func    Test function to be run in the new thread
 */
void tfm_sst_run_test(const char *thread_name, struct test_result_t *ret,
                      test_func_t *test_func);

#ifdef __cplusplus
}
#endif

#endif /* __NS_TEST_HELPERS_H__ */
