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
#include "Driver_USART.h"
#include "target_cfg.h"

#define ASSERT_HIGH(X)  assert(X == ARM_DRIVER_OK)

/* Imports USART driver */
extern ARM_DRIVER_USART TFM_DRIVER_STDIO;

/* Struct FILE is implemented in stdio.h. Used to redirect printf to
 * TFM_DRIVER_STDIO
 */
FILE __stdout;

/* Redirects printf to TFM_DRIVER_STDIO */
__attribute__ ((weak)) int fputc(int ch, FILE *f) {
    /* Send byte to USART */
    uart_putc(ch);

    /* Return character written */
    return ch;
}

int _write(int fd, char * str, int len)
{
    for (int i = 0; i < len; i++) {
        uart_putc(str[i]);
    }
    return len;
}

void stdio_init(void)
{
    int32_t ret = ARM_DRIVER_OK;
    ret = TFM_DRIVER_STDIO.Initialize(NULL);
    ASSERT_HIGH(ret);

    ret = TFM_DRIVER_STDIO.Control(ARM_USART_MODE_ASYNCHRONOUS, 115200);
    ASSERT_HIGH(ret);
}

void uart_putc(unsigned char c)
{
    int32_t ret = ARM_DRIVER_OK;

    ret = TFM_DRIVER_STDIO.Send(&c, 1);
    ASSERT_HIGH(ret);
}

unsigned char uart_getc(void)
{
    unsigned char c = 0;
    int32_t ret = ARM_DRIVER_OK;

    ret = TFM_DRIVER_STDIO.Receive(&c, 1);
    ASSERT_HIGH(ret);

    return c;
}
