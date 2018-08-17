/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __ATTESTATION_TESTS_COMMON_H__
#define __ATTESTATION_TESTS_COMMON_H__

#include "psa_initial_attestation_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \def TEST_TOKEN_SIZE
 *
 * \brief Size of token buffer in bytes.
 */
#define TEST_TOKEN_SIZE PSA_INITIAL_ATTEST_TOKEN_SIZE

/*!
 * \def TOO_SMALL_TOKEN_BUFFER
 *
 * \brief Smaller buffer size which is not big enough to store the created token
 *        by attestation service.
 */
#define TOO_SMALL_TOKEN_BUFFER (16u)

/*!
 * \def TEST_CHALLENGE_OBJ_SIZE
 *
 * \brief Size of challenge object in bytes used for test.
 */
#define TEST_CHALLENGE_OBJ_SIZE (16u)

/*!
 * \def TOO_BIG_CHALLENGE_OBJECT
 *
 * \brief Size of challenge object that is bigger than it is allowed.
 */
#define TOO_BIG_CHALLENGE_OBJECT (PSA_INITIAL_ATTEST_MAX_CHALLENGE_SIZE + 1)

/*!
 * \def CHALLENGE_FOR_TEST
 *
 * \brief Fix value used as challenge object, because currently random number
 *        generation is not supported on secure side.
 */
#define CHALLENGE_FOR_TEST    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, \
                              0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF



#ifdef __cplusplus
}
#endif

#endif /* __ATTESTATION_TESTS_COMMON_H__ */
