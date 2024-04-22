/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "dpa_hardened_word_copy.h"

#include "device_definition.h"
#include "cc3xx_rng.h"
#include "cc3xx_config.h"

#include <assert.h>

/**
 * \brief This is a requirement for the maximum number of words that can
 *        be copied through a single call to \ref cc3xx_dpa_hardened_word_copy
 */
#define CC3XX_STDLIB_SECURE_COPY_MAX_WORDS (256)

void dpa_hardened_word_copy(volatile uint32_t *dst,
                            volatile const uint32_t *src, size_t word_count)
{
    uint8_t permutation_buf[word_count]; /* This is a VLA */
    size_t idx;

    /* We don't support more than 256 word permutations per copy, i.e. 2048 bit copy */
    assert(word_count <= CC3XX_STDLIB_SECURE_COPY_MAX_WORDS);

    /* Initializes the permutation buffer */
    for (idx = 0; idx < word_count; idx++) {
        permutation_buf[idx] = idx;
    }

    cc3xx_lowlevel_rng_get_random_permutation(permutation_buf, word_count,
                                              CC3XX_RNG_FAST);

    for(idx = 0; idx < word_count; idx++) {
        kmu_random_delay(&KMU_DEV_S, KMU_DELAY_LIMIT_32_CYCLES);
        dst[permutation_buf[idx]] = src[permutation_buf[idx]];
    }
}
