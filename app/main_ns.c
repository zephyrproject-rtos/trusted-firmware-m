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
#include "tfm_ns_lock.h"
#ifdef TEST_FRAMEWORK_NS
#include "test/framework/integ_test.h"
#endif

#include "target_cfg.h"
#include "Driver_USART.h"

/* For UART the CMSIS driver is used */
extern ARM_DRIVER_USART NS_DRIVER_STDIO;

/* Struct FILE is implemented in stdio.h. Used to redirect printf to
 * NS_DRIVER_STDIO
 */
FILE __stdout;
/* Redirects armclang printf to NS_DRIVER_STDIO */
int fputc(int ch, FILE *f) {
    /* Send byte to NS_DRIVER_STDIO */
    (void)NS_DRIVER_STDIO.Send((const unsigned char *)&ch, 1);
    /* Return character written */
    return ch;
}
/* redirects gcc printf to NS_DRIVER_STDIO */
int _write(int fd, char * str, int len)
{
    (void)NS_DRIVER_STDIO.Send(str, len);

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
    (void)NS_DRIVER_STDIO.Initialize(NULL);
    NS_DRIVER_STDIO.Control(ARM_USART_MODE_ASYNCHRONOUS, 115200);

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
