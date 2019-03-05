/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_IRQ_TEST_1_SERVICE_SIGNAL_H__
#define __TFM_IRQ_TEST_1_SERVICE_SIGNAL_H__

/* FixMe: hardcode it for the tool cannot support now */
#ifdef TFM_PSA_API

#define SPM_CORE_IRQ_TEST_1_PREPARE_TEST_SCENARIO_SIGNAL    (1U << (4  + 4))
#define SPM_CORE_IRQ_TEST_1_EXECUTE_TEST_SCENARIO_SIGNAL    (1U << (5  + 4))

#endif /* TFM_PSA_API */

#endif /* __TFM_IRQ_TEST_1_SERVICE_SIGNAL_H__ */
