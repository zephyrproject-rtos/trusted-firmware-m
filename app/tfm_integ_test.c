/*
 * Copyright (c) 2017 - 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "cmsis.h"
#include "tfm_api.h"
#include "cmsis_os2.h"

#include "tfm_integ_test.h"
#include "test/framework/integ_test.h"

#ifdef CORE_TEST_INTERACTIVE
#include "test/test_services/tfm_core_test/core_test_defs.h"
#include "test/test_services/tfm_core_test/tfm_ss_core_test_veneers.h"
#include "tfm_ns_svc.h"

#define TRY_SFN(fn, ...) \
    do { \
        enum tfm_status_e res = (enum tfm_status_e) fn(__VA_ARGS__); \
        switch(res) { \
            case TFM_SUCCESS: \
                LOG_MSG("Secure call to " #fn "(" #__VA_ARGS__") successful!");\
                break; \
            case TFM_SERVICE_PENDED: \
                LOG_MSG("Secure call to " #fn "(" #__VA_ARGS__") pended!"); \
                break; \
            case TFM_ERROR_SERVICE_ALREADY_PENDED: \
                LOG_MSG("Secure call to " #fn "(" #__VA_ARGS__") failed, " \
                                                            "already pended!");\
                break; \
            case TFM_ERROR_SECURE_DOMAIN_LOCKED: \
                LOG_MSG("Secure call to " #fn "(" #__VA_ARGS__") failed, " \
                                                           "S domain locked!");\
                break; \
            case TFM_ERROR_NS_THREAD_MODE_CALL: \
                LOG_MSG("Secure call to " #fn "(" #__VA_ARGS__") failed, " \
                                                            "NS thread mode!");\
                break; \
            default: \
                LOG_MSG("Secure call to " #fn "(" #__VA_ARGS__") failed, " \
                                                                   "generic!");\
        } \
    } while(0)
/**
 * \brief SVC_SECURE_DECREMENT_NS_LOCK_1
 *
 */
void svc_secure_decrement_ns_lock_1(void)
{
    TRY_SFN(tfm_core_test_sfn, CORE_TEST_ID_BLOCK, 0x1, 0x1, 0x1);
}

/**
 * \brief SVC_SECURE_DECREMENT_NS_LOCK_2
 *
 */
void svc_secure_decrement_ns_lock_2(void)
{
    TRY_SFN(tfm_core_test_sfn, CORE_TEST_ID_BLOCK, 0x2, 0x2, 0x2);
}
/**
 * \brief Test definition for the RTX - TFM integration tests
 *        scenarios
 */
enum test_type {
    TEST_TYPE_1 = 1, /*!< Sequential test: single task using the NS lock to
                          access TFM */
    TEST_TYPE_2,     /*!< Priority test: high priority tries to preempt TFM,
                          gets delayed */
    TEST_TYPE_3,     /*!< Priority inversion: classical scenario with high
                          priority task waiting on lower priority task
                          undefinitely if NS lock is configured without priority
                          inheritance */
    TEST_TYPE_4,     /*!< non-NS lock: like sequential, but doesn't use any NS
                          lock mechanism */
    TEST_TYPE_5,     /*!< non-NS lock, core locked: high priority tries to
                          overcome the NS lock but finds TFM core locked by
                          lower priority task and fails */
};

static const osThreadAttr_t tattr_seq = {
    .name = "seq_task",
    .stack_size = 1024U,
    .attr_bits = osThreadJoinable,
    .tz_module = 1,
};
static const osThreadAttr_t tattr_mid = {
    .name = "mid_task",
    .stack_size = 512U,
    .attr_bits = osThreadJoinable,
    .tz_module = 0,
    .priority = osPriorityAboveNormal
};
static const osThreadAttr_t tattr_pri = {
    .name = "pri_task",
    .stack_size = 1024U,
    .attr_bits = osThreadJoinable,
    .tz_module = 1,
    .priority = osPriorityHigh
};

/**
 * \brief Mutex id, NS lock
 */
static osMutexId_t  mutex_id;

/**
 * \brief Mutex properties, NS lock
 */
static const osMutexAttr_t mattr_ns_lock = {
    .name = "ns_lock",
    //.attr_bits = osMutexPrioInherit
};

/**
 * \brief SVC dispatcher
 */
__attribute__((always_inline)) __STATIC_INLINE
void svc_dispatch(enum tfm_svc_num svc_num)
{
    switch (svc_num) {
    case SVC_SECURE_DECREMENT_NS_LOCK_1:
        SVC(SVC_SECURE_DECREMENT_NS_LOCK_1);
        break;
    case SVC_SECURE_DECREMENT_NS_LOCK_2:
        SVC(SVC_SECURE_DECREMENT_NS_LOCK_2);
        break;
    default:
        break;
    }
}

/**
 * \brief tfm_service_request
 *
 * \details This function is used to request a TFM
 *          service in handler mode, using SVC.
 *          Optionally uses the NS lock
 */
static void tfm_service_request(enum tfm_svc_num svc_num, bool use_ns_lock)
{
    osStatus_t result;

    char buffer[80];

#define LOG_MSG_THREAD(MSG_THREAD) \
  do { \
      sprintf(buffer,"%s [%s]", MSG_THREAD, osThreadGetName(osThreadGetId())); \
      LOG_MSG(buffer); \
  } \
  while(0)

    LOG_MSG_THREAD("Trying to acquire the TFM core from NS");

    if (use_ns_lock) {
        result = osMutexAcquire(mutex_id,0);
        if (result == osOK) {
            LOG_MSG_THREAD("NS Lock: acquired");
            svc_dispatch(svc_num);
            LOG_MSG_THREAD("NS Lock: releasing...");
            osMutexRelease(mutex_id);
        } else {
            LOG_MSG_THREAD("Failed to acquire the NS lock");

            osMutexAcquire(mutex_id,osWaitForever);
            LOG_MSG_THREAD("NS Lock: acquired");
            svc_dispatch(svc_num);
            LOG_MSG_THREAD("NS Lock: releasing...");
            osMutexRelease(mutex_id);
        }
    } else {
        svc_dispatch(svc_num);
    }
}

