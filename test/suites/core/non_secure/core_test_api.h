/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CORE_TEST_API_H__
#define __CORE_TEST_API_H__

#include <stdio.h>
#include <cmsis_compiler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Calls the secure function provided in \ref fn_ptr
 *
 * \param[in] fn_ptr  Secure function to be called.
 * \param[in] args    Arguments for fn_ptr.
 *
 * \return Returns value depending on fn_ptr.
 */
int32_t tfm_core_test_call(void *fn_ptr, int32_t args[]);

#ifdef __cplusplus
}
#endif

#endif /* __CORE_TEST_API_H__ */
