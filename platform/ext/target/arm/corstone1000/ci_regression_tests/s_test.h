/*
 * Copyright (c) 2021-22, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __S_TESTS_H__
#define __S_TESTS_H__

#include "extra_tests_common.h"

#ifdef __cplusplus
extern "C" {
#endif

const struct extra_tests_t plat_s_t;

/**
 * \brief Platform specific secure test function.
 *
 * \returns Returns error code as specified in \ref int32_t
 */
int32_t s_test(void);

#ifdef __cplusplus
}
#endif

#endif /* __S_TESTS_H__ */
