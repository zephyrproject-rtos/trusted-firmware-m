/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TEST_STATE_TRANSITIONS_H__
#define __TEST_STATE_TRANSITIONS_H__

#include "test_framework.h"

#ifdef __cplusplus
extern "C" {
#endif

void rse_test_virgin_to_tci_or_pci(struct test_result_t *ret);

void rse_test_cm_sp_enable(struct test_result_t *ret);
void rse_test_cm_to_dm(struct test_result_t *ret);

void rse_test_dm_sp_enable(struct test_result_t *ret);
void rse_test_dm_to_se(struct test_result_t *ret);

void rse_test_se_to_rma(struct test_result_t *ret);

#ifdef __cplusplus
}
#endif

#endif /* __TEST_STATE_TRANSITIONS_H__ */
