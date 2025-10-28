/*
 * Copyright © 2025, Keith Packard <keithp@keithp.com>
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
 * segment.  Augment that by initializing the remaining segments using
 * the lists provided by the linker script in a constructor which will
 * be called from _start
 */

static void __attribute__((constructor))
picolibc_startup(void)
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
}

/*
 * Create symbols for picolibc. These are used by the
 * startup code to initialize memory, but that work
 * is done by the function above
 */

__asm__(".globl __data_source\n"
        ".globl __data_start\n"
        ".globl __data_size\n"
        ".globl __bss_size\n"
        ".globl __bss_start\n"
        ".equ __data_source, 0\n"
        ".equ __data_start, 0\n"
        ".equ __data_size, 0\n"
        ".equ __bss_size, 0\n"
        ".equ __bss_start, 0");

#ifdef __THREAD_LOCAL_STORAGE

__asm__(".globl __tls_base\n"
        ".equ __tls_base, 0");

#include <picotls.h>

/*
 * Picolibc's startup code wants to initialize the thread local
 * storage base address, but tf-m doesn't use any TLS variables.
 *
 * Stub out this function until TLS is required.
 */

void
_set_tls(void *tls_base)
{
    (void) tls_base;
}

#endif

#endif /* __PICOLIBC__ */
