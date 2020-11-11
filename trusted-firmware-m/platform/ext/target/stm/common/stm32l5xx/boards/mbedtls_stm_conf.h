/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef MBEDTLS_STM_CONF_H
#define MBEDTLS_STM_CONF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#undef MBEDTLS_TEST_NULL_ENTROPY
#undef MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES

#define MBEDTLS_PLATFORM_ENTROPY
#define MBEDTLS_ENTROPY_C
#define MBEDTLS_ENTROPY_HARDWARE_ALT

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MBEDTLS_STM_CONF_H */
