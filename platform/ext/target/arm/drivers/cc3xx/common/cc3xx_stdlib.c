/*
 * Copyright (c) 2023-2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cc3xx_stdlib.h"

#include "cc3xx_rng.h"
#include "cc3xx_config.h"

#include <assert.h>
#include <stdbool.h>

void cc3xx_secure_erase_buffer(uint32_t *buf, size_t word_count)
{
    size_t idx;
    uint32_t random_val;

    /* Overwrites the input buffer with random values */
    cc3xx_lowlevel_rng_get_random((uint8_t *)&random_val, sizeof(random_val),
                                  CC3XX_RNG_FAST);
    for (idx = 0; idx < word_count; idx++) {
        buf[idx] = random_val;
    }
}

#ifndef CC3XX_CONFIG_STDLIB_EXTERNAL_DPA_HARDENED_WORD_COPY
void cc3xx_dpa_hardened_word_copy(volatile uint32_t *dst,
                                  volatile const uint32_t *src, size_t word_count)
{
    uint8_t permutation_buf[word_count]; /* This is a VLA */
    size_t idx;

    /* We don't support more than 256 word permutations per copy, i.e. 2048 bit copy */
    assert(word_count <= UINT8_MAX);

    cc3xx_lowlevel_rng_get_random_permutation(permutation_buf, word_count,
                                              CC3XX_RNG_FAST);

    for(idx = 0; idx < word_count; idx++) {
        dst[permutation_buf[idx]] = src[permutation_buf[idx]];
    }
}
#else
void cc3xx_dpa_hardened_word_copy(volatile uint32_t *dst,
                                  volatile const uint32_t *src, size_t word_count)
{
    dpa_hardened_word_copy(dst, src, word_count);
}
#endif /* CC3XX_CONFIG_STDLIB_EXTERNAL_DPA_HARDENED_WORD_COPY */
