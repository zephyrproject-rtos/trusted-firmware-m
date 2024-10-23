/*
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 */

#undef MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG
#define MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG 1

#define PSA_WANT_ALG_GCM 1

