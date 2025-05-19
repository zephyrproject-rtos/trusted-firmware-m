/*
 * Copyright © 2025, Keith Packard <keithp@keithp.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <string.h>
#include "config_impl.h"

/*
 * Picolibc's startup code only initializes a single data and bss
 * segment.  Augment that by initializing the remaining segments using
 * the lists provided by the linker script in a constructor which will
 * be called from _start
 */

#if defined(__PICOLIBC__) && TFM_ISOLATION_LEVEL > 0

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
#endif
