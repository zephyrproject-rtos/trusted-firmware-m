/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "cmsis.h"
#include "tfm_api.h"
#include "cmsis_os2.h"
#include "tfm_integ_test.h"
#include "tfm_ns_svc.h"
#include "tfm_sst_svc_handler.h"
#include "tfm_ns_lock.h"
#ifdef TFM_PARTITION_TEST_CORE
#include "svc_core_test_ns.h"
#endif
#ifdef TFM_PARTITION_TEST_SST
#include "sst_test_service_svc.h"
#endif
#ifdef TEST_FRAMEWORK_NS
#include "test/framework/integ_test.h"
#endif

#include "Driver_USART.h"

/* For UART the CMSIS driver is used */
extern ARM_DRIVER_USART Driver_USART0;

/**
 * \brief Modified table template for user defined SVC functions
 *
 * \details RTX has a weak definition of osRtxUserSVC, which
 *          is overridden here
 */
extern void * const osRtxUserSVC[1+USER_SVC_COUNT];
       void * const osRtxUserSVC[1+USER_SVC_COUNT] = {
  (void *)USER_SVC_COUNT,

/* SERVICES_TEST_NS */
  (void *)tfm_sst_svc_get_handle,
  (void *)tfm_sst_svc_create,
  (void *)tfm_sst_svc_get_attributes,
  (void *)tfm_sst_svc_read,
  (void *)tfm_sst_svc_write,
  (void *)tfm_sst_svc_delete,

#if defined(CORE_TEST_INTERACTIVE)
  (void *)svc_secure_decrement_ns_lock_1,
  (void *)svc_secure_decrement_ns_lock_2,
#endif /* CORE_TEST_INTERACTIVE */

#if defined(TFM_PARTITION_TEST_CORE)
  (void *)svc_tfm_core_test,
  (void *)svc_tfm_core_test_multiple_calls,
#endif /* TFM_PARTITION_TEST_CORE */

#if defined(TFM_PARTITION_TEST_SST)
  (void *)sst_test_service_svc_setup,
  (void *)sst_test_service_svc_dummy_encrypt,
  (void *)sst_test_service_svc_dummy_decrypt,
  (void *)sst_test_service_svc_clean,
#endif /* TFM_PARTITION_TEST_SST */

//(void *)user_function1,
// ...
};

/* Struct FILE is implemented in stdio.h. Used to redirect printf to UART0 */
FILE __stdout;
/* Redirects armclang printf to UART */
int fputc(int ch, FILE *f) {
    /* Send byte to UART0 */
    (void)Driver_USART0.Send((const unsigned char *)&ch, 1);
    /* Return character written */
    return ch;
}
/* redirects gcc printf to uart */
int _write(int fd, char * str, int len)
{
    (void)Driver_USART0.Send(str, len);

    return len;
}

/**
 * \brief List of RTOS thread attributes
 */
#ifdef TEST_FRAMEWORK_NS
static const osThreadAttr_t tserv_test = {
    .name = "test_app",
    .stack_size = 1024U
};
#endif

/**
 * \brief Static globals to hold RTOS related quantities,
 *        main thread
 */
static osStatus_t   status;
static osThreadId_t thread_id;

/**
 * \brief main() function
 */
__attribute__((noreturn))
int main(void)
{
    (void)Driver_USART0.Initialize(NULL); /* Use UART0 as stdout */
    Driver_USART0.Control(ARM_USART_MODE_ASYNCHRONOUS, 115200);

    status = osKernelInitialize();

    /* Initialize the TFM NS lock */
    tfm_ns_lock_init();

#ifdef TEST_FRAMEWORK_NS
    thread_id = osThreadNew(test_app, NULL, &tserv_test);
#else
    UNUSED_VARIABLE(thread_id);
#endif

    status = osKernelStart();

    /* Reached only in case of error */
    for (;;) {
    }
}
