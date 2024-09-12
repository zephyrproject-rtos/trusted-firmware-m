/*
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 */

#include <stddef.h>

size_t strlen(const char *s)
{
    size_t idx = 0;

    while (s[idx] != '\0') {
        idx++;
    }
    return idx;
}
