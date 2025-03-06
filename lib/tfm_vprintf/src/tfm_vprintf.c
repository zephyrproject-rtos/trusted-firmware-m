/*
 * Copyright (c) 2017-2025, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/*
 * Based on TF-A common/tf_log.c
 */

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

#include "tfm_vprintf.h"

static inline const char *get_log_prefix(uint8_t log_level)
{
    switch(log_level) {
    case LOG_LEVEL_ERROR:
        return "[ERR]";
    case LOG_LEVEL_NOTICE:
        return "[NOT]";
    case LOG_LEVEL_WARNING:
        return "[WAR]";
    case LOG_LEVEL_INFO:
        return "[INF]";
    case LOG_LEVEL_VERBOSE:
        return "[VER]";
    default:
        /* String must start with LOG_MARKER_* */
        assert(0);
        return NULL;
    }
}

static void output_char(tfm_log_output_str output_func, void *priv, char c)
{
    output_func(priv, &c, 1);
}

static void output_str(tfm_log_output_str output_func, void *priv,
                        const char *str, uint32_t len, bool calculate_length)
{
    const char *str_ptr = str;

    if (calculate_length) {
        len = 0;
        while (*str_ptr++ != '\0') {
            len++;
        }
    }

    output_func(priv, str, len);
}

static void output_val(tfm_log_output_str output_func, void *priv, uint32_t val,
                        uint16_t num_padding, bool zero_padding, uint8_t base, bool signed_specifier)
{
    uint8_t digit, chars_to_print;
    uint16_t i;
    /* uint32_t has maximum value of 4,294,967,295. Require enough space in buffer
     * for 10 digits + null terminator + '-' */
    char buf[12] = { 0 };
    char *const buf_end = &buf[sizeof(buf) - 1];
    /* Leaving space for NULL terminator */
    char *buf_ptr = buf_end - 1;
    const char pad_char = zero_padding ? '0' : ' ';
    const char negative_char = '-';
    bool negative = false;

    if (signed_specifier && ((int32_t)val < 0)) {
        val = -val;
        negative = true;
    }

    do {
        digit = val % base;

        if (digit < 10) {
            *buf_ptr-- = '0' + digit;
        } else {
            *buf_ptr-- = 'a' + digit - 10;
        }

        val /= base;
    } while (val);

    if (negative) {
        if (!zero_padding) {
            *buf_ptr-- = negative_char;
        } else {
            output_char(output_func, priv, negative_char);
        }
    }

    chars_to_print = (buf_end - 1) - buf_ptr;
    if (num_padding > chars_to_print) {
        num_padding -= chars_to_print;
    } else {
        num_padding = 0;
    }

    for (i = 0; i < num_padding; i++) {
        output_char(output_func, priv, pad_char);
    }

    output_str(output_func, priv, buf_ptr + 1, chars_to_print, false);
}

/* Basic vprintf, understands:
 * %s: output string
 * %u: output uint32_t in decimal
 * %d: output int32_t in decimal
 * %x: output uint32_t in hex
 */
static void tfm_vprintf_internal(tfm_log_output_str output_func,
                                void *priv, const char *fmt, va_list args)
{
    char c;
    bool formatting = false;
    uint16_t num_padding = 0;
    bool zero_padding = false;

    while ((c = *fmt++) != '\0') {
        if (!formatting) {
            if (c == '%') {
                zero_padding = false;
                num_padding = 0;
                formatting = true;
            } else {
                if (c == '\n') {
                    output_char(output_func, priv, '\r');
                }
                output_char(output_func, priv, c);
            }
            continue;
        }

        switch (c) {
        case 'l':
            continue;
        case 'u':
            output_val(output_func, priv, va_arg(args, uint32_t), num_padding, zero_padding, 10, false);
            break;
        case 'd':
            output_val(output_func, priv, va_arg(args, uint32_t), num_padding, zero_padding, 10, true);
            break;
        case 'x':
            output_val(output_func, priv, va_arg(args, uint32_t), num_padding, zero_padding, 16, false);
            break;
        case 's':
            output_str(output_func, priv, va_arg(args, char *), 0, true);
            break;
        case '0':
            if (num_padding == 0) {
                zero_padding = true;
                continue;
            }
            /* fallthrough */
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            num_padding *= 10;
            num_padding += c - '0';
            continue;
        case '%':
            output_char(output_func, priv, '%');
            break;
        default:
            output_str(output_func, priv, "[Unsupported]", 0, true);
        }

        formatting = false;
    }
}

void tfm_vprintf(tfm_log_output_str output_func, void *priv, const char *fmt, va_list args)
{
    uint8_t log_level;
    const char spacer = ' ';

    /* We expect the LOG_MARKER_* macro as the first character */
    log_level = fmt[0];
    fmt++;

    output_str(output_func, priv, get_log_prefix(log_level), 0, true);
    output_char(output_func, priv, spacer);

    tfm_vprintf_internal(output_func, priv, fmt, args);
}
