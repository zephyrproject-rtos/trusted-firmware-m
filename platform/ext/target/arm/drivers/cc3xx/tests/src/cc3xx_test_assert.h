/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_TEST_ASSERT_H
#define CC3XX_TEST_ASSERT_H

#include <assert.h>

#define cc3xx_test_assert(x) do { \
        if (!(x)) { \
            printf_set_color(RED); \
            TEST_LOG("CC3XX test assertion %s failed at %s:%d\r\n", #x, __FILE__, __LINE__); \
            printf_set_color(DEFAULT); \
            rc = 1; \
            goto cleanup; \
        } \
    } while (0);

#endif /* CC3XX_TEST_ASSERT_H */
