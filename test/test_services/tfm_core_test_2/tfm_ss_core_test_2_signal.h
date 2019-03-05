/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_CORE_TEST_2_SIGNAL_H__
#define __TFM_CORE_TEST_2_SIGNAL_H__

/* FixMe: hardcode it for the tool cannot support now */
#ifdef TFM_PSA_API

#define SPM_CORE_TEST_2_SLAVE_SERVICE_SIGNAL            (1 << (0  + 4))
#define SPM_CORE_TEST_2_CHECK_CALLER_CLIENT_ID_SIGNAL   (1 << (1  + 4))
#define SPM_CORE_TEST_2_GET_EVERY_SECOND_BYTE_SIGNAL    (1 << (2  + 4))
#define SPM_CORE_TEST_2_INVERT_SIGNAL                   (1 << (3  + 4))
#define SPM_CORE_TEST_2_PREPARE_TEST_SCENARIO_SIGNAL    (1 << (4  + 4))
#define SPM_CORE_TEST_2_EXECUTE_TEST_SCENARIO_SIGNAL    (1 << (5  + 4))

#endif /* TFM_PSA_API */

#endif /* __TFM_CORE_TEST_2_SIGNAL_H__ */
