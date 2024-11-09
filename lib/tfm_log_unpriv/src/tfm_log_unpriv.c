/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

#include "tfm_log_unpriv.h"
#include "tfm_hal_sp_logdev.h"

#define LOG_UNPRIV_BUFFER_SIZE (32)

struct tfm_log_unpriv_data {
    uint8_t buf_pos;
    char buf[LOG_UNPRIV_BUFFER_SIZE];
};

static void output_buf(const char *buf, uint32_t buf_len)
{
    tfm_hal_output_sp_log(buf, buf_len);
}

static void output_string_to_buf(void *priv, const char *str, uint32_t len)
{
    struct tfm_log_unpriv_data *data = priv;
    uint32_t i;

    if (len > LOG_UNPRIV_BUFFER_SIZE) {
        /* Not enough space for str */
        return;
    } else if ((data->buf_pos + len) > LOG_UNPRIV_BUFFER_SIZE) {
        /* Flush current buffer and re-use */
        output_buf(data->buf, data->buf_pos);
        data->buf_pos = 0;
    }

    for (i = 0; i < len; i++) {
        data->buf[data->buf_pos++] = *str++;
    }
}

void tfm_log_unpriv(const char *fmt, ...)
{
    va_list args;
    struct tfm_log_unpriv_data data;

    data.buf_pos = 0;

    va_start(args, fmt);
    tfm_vprintf(output_string_to_buf, &data, fmt, args);
    va_end(args);

    output_buf(data.buf, data.buf_pos);
}
