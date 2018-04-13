/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_SS_CORE_TEST_2_VENEERS_H__
#define __TFM_SS_CORE_TEST_2_VENEERS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <cmsis_compiler.h>

/**
 * \brief A minimal test service to be called from another service.
 *
 * \return Returns \ref TFM_SUCCESS.
 */
int32_t tfm_core_test_2_veneer_slave_service(void);

/**
 * \brief Bitwise inverts the buffer received as input.
 *
 * \param[out] res_ptr  Result, 0 on success, -1 othervise.
 * \param[in] in_ptr    Buffer containing data to be inverted.
 * \param[out] out_ptr  Buffer to store the inverted data.
 * \param[in] len       Number of bytes to be inverted.
 *
 * \return Returns \ref TFM_SUCCESS on success, TFM_PARTITION_BUSY otherwise.
 */
int32_t tfm_core_test_2_sfn_invert(int32_t *res_ptr,
                                   uint32_t *in_ptr,
                                   uint32_t *out_ptr,
                                   int32_t len);

/**
 * \brief A minimal test secure function to be called from another partition.
 *
 * Checks the functionality of querying the client ID of the caller service.
 *
 * \return Returns \ref TFM_SUCCESS on success, \ref CORE_TEST_ERRNO_TEST_FAULT
 *         othervise.
 */
int32_t tfm_core_test_2_check_caller_client_id(void);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_SS_CORE_TEST_2_VENEERS_H__ */
