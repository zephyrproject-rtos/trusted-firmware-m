/*
 * Copyright (c) 2017-2018 ARM Limited
 *
 * Licensed under the Apace License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apace.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "uart_stdout.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#ifdef TARGET_MUSCA_A
#include "uart_pl011_drv.h"
#else
#include "arm_uart_drv.h"
#endif
#include "Driver_USART.h"

#define ASSERT_HIGH(X)  assert(X == ARM_DRIVER_OK)

/* Imports USART driver */
extern ARM_DRIVER_USART Driver_USART0;
extern ARM_DRIVER_USART Driver_USART1;

/* Struct FILE is implemented in stdio.h. Used to redirect printf to UART */
FILE __stdout;

/* Redirects printf to UART */
__attribute__ ((weak)) int fputc(int ch, FILE *f) {
    /* Send byte to USART */
    uart_putc(ch);

    /* Return character written */
    return ch;
}

int _write(int fd, char * str, int len)
{
    for (int i = 0; i < len; i++)
    {
        uart_putc(str[i]);
    }
    return len;
}

#ifdef TARGET_MUSCA_A
extern struct uart_pl011_dev_t UART0_DEV_S, UART0_DEV_NS;
extern struct uart_pl011_dev_t UART1_DEV_S, UART1_DEV_NS;
#else
extern struct arm_uart_dev_t ARM_UART0_DEV_S, ARM_UART0_DEV_NS;
extern struct arm_uart_dev_t ARM_UART1_DEV_S, ARM_UART1_DEV_NS;
#endif

/* Generic driver to be configured and used */
ARM_DRIVER_USART *Driver_USART = NULL;

void uart_init(enum uart_channel uchan)
{
    int32_t ret = ARM_DRIVER_OK;

    /* Add a configuration step for the UART channel to use, 0 or 1 */
    switch(uchan) {
    case UART0_CHANNEL:
        /* UART0 is configured as a non-secure peripheral, so it cannot be
         * accessed using its secure alias. Ideally the driver would
         * be configured with the right properties, but for simplicity,
         * use a workaround for now
         */
#ifdef TARGET_MUSCA_A
        memcpy(&UART0_DEV_S, &UART0_DEV_NS, sizeof(struct uart_pl011_dev_t));
#else
        memcpy(&ARM_UART0_DEV_S, &ARM_UART0_DEV_NS,
                                                sizeof(struct arm_uart_dev_t));
#endif
        Driver_USART = &Driver_USART0;
        break;
    case UART1_CHANNEL:
#ifndef SECURE_UART1
        /* If UART1 is configured as a non-secure peripheral, it cannot be
         * accessed using its secure alias. Ideally the driver would
         * be configured with the right properties, but for simplicity,
         * use a workaround for now
         */
#ifdef TARGET_MUSCA_A
        memcpy(&UART1_DEV_S, &UART1_DEV_NS, sizeof(struct uart_pl011_dev_t));
#else
        memcpy(&ARM_UART1_DEV_S, &ARM_UART1_DEV_NS,
                                                sizeof(struct arm_uart_dev_t));
#endif
#endif
        Driver_USART = &Driver_USART1;
        break;
    default:
        ret = ARM_DRIVER_ERROR;
    }
    ASSERT_HIGH(ret);

    ret = Driver_USART->Initialize(NULL);
    ASSERT_HIGH(ret);

    ret = Driver_USART->Control(ARM_USART_MODE_ASYNCHRONOUS, 115200);
    ASSERT_HIGH(ret);
}

void uart_putc(unsigned char c)
{
    int32_t ret = ARM_DRIVER_OK;

    ret = Driver_USART->Send(&c, 1);
    ASSERT_HIGH(ret);
}

unsigned char uart_getc(void)
{
    unsigned char c = 0;
    int32_t ret = ARM_DRIVER_OK;

    ret = Driver_USART->Receive(&c, 1);
    ASSERT_HIGH(ret);

    return c;
}
