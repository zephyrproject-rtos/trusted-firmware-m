/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "fatal_error.h"

#include "tfm_hal_device_header.h"

#include <stdio.h>

extern uint32_t stdio_is_initialized;

__WEAK bool log_error_permissions_check(uint32_t err, bool is_fatal)
{
    return true;
}

__WEAK void log_error(char *file, uint32_t line, uint32_t err, void *sp, bool is_fatal)
{
    if (stdio_is_initialized) {
        if (is_fatal) {
            printf("[ERR] Fatal error ");
        } else {
            printf("[WRN] Non-fatal error ");
        }

        if (err != 0) {
            printf("%08X ", err);
        }

        if (file != NULL) {
            printf("in file %s ", file);
        }

        if (line != 0) {
            printf("at line %u ", line);
        }

        if (sp != NULL) {
            printf("with SP=%p ", sp);
        }

        printf("\r\n");
    }
}
