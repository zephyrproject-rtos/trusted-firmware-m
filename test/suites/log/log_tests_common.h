/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __LOG_TESTS_COMMON_H__
#define __LOG_TESTS_COMMON_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \def STR(a)
 *
 * \brief A standard stringify macro
 */
#define STR(a) _STR(a)
#define _STR(a) #a

/*!
 * \def LOCAL_BUFFER_SIZE
 *
 * \brief Size in bytes of the local buffer. Size accomodates two standard size
 *        (no payload) log items, at maximum
 */
#define LOCAL_BUFFER_SIZE (100)

/*!
 * \def LOCAL_BUFFER_ITEMS
 *
 * \brief Number of items which can be held within a buffer of size
 *        LOCAL_BUFFER_SIZE
 */
#define LOCAL_BUFFER_ITEMS (2)

/*!
 * \def STANDARD_LOG_ENTRY_SIZE
 *
 * \brief A log item with no payload (standard size) has the following size.
 *        More details can be found observing \ref tfm_log_line
 *        \ref tfm_log_tlr and \ref tfm_log_hdr
 */
#define STANDARD_LOG_ENTRY_SIZE (44)

/*!
 * \def INITIAL_LOGGING_REQUESTS
 *
 * \brief Number of initial consecutive logging requests to perform
 */
#define INITIAL_LOGGING_REQUESTS (23)

/*!
 * \def INITIAL_LOGGING_SIZE
 *
 * \brief Size of the initial consecutive logging requests
 */
#define INITIAL_LOGGING_SIZE (1012)

/*!
 * \def FINAL_LOGGING_REQUESTS
 *
 * \brief Number of final consecutive logging requests to perform
 *
 * \note This defines the state of the log when secure interface tests are
 *       terminated
 */
#define FINAL_LOGGING_REQUESTS (2)

/*!
 * \def FINAL_LOGGING_SIZE
 *
 * \brief Size of the final consecutive logging requests
 *
 * \note This defines the state of the log when secure interface tests are
 *       terminated
 */
#define FINAL_LOGGING_SIZE (88)

/*!
 * \def SECOND_ELEMENT_EXPECTED_CONTENT
 *
 * \brief Content of the log line in the second log item in the final request.
 *        In particular this is the value of the first argument which has been
 *        stored in the last addition from the secure test suite
 */
#define SECOND_ELEMENT_EXPECTED_CONTENT ( 1 + \
                      (INITIAL_LOGGING_REQUESTS+1+FINAL_LOGGING_REQUESTS)*10 )
/*!
 * \def MAX_LOG_SIZE
 *
 * \brief The maximum possible log size in the current implementation
 *
 * \brief This parameter for tests has to be changed for the tests in case the
 *        implementation is modified
 */
#define MAX_LOG_SIZE (1024)

/*!
 * \def MAX_LOG_LINE_SIZE
 *
 * \brief The maximum possible log line size to fill a MAX_LOG_SIZE bytes log
 *
 * \note This takes into account additional fields that are concatenated to the
 *       log line in the header and trailer
 */
#define MAX_LOG_LINE_SIZE (1000)

/*!
 * \def INITIAL_LOG_SIZE
 *
 * \brief Initial state of the log size in bytes
 *
 * \note This defines the state of the log when non-secure interface tests start
 */
#define INITIAL_LOG_SIZE (FINAL_LOGGING_SIZE)

/*!
 * \def INITIAL_LOG_ITEMS
 *
 * \brief Initial state of the log number of items
 *
 * \note This defines the state of the log when non-secure interface tests start
 */
#define INITIAL_LOG_ITEMS (FINAL_LOGGING_REQUESTS)

#ifdef __cplusplus
}
#endif

#endif /* __LOG_TESTS_COMMON_H__ */
