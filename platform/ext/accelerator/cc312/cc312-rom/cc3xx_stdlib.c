/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cc3xx_stdlib.h"

#include "cc3xx_rng.h"

#include "assert.h"

#ifdef CC3XX_CONFIG_STDLIB_EXTERNAL_SECURE_WORD_COPY
#include "secure_word_copy.h"
#endif /* CC3XX_CONFIG_STDLIB_EXTERNAL_SECURE_WORD_COPY */

#define SECURE_COPY_MAX_WORDS 8

/* https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle. This returns a
 * uniformly random permutation, verified by experiment.
 */
static void fisher_yates_shuffle(uint8_t *permutation_buf, size_t len)
{
    uint32_t idx;
    uint32_t swap_idx;
    uint8_t temp_elem;

    for (idx = 0; idx <= len - 1; idx++) {
        /* Don't catch the error, in that case just perform no swap this round */
        swap_idx = 0;
        cc3xx_rng_get_random_uint(len - idx, &swap_idx);

        swap_idx += idx;
        temp_elem = permutation_buf[idx];
        permutation_buf[idx] = permutation_buf[swap_idx];
        permutation_buf[swap_idx] = temp_elem;
    }
}

#ifndef CC3XX_CONFIG_STDLIB_EXTERNAL_SECURE_WORD_COPY
void cc3xx_secure_word_copy(volatile uint32_t *dst, volatile const uint32_t *src, size_t word_count)
{
    uint8_t permutation_buf[SECURE_COPY_MAX_WORDS] = {0, 1, 2, 3, 4, 5, 6, 7};
    uint32_t offset = 0;
    size_t idx;


    /* If the size is small enough use the shuffled permutation buf, if not then
     * use a randomized offset.
     */
    if (word_count <= SECURE_COPY_MAX_WORDS) {
        fisher_yates_shuffle(permutation_buf, word_count);
        for(idx = 0; idx < word_count; idx++) {
            dst[permutation_buf[idx]] = src[permutation_buf[idx]];
        }
    } else {
        /* Once again failure isn't an error, it just makes it less random */
        cc3xx_rng_get_random_uint(word_count, &offset);
        for (idx = 0; idx < word_count; idx++) {
            dst[(idx + offset) % word_count] = src[(idx + offset) % word_count];
        }
    }
}
#else
void cc3xx_secure_word_copy(volatile uint32_t *dst, volatile const uint32_t *src, size_t word_count)
{
    secure_word_copy(dst, src, word_count);
}
#endif /* CC3XX_CONFIG_STDLIB_EXTERNAL_SECURE_WORD_COPY */