/**
 * \brief Non-blocking test thread
 *
 */
__attribute__((noreturn))
static void mid_task(void *argument)
{
    osThreadId_t thread_id_pri;
    osThreadState_t thread_pri_state;
    uint32_t idx;

    thread_id_pri = *((osThreadId_t *)argument);

    /* go to sleep */
    osDelay(100U);

    thread_pri_state = osThreadGetState(thread_id_pri);

    if (thread_pri_state == osThreadBlocked) {
        LOG_MSG("Running [mid_task] while [pri_task] is blocked");
    } else if (thread_pri_state == osThreadTerminated) {
        LOG_MSG("Running [mid_task] while [pri_task] is terminated");
    } else {
        LOG_MSG("Running [mid_task]");
    }

    /* Do non TFM related, non blocking, operations */
    for (idx=0; idx<0x3ffffff; idx++) {
    }

    LOG_MSG("Exiting [mid_task]");

    osThreadExit();
}

/**
 * \brief Priority test thread
 *
 */
__attribute__((noreturn))
static void pri_task(void *argument)
{
    /* go to sleep */
    osDelay(100U);

    /* After wake up, try to get hold of the NS lock */
    tfm_service_request(SVC_SECURE_DECREMENT_NS_LOCK_2, *((bool *)argument));

    osThreadExit();
}

/**
 * \brief Sequential test thread
 *
 */
__attribute__((noreturn))
static void seq_task(void *argument)
{
    osThreadId_t thread_id, thread_id_mid;
    bool use_ns_lock, use_ns_lock_pri;
    enum test_type test_type;

    test_type = *((enum test_type *)argument);

    if (test_type == TEST_TYPE_1) {
        LOG_MSG("Scenario 1 - Sequential");
        use_ns_lock = true;
    } else if (test_type == TEST_TYPE_2) {
        LOG_MSG("Scenario 2 - Priority");
        use_ns_lock = true;
        use_ns_lock_pri = true;
        thread_id = osThreadNew(pri_task, &use_ns_lock_pri, &tattr_pri);
    } else if (test_type == TEST_TYPE_3) {
        LOG_MSG("Scenario 3 - Priority inversion");
        use_ns_lock = true;
        use_ns_lock_pri = true;
        thread_id = osThreadNew(pri_task, &use_ns_lock_pri, &tattr_pri);
        thread_id_mid = osThreadNew(mid_task, &thread_id, &tattr_mid);
    } else if (test_type == TEST_TYPE_4) {
        LOG_MSG("Scenario 4 - non-NS lock");
        use_ns_lock = false;
    } else if (test_type == TEST_TYPE_5) {
        LOG_MSG("Scenario 5 - non-NS lock, core locked");
        use_ns_lock = true;
        use_ns_lock_pri = false;
        thread_id = osThreadNew(pri_task, &use_ns_lock_pri, &tattr_pri);
    } else {
        LOG_MSG("Scenario not supported");
        osThreadExit();
    }

    /* Try to acquire the NS lock */
    tfm_service_request(SVC_SECURE_DECREMENT_NS_LOCK_1, use_ns_lock);

    if (test_type == TEST_TYPE_1) {
        LOG_MSG("Scenario 1 - test finished\n");
    } else if (test_type == TEST_TYPE_2) {
        osThreadJoin(thread_id);
        LOG_MSG("Scenario 2 - test finished\n");
    } else if (test_type == TEST_TYPE_3) {
        osThreadJoin(thread_id);
        osThreadJoin(thread_id_mid);
        LOG_MSG("Scenario 3 - test finished\n");
    } else if (test_type == TEST_TYPE_4) {
        LOG_MSG("Scenario 4 - test finished\n");
    } else if (test_type == TEST_TYPE_5) {
        osThreadJoin(thread_id);
        LOG_MSG("Scenario 5 - test finished\n");
    }

    osThreadExit();
}

/**
 * \brief Execute the interactive tets cases
 *
 */
void execute_ns_interactive_tests(void)
{
    uint8_t idx;

    osThreadId_t thread_id;

    /* Test type list */
    enum test_type test_type[] = {TEST_TYPE_1, TEST_TYPE_2, TEST_TYPE_3,
                                  TEST_TYPE_4, TEST_TYPE_5};

    /* Create the NS lock -- shared among testing scenarios */
    mutex_id = osMutexNew(&mattr_ns_lock);

    /* Loop in the test list */
    for (idx=0; idx<sizeof(test_type); idx++) {
        /* Spawn the main thread */
        thread_id = osThreadNew(seq_task, &test_type[idx], &tattr_seq);

        /* Wait for it to finish before moving to the next scenario */
        osThreadJoin(thread_id);
    }
}
#endif /* CORE_TEST_INTERACTIVE */

#ifdef TEST_FRAMEWORK_NS
/**
 * \brief Services test thread
 *
 */
__attribute__((noreturn))
void test_app(void *argument)
{
    UNUSED_VARIABLE(argument);
    start_integ_test();
    /* End of test */
    for (;;) {
    }
}
#endif /* TEST_FRAMEWORK_NS */
