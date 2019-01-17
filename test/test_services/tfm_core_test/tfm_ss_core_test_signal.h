/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_CORE_TEST_SIGNAL_H__
#define __TFM_CORE_TEST_SIGNAL_H__

/* FixMe: hardcode it for the tool cannot support now */
#ifdef TFM_PSA_API

#define SPM_CORE_TEST_INIT_SUCCESS_SIGNAL          (1 << (0  + 4))
#define SPM_CORE_TEST_DIRECT_RECURSION_SIGNAL      (1 << (1  + 4))
#define SPM_CORE_TEST_MPU_ACCESS_SIGNAL            (1 << (2  + 4))
#define SPM_CORE_TEST_MEMORY_PERMISSIONS_SIGNAL    (1 << (3  + 4))
#define SPM_CORE_TEST_SHARE_REDIRECTION_SIGNAL     (1 << (4  + 4))
#define SPM_CORE_TEST_SS_TO_SS_SIGNAL              (1 << (5  + 4))
#define SPM_CORE_TEST_SS_TO_SS_BUFFER_SIGNAL       (1 << (6  + 4))
#define SPM_CORE_TEST_OUTVEC_WRITE_SIGNAL          (1 << (7  + 4))
#define SPM_CORE_TEST_PERIPHERAL_ACCESS_SIGNAL     (1 << (8  + 4))
#define SPM_CORE_TEST_GET_CALLER_CLIENT_ID_SIGNAL  (1 << (9  + 4))
#define SPM_CORE_TEST_SPM_REQUEST_SIGNAL           (1 << (10 + 4))
#define SPM_CORE_TEST_BLOCK_SIGNAL                 (1 << (11 + 4))
#define SPM_CORE_TEST_NS_THREAD_SIGNAL             (1 << (12 + 4))
#endif /* TFM_PSA_API */

#endif /* __TFM_CORE_TEST_SIGNAL_H__ */
