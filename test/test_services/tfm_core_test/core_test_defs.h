/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CORE_TEST_DEFS_H__
#define __CORE_TEST_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <limits.h>
#include "tfm_api.h"
#include "secure_fw/core/tfm_core.h"

/* These definitions are used in symbols, only digits are permitted */
#define CORE_TEST_ID_NS_THREAD            1001
#define CORE_TEST_ID_CHECK_INIT           1003
#define CORE_TEST_ID_RECURSION            1004
#define CORE_TEST_ID_MEMORY_PERMISSIONS   1005
#define CORE_TEST_ID_MPU_ACCESS           1006
#define CORE_TEST_ID_BUFFER_CHECK         1007
#define CORE_TEST_ID_SS_TO_SS             1008
#define CORE_TEST_ID_SHARE_REDIRECTION    1009
#define CORE_TEST_ID_SS_TO_SS_BUFFER      1010
#define CORE_TEST_ID_PERIPHERAL_ACCESS    1012
#define CORE_TEST_ID_GET_CALLER_CLIENT_ID 1013
#define CORE_TEST_ID_SPM_REQUEST          1014
#define CORE_TEST_ID_IOVEC_SANITIZATION   1015
#define CORE_TEST_ID_OUTVEC_WRITE         1016
#define CORE_TEST_ID_SECURE_IRQ           1017
#define CORE_TEST_ID_BLOCK                2001

#define SPM_CORE_TEST_INIT_SUCCESS_SID                 0x0000F020
#define SPM_CORE_TEST_DIRECT_RECURSION_SID             0x0000F021
#define SPM_CORE_TEST_MPU_ACCESS_SID                   0x0000F022
#define SPM_CORE_TEST_MEMORY_PERMISSIONS_SID           0x0000F023
#define SPM_CORE_TEST_SHARE_REDIRECTION_SID            0x0000F024
#define SPM_CORE_TEST_SS_TO_SS_SID                     0x0000F025
#define SPM_CORE_TEST_SS_TO_SS_BUFFER_SID              0x0000F026
#define SPM_CORE_TEST_OUTVEC_WRITE_SID                 0x0000F027
#define SPM_CORE_TEST_PERIPHERAL_ACCESS_SID            0x0000F028
#define SPM_CORE_TEST_GET_CALLER_CLIENT_ID_SID         0x0000F029
#define SPM_CORE_TEST_SPM_REQUEST_SID                  0x0000F02A
#define SPM_CORE_TEST_BLOCK_SID                        0x0000F02B
#define SPM_CORE_TEST_NS_THREAD_SID                    0x0000F02C
#define SPM_CORE_TEST_INIT_SUCCESS_MIN_VER                 0x0001
#define SPM_CORE_TEST_DIRECT_RECURSION_MIN_VER             0x0001
#define SPM_CORE_TEST_MPU_ACCESS_MIN_VER                   0x0001
#define SPM_CORE_TEST_MEMORY_PERMISSIONS_MIN_VER           0x0001
#define SPM_CORE_TEST_SHARE_REDIRECTION_MIN_VER            0x0001
#define SPM_CORE_TEST_SS_TO_SS_MIN_VER                     0x0001
#define SPM_CORE_TEST_SS_TO_SS_BUFFER_MIN_VER              0x0001
#define SPM_CORE_TEST_OUTVEC_WRITE_MIN_VER                 0x0001
#define SPM_CORE_TEST_PERIPHERAL_ACCESS_MIN_VER            0x0001
#define SPM_CORE_TEST_GET_CALLER_CLIENT_ID_MIN_VER         0x0001
#define SPM_CORE_TEST_SPM_REQUEST_MIN_VER                  0x0001
#define SPM_CORE_TEST_BLOCK_MIN_VER                        0x0001
#define SPM_CORE_TEST_NS_THREAD_MIN_VER                    0x0001

