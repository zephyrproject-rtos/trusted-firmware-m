/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_STDLIB_H
#define CC3XX_STDLIB_H

#include "stdint.h"
#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief                        Copy a series of words in a randomised order.
 *                               Intended to be used as a DPA countermeasure
 *                               when copying key materal.
 *
 * \Note                         This function may take a variable amount of
 *                               time to execute.
 *
 * \param[out] dst               Destination buffer to copy into
 * \param[in]  src               Source buffer to copy from.
 * \param[in]  word_count        The amount of words to copy.
 */
void cc3xx_secure_word_copy(volatile uint32_t *dst, volatile const uint32_t *src,
                            size_t word_count);

#ifdef __cplusplus
}
#endif

#endif /* CC3XX_STDLIB_H */
