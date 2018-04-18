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
#include "tfm_ns_lock.h"
#ifdef TEST_FRAMEWORK_NS
#include "test/framework/integ_test.h"
#endif
#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
#include \
  "test/test_services/tfm_secure_client_service/tfm_secure_client_service_svc.h"
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

#define X(SVC_ENUM, SVC_HANDLER) (void*)SVC_HANDLER,
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
#ifndef __GNUC__
__attribute__((noreturn))
#endif
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
