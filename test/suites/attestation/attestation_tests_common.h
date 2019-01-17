/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __ATTESTATION_TESTS_COMMON_H__
#define __ATTESTATION_TESTS_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \def TEST_TOKEN_SIZE
 *
 * \brief Size of token buffer in bytes.
 */
#define TEST_TOKEN_SIZE (0x200)

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
#define TEST_CHALLENGE_OBJ_SIZE (32u)

/*!
 * \def INVALID_CHALLENGE_OBJECT_SIZE
 *
 * \brief Size of challenge object that is
 */
#define INVALID_CHALLENGE_OBJECT_SIZE (PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 + 1)

/*!
 * \def CHALLENGE_FOR_TEST
 *
 * \brief Fix value used as challenge object, because currently random number
 *        generation is not supported on secure side.
 */
#define CHALLENGE_FOR_TEST    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, \
                              0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, \
                              0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, \
                              0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xEE, 0xEE, 0xEF,

#ifdef __cplusplus
}
#endif

#endif /* __ATTESTATION_TESTS_COMMON_H__ */
