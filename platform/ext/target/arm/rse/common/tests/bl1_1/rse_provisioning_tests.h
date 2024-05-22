/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSE_PROVISIONING_TESTS_H__
#define __RSE_PROVISIONING_TESTS_H__

#include "test_framework.h"

#ifdef __cplusplus
extern "C" {
#endif

void rse_bl1_provisioning_test_0001(struct test_result_t *ret);
void rse_bl1_provisioning_test_0002(struct test_result_t *ret);
void rse_bl1_provisioning_test_0003(struct test_result_t *ret);
void rse_bl1_provisioning_test_0004(struct test_result_t *ret);

void rse_bl1_provisioning_test_0101(struct test_result_t *ret);

void rse_bl1_provisioning_test_0201(struct test_result_t *ret);
void rse_bl1_provisioning_test_0202(struct test_result_t *ret);

void rse_bl1_provisioning_test_0301(struct test_result_t *ret);
void rse_bl1_provisioning_test_0302(struct test_result_t *ret);
void rse_bl1_provisioning_test_0303(struct test_result_t *ret);
void rse_bl1_provisioning_test_0304(struct test_result_t *ret);

#ifdef __cplusplus
}
#endif

#endif /* __RSE_PROVISIONING_TESTS_H__ */
