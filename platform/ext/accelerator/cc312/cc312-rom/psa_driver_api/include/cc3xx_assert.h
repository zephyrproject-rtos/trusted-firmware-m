/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/** @file cc3xx_assert.h
 *
 * Contains the definition of the macro for CC3XX_ASSERT(). The macro
 * can be configured at build time to behave differently, i.e. asserting,
 * returning an error code, or not doing anything
 */

#ifndef __CC3XX_ASSERT_H__
#define __CC3XX_ASSERT_H__

#include <assert.h>
#include "psa/crypto.h"
#include "cc3xx_psa_api_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CC3XX_CONFIG_ASSERT_ENABLE)

#define CC3XX_ASSERT(x) assert(x)

#elif defined(CC3XX_CONFIG_ASSERT_CHECK)

#define CC3XX_ASSERT(x)                          \
    do {                                         \
        if (!(x)) {                              \
            return PSA_ERROR_INVALID_ARGUMENT;   \
        }                                        \
    } while(0)                                   \

#elif defined(CC3XX_CONFIG_ASSERT_DISABLE)

#define CC3XX_ASSERT(x) (void)(0)

#else

#error "CC3XX_CONFIG_ASSERT is not configured!"

#endif /* CC3XX_CONFIG_ASSERT_ */

#ifdef __cplusplus
}
#endif

#endif /* __CC3XX_ASSERT_H__ */
