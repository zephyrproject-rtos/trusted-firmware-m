/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __S_TEST_HELPERS_H__
#define __S_TEST_HELPERS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "test/framework/test_framework.h"

/**
 * \brief Prepares secure storage area for the new tests. It executes wipes and
 *        prepare.
 *
 * \param[out] ret  Pointer to the test result structure.
 *
 * \return 0 if the context is correcly prepared. Otherwise 1.
 */
uint32_t prepare_test_ctx(struct test_result_t *ret);

#ifdef __cplusplus
}
#endif

#endif /* __S_TEST_HELPERS_H__ */
