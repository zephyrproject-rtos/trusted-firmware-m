/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <unwind.h>
#include <stdint.h>
#include <stddef.h>

#include "backtrace.h"
#include "tfm_vprintf.h"

#define DUMP_BACKTRACE_BUF_SIZE (10)

#define LOG_PREFIX_CHAR LOG_MARKER_ERROR

struct trace_priv_data {
    uint32_t *buf;
    uint8_t buf_size;
    uint32_t prev_ip;
    uint8_t cur_frame;
};

static _Unwind_Reason_Code trace_func(_Unwind_Context *context, void *priv)
{
    struct trace_priv_data *priv_data = priv;
    uint32_t cur_ip;

    if (priv_data->cur_frame >= priv_data->buf_size) {
        return _URC_END_OF_STACK;
    }

    cur_ip = _Unwind_GetIP(context);
    if (cur_ip == priv_data->prev_ip) {
        return _URC_END_OF_STACK;
    }

    /* Branch instruction 4 bytes before return address */
    priv_data->buf[priv_data->cur_frame] = cur_ip - 4;

    priv_data->prev_ip = cur_ip;
    priv_data->cur_frame++;

    return _URC_OK;
}

static void get_backtrace(uint32_t *buf, size_t buf_size, uint8_t *num_frames)
{
    struct trace_priv_data priv_data = {
        .buf = buf,
        .buf_size = buf_size,
        .prev_ip = 0,
        .cur_frame = 0
    };

    _Unwind_Backtrace(trace_func, &priv_data);

    *num_frames = priv_data.cur_frame;
}

void tfm_dump_backtrace(const char *info, tfm_log_func log_func)
{
    uint32_t buf[DUMP_BACKTRACE_BUF_SIZE];
    uint8_t num_frames;

    get_backtrace(buf, DUMP_BACKTRACE_BUF_SIZE, &num_frames);

    log_func(LOG_PREFIX_CHAR "BACKTRACE: START: %s\n", info);

    for (uint8_t i = 0; i < num_frames; i++) {
        log_func(LOG_PREFIX_CHAR "%lx: 0x%lx\n", (uint32_t)i, buf[i]);
    }

    log_func(LOG_PREFIX_CHAR "BACKTRACE: END: %s\n", info);
}

void tfm_get_backtrace(uint32_t *buf, uint8_t buf_size, uint8_t *num_frames)
{
    get_backtrace(buf, buf_size, num_frames);
}
