/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
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
 * \brief Does an SVC for calling the secure function provided in \ref fn_ptr
 *
 * \param[in] fn_ptr  Secure function to be called.
 * \param[in] args    Arguments for fn_ptr.
 *
 * \return Returns value depending on fn_ptr.
 */
int32_t tfm_core_test_svc(void *fn_ptr, int32_t args[]);

/**
 * \brief Tests secure function \ref fn_ptr called two times sequentially.
 *
 * \param[in] fn_ptr  Secure function to be called.
 * \param[in] args    Arguments for fn_ptr.
 *
 * \return Returns \ref CORE_TEST_ERRNO_SUCCESS or error if one of the calls
 *         failed
 */
int32_t tfm_core_test_multiple_calls_svc(void *fn_ptr, int32_t args[]);

#ifdef __cplusplus
}
#endif

#endif /* __CORE_TEST_API_H__ */
