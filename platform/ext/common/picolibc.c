/*
 * Copyright 2025, Keith Packard <keithp@keithp.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <string.h>
#include "config_impl.h"

#ifdef __PICOLIBC__

/*
 * Picolibc's startup code only initializes a single data and bss
 * segment. Replace that to initialize all of the segments using
 * the lists provided by the linker script.
 */

void __libc_init_array(void);
void _start(void);
int main(int, char **);

void
_start(void)
{
    typedef struct __copy_table {
        uint32_t const* src;
        uint32_t* dest;
        uint32_t  wlen;
    } __copy_table_t;

    typedef struct __zero_table {
        uint32_t* dest;
        uint32_t  wlen;
    } __zero_table_t;

    extern const __copy_table_t __copy_table_start__;
    extern const __copy_table_t __copy_table_end__;
    extern const __zero_table_t __zero_table_start__;
    extern const __zero_table_t __zero_table_end__;

    for (__copy_table_t const* pTable = &__copy_table_start__; pTable < &__copy_table_end__; ++pTable) {
        memcpy(pTable->dest, pTable->src, pTable->wlen << 2);
    }

    for (__zero_table_t const* pTable = &__zero_table_start__; pTable < &__zero_table_end__; ++pTable) {
        memset(pTable->dest, 0, pTable->wlen << 2);
    }

    __libc_init_array();
    main(0, NULL);
    return;
}

#endif /* __PICOLIBC__ */
