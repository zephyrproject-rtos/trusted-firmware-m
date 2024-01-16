/*
 * Copyright (c) 2023-2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/** @file cc3xx_misc.h
 *
 * Contains the definition of the macro for CC3XX_ASSERT(), and the
 * conversion function for error translation. The macro can be configured
 * at build time to behave differently, i.e. asserting, returning an error
 * code, or act transparently.
 */

#ifndef __CC3XX_MISC_H__
#define __CC3XX_MISC_H__

#include <assert.h>
#include "psa/crypto.h"
#include "cc3xx_psa_api_config.h"
#include "cc3xx_error.h"

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

/**
 * @brief Convert from CC3XX specific error codes to generic PSA error codes
 *
 * @param[in] err An error code as specified by the CC3XX lowlevel hardware
 * @return psa_status_t A corresponding PSA error status
 */
psa_status_t cc3xx_to_psa_err(enum cc3xx_error err);

#ifdef __cplusplus
}
#endif

#endif /* __CC3XX_MISC_H__ */
