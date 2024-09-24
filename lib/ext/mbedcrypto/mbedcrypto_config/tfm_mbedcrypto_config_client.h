/*
 * Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/**
 * \file config.h
 *
 * \brief Configuration options (set of defines)
 *
 *  This set of compile-time options may be used to enable
 *  or disable features selectively, and reduce the global
 *  memory footprint.
 */

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

/** \def MBEDTLS_PSA_CRYPTO_CLIENT
 *
 * Enable support for PSA crypto client.
 *
 * \note This option allows to include the code necessary for a PSA
 *       crypto client when the PSA crypto implementation is not included in
 *       the library (MBEDTLS_PSA_CRYPTO_C disabled). The code included is the
 *       code to set and get PSA key attributes.
 *       The development of PSA drivers partially relying on the library to
 *       fulfill the hardware gaps is another possible usage of this option.
 *
 * \warning This interface is experimental and may change or be removed
 * without notice.
 */
#define MBEDTLS_PSA_CRYPTO_CLIENT

/**
 * \def MBEDTLS_PSA_CRYPTO_CONFIG
 *
 * This setting allows support for cryptographic mechanisms through the PSA
 * API to be configured separately from support through the mbedtls API.
 *
 * When this option is disabled, the PSA API exposes the cryptographic
 * mechanisms that can be implemented on top of the `mbedtls_xxx` API
 * configured with `MBEDTLS_XXX` symbols.
 *
 * When this option is enabled, the PSA API exposes the cryptographic
 * mechanisms requested by the `PSA_WANT_XXX` symbols defined in
 * include/psa/crypto_config.h. The corresponding `MBEDTLS_XXX` settings are
 * automatically enabled if required (i.e. if no PSA driver provides the
 * mechanism). You may still freely enable additional `MBEDTLS_XXX` symbols
 * in mbedtls_config.h.
 *
 * If the symbol #MBEDTLS_PSA_CRYPTO_CONFIG_FILE is defined, it specifies
 * an alternative header to include instead of include/psa/crypto_config.h.
 *
 * This feature is still experimental and is not ready for production since
 * it is not completed.
 */
#define MBEDTLS_PSA_CRYPTO_CONFIG

#endif /* MBEDTLS_CONFIG_H */
