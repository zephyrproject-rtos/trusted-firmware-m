/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __TFM_COPY_ZERO_TABLES_H__
#define __TFM_COPY_ZERO_TABLES_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t const *src;
    uint32_t       *dest;
    uint32_t        wlen;
} tfm_copy_table_t;

typedef struct {
    uint32_t *dest;
    uint32_t  wlen;
} tfm_zero_table_t;

/* Emitted by the scatter linker scripts; weak so images without them link. */
extern const tfm_copy_table_t __copy_table_start__[] __attribute__((weak));
extern const tfm_copy_table_t __copy_table_end__[]   __attribute__((weak));
extern const tfm_zero_table_t __zero_table_start__[] __attribute__((weak));
extern const tfm_zero_table_t __zero_table_end__[]   __attribute__((weak));

/*
 * Initialise the RW data and bss regions described by the CMSIS scatter
 * __copy_table__/__zero_table__. Needed by startups that enter a C library
 * whose own crt0 (e.g. picolibc when CONFIG_TFM_INCLUDE_STDLIBC is enabled)
 * initialises only the single __data/__bss region. Inlined so it links into
 * every image sharing the startup (BL1, BL2, SPE) without extra build wiring.
 */
static inline void tfm_copy_zero_tables(void)
{
    const tfm_copy_table_t *ct;
    const tfm_zero_table_t *zt;
    uint32_t i;

    for (ct = __copy_table_start__; ct < __copy_table_end__; ct++) {
        for (i = 0u; i < ct->wlen; i++) {
            ct->dest[i] = ct->src[i];
        }
    }

    for (zt = __zero_table_start__; zt < __zero_table_end__; zt++) {
        for (i = 0u; i < zt->wlen; i++) {
            zt->dest[i] = 0u;
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif /* __TFM_COPY_ZERO_TABLES_H__ */
