#-------------------------------------------------------------------------------
# Copyright (c) 2023-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# CFB mode is not supported by CC312
set(TFM_CRYPTO_TEST_ALG_CFB             OFF        CACHE BOOL     "Test CFB cryptography mode")
set(TFM_CRYPTO_TEST_ALG_OFB             OFF        CACHE BOOL     "Test CFB cryptography mode")
set(TFM_CRYPTO_TEST_ALG_RSASSA_PSS_VERIFICATION OFF       CACHE BOOL      "Test RSASSA-PSS signature verification algorithm")

set(TEST_NS_MULTI_CORE                  OFF        CACHE BOOL     "Whether to build NS regression multi-core tests")

set(PLATFORM_SLIH_IRQ_TEST_SUPPORT      ON)
set(PLATFORM_FLIH_IRQ_TEST_SUPPORT      ON)
