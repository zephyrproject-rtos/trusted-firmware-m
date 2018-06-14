/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <cmsis_compiler.h>

#ifndef __TFM_NS_SVC_H__
#define __TFM_NS_SVC_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Include all the SVC handler headers
 */
#include "tfm_sst_svc_handler.h"
#include "tfm_log_svc_handler.h"
#include "svc_core_test_ns.h"
#include "sst_test_service_svc.h"

/**
 * \brief Macro to encode an svc instruction
 *
 */
#define SVC(code) __ASM("svc %0" : : "I" (code))

/**
 * \def LIST_SVC_DISPATCHERS
 *
 * \brief This is an X macro which lists
 *        the SVC interface exposed by the
 *        available secure services. The
 *        enumerator and corresponding
 *        SVC handler function need to be
 *        registered.
 *
 */
#define LIST_SVC_DISPATCHERS \
    X(SVC_TFM_SST_GET_HANDLE, tfm_sst_svc_get_handle) \
    X(SVC_TFM_SST_CREATE, tfm_sst_svc_create) \
    X(SVC_TFM_SST_GET_INFO, tfm_sst_svc_get_info) \
    X(SVC_TFM_SST_GET_ATTRIBUTES, tfm_sst_svc_get_attributes) \
    X(SVC_TFM_SST_SET_ATTRIBUTES, tfm_sst_svc_set_attributes) \
    X(SVC_TFM_SST_READ, tfm_sst_svc_read) \
    X(SVC_TFM_SST_WRITE, tfm_sst_svc_write) \
    X(SVC_TFM_SST_DELETE, tfm_sst_svc_delete) \
    X(SVC_TFM_LOG_RETRIEVE, tfm_log_svc_retrieve) \
    X(SVC_TFM_LOG_GET_INFO, tfm_log_svc_get_info) \
    X(SVC_TFM_LOG_DELETE_ITEMS, tfm_log_svc_delete_items)

/**
 * \def LIST_SVC_CORE_TEST_INTERACTIVE
 *
 * \brief This is an X macro which lists
 *        the SVC interface available for
 *        the CORE_TEST_INTERACTIVE. The
 *        enumerator and corresponding
 *        SVC handler function need to be
 *        registered.
 *
 */
#define LIST_SVC_CORE_TEST_INTERACTIVE \
    X(SVC_SECURE_DECREMENT_NS_LOCK_1, svc_secure_decrement_ns_lock_1) \
    X(SVC_SECURE_DECREMENT_NS_LOCK_2, svc_secure_decrement_ns_lock_2)

/**
 * \def LIST_SVC_TFM_PARTITION_TEST_CORE
 *
 * \brief This is an X macro which lists
 *        the SVC interface available for
 *        the TEST_CORE partition. The
 *        enumerator and corresponding
 *        SVC handler function need to be
 *        registered.
 *
 */
#define LIST_SVC_TFM_PARTITION_TEST_CORE \
    X(SVC_TFM_CORE_TEST, svc_tfm_core_test) \
    X(SVC_TFM_CORE_TEST_MULTIPLE_CALLS, svc_tfm_core_test_multiple_calls)

/**
 * \def LIST_SVC_TFM_PARTITION_TEST_SST
 *
 * \brief This is an X macro which lists
 *        the SVC interface available for
 *        TEST_SST partition. The
 *        enumerator and corresponding
 *        SVC handler function need to be
 *        registered.
 *
 */
#define LIST_SVC_TFM_PARTITION_TEST_SST \
    X(SVC_SST_TEST_SERVICE_SETUP, sst_test_service_svc_setup) \
    X(SVC_SST_TEST_SERVICE_DUMMY_ENCRYPT, sst_test_service_svc_dummy_encrypt) \
    X(SVC_SST_TEST_SERVICE_DUMMY_DECRYPT, sst_test_service_svc_dummy_decrypt) \
    X(SVC_SST_TEST_SERVICE_CLEAN, sst_test_service_svc_clean)

/**
 * \def LIST_SVC_TFM_PARTITION_TEST_SECURE_SERVICES
 *
 * \brief This is an X macro which lists
 *        the SVC interface available for
 *        TEST_SECURE_SERVICES partition.
 *        The enumerator and corresponding
 *        SVC handler function need to be
 *        registered.
 *
 */
#define LIST_SVC_TFM_PARTITION_TEST_SECURE_SERVICES \
    X(SVC_TFM_SECURE_CLIENT_RUN_TESTS, tfm_secure_client_service_svc_run_tests)

/**
 * \brief Numbers associated to each SVC available
 *
 * \details Start from 1 as 0 is reserved by RTX
 */
enum tfm_svc_num {
    SVC_INVALID = 0,

#define X(SVC_ENUM, SVC_HANDLER) SVC_ENUM,
    /* SVC API for Services */
    LIST_SVC_DISPATCHERS

#if defined(CORE_TEST_INTERACTIVE)
    LIST_SVC_CORE_TEST_INTERACTIVE
#endif /* CORE_TEST_INTERACTIVE */

#if defined(TFM_PARTITION_TEST_CORE)
    LIST_SVC_TFM_PARTITION_TEST_CORE
#endif /* TFM_PARTITION_TEST_CORE */

#if defined(TFM_PARTITION_TEST_SST)
    LIST_SVC_TFM_PARTITION_TEST_SST
#endif /* TFM_PARTITION_TEST_SST */

#if defined(TFM_PARTITION_TEST_SECURE_SERVICES)
    LIST_SVC_TFM_PARTITION_TEST_SECURE_SERVICES
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#undef X

    /* add all the new entries above this line */
    SVC_TFM_MAX,
};

/* number of user SVC functions */
#define USER_SVC_COUNT (SVC_TFM_MAX - 1)

#ifdef __cplusplus
}
#endif

#endif /* __TFM_NS_SVC_H__ */
