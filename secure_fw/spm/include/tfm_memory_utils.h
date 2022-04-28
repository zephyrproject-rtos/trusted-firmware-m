/*
 * Copyright (c) 2019-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_MEMORY_UTILS_H__
#define __TFM_MEMORY_UTILS_H__

#include <string.h>
#include "cmsis_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__ ((always_inline)) __STATIC_INLINE
void *tfm_memcpy(void *dest, const void *src, size_t num)
{
    return (memcpy(dest, src, num));
}

__attribute__ ((always_inline)) __STATIC_INLINE
int tfm_memcmp(const void *ptr1, const void *ptr2, size_t num)
{
    return (memcmp(ptr1, ptr2, num));
}

__attribute__ ((always_inline)) __STATIC_INLINE
void *tfm_memset(void *ptr, int value, size_t num)
{
    return (memset(ptr, value, num));
}

#ifdef __cplusplus
}
#endif

#endif /* __TFM_MEMORY_UTILS_H__ */
