/*
 * Copyright (c) 2019-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef MBEDTLS_ACCELERATOR_CONFIG_BL2_H
#define MBEDTLS_ACCELERATOR_CONFIG_BL2_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* When using the PSA Unified driver API, this is the only define that has to be added
 * to the ones being available in the non-platform aware config files being used, i.e.
 * just make sure that that the Crypto core and driver wrappers are aware of the driver
 */
#define PSA_CRYPTO_DRIVER_CC3XX

/* As CC3XX has HW P256 acceleration for signature verification, we can just disable the
 * P256M software driver if it is being enabled in the BL2 config.
 */
#if defined(MBEDTLS_PSA_P256M_DRIVER_ENABLED)
#undef MBEDTLS_PSA_P256M_DRIVER_ENABLED
#endif /* */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MBEDTLS_ACCELERATOR_CONFIG_BL2_H */