#define SPM_CORE_TEST_2_SLAVE_SERVICE_SID              0x0000F040
#define SPM_CORE_TEST_2_CHECK_CALLER_CLIENT_ID_SID     0x0000F041
#define SPM_CORE_TEST_2_GET_EVERY_SECOND_BYTE_SID      0x0000F042
#define SPM_CORE_TEST_2_INVERT_SID                     0x0000F043
#define SPM_CORE_TEST_2_PREPARE_TEST_SCENARIO_SID      0x0000F044
#define SPM_CORE_TEST_2_EXECUTE_TEST_SCENARIO_SID      0x0000F045
#define SPM_CORE_TEST_2_SLAVE_SERVICE_MIN_VER              0x0001
#define SPM_CORE_TEST_2_CHECK_CALLER_CLIENT_ID_MIN_VER     0x0001
#define SPM_CORE_TEST_2_GET_EVERY_SECOND_BYTE_MIN_VER      0x0001
#define SPM_CORE_TEST_2_INVERT_MIN_VER                     0x0001
#define SPM_CORE_TEST_2_PREPARE_TEST_SCENARIO_MIN_VER      0x0001
#define SPM_CORE_TEST_2_EXECUTE_TEST_SCENARIO_MIN_VER      0x0001

#define SPM_CORE_IRQ_TEST_1_PREPARE_TEST_SCENARIO_SID  0x0000F0A0
#define SPM_CORE_IRQ_TEST_1_EXECUTE_TEST_SCENARIO_SID  0x0000F0A1
#define SPM_CORE_IRQ_TEST_1_PREPARE_TEST_SCENARIO_MIN_VER  0x0001
#define SPM_CORE_IRQ_TEST_1_EXECUTE_TEST_SCENARIO_MIN_VER  0x0001

enum irq_test_scenario_t {
    IRQ_TEST_SCENARIO_NONE,
    IRQ_TEST_SCENARIO_1,
    IRQ_TEST_SCENARIO_2,
    IRQ_TEST_SCENARIO_3,
    IRQ_TEST_SCENARIO_4,
    IRQ_TEST_SCENARIO_5,
};

struct irq_test_execution_data_t {
    volatile int32_t timer0_triggered;
    volatile int32_t timer1_triggered;
};

/* Use lower 16 bits in return value for error code, upper 16 for line number
 * in test service
 */
#define CORE_TEST_RETURN_ERROR(x) return (((__LINE__) << 16) | x)
#define CORE_TEST_ERROR_GET_EXTRA(x) (x >> 16)
#define CORE_TEST_ERROR_GET_CODE(x) (x & 0xFFFF)

enum core_test_errno_t {
    CORE_TEST_ERRNO_TEST_NOT_SUPPORTED         = -13,
    CORE_TEST_ERRNO_SP_NOT_INITED              = -12,
    CORE_TEST_ERRNO_UNEXPECTED_CORE_BEHAVIOUR  = -11,
    CORE_TEST_ERRNO_SP_RECURSION_NOT_REJECTED  = -10,
    CORE_TEST_ERRNO_INVALID_BUFFER             = -9,
    CORE_TEST_ERRNO_SLAVE_SP_CALL_FAILURE      = -8,
    CORE_TEST_ERRNO_SLAVE_SP_BUFFER_FAILURE    = -7,
    CORE_TEST_ERRNO_FIRST_CALL_FAILED          = -6,
    CORE_TEST_ERRNO_SECOND_CALL_FAILED         = -5,
    CORE_TEST_ERRNO_PERIPHERAL_ACCESS_FAILED   = -4,
    CORE_TEST_ERRNO_TEST_FAULT                 = -3,
    CORE_TEST_ERRNO_INVALID_TEST_ID            = -2,
    CORE_TEST_ERRNO_INVALID_PARAMETER          = -1,

    CORE_TEST_ERRNO_SUCCESS                    =  0,

    CORE_TEST_ERRNO_SUCCESS_2                  =  1,

    /* Following entry is only to ensure the error code of int size */
    CORE_TEST_ERRNO_FORCE_INT_SIZE = INT_MAX
};

#ifdef __cplusplus
}
#endif

#endif /* __CORE_TEST_DEFS_H__ */
