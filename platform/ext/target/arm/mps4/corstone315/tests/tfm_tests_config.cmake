#-------------------------------------------------------------------------------
# Copyright (c) 2023-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Make FLIH IRQ test as the default IRQ test on Corstone-315
set(TEST_NS_SLIH_IRQ                  OFF   CACHE BOOL    "Whether to build NS regression Second-Level Interrupt Handling tests")

set(PLATFORM_SLIH_IRQ_TEST_SUPPORT    ON)
set(PLATFORM_FLIH_IRQ_TEST_SUPPORT    ON)

# CFB mode is not supported by CC312
set(TFM_CRYPTO_TEST_ALG_CFB             OFF        CACHE BOOL     "Test CFB cryptography mode")
set(TEST_NS_MULTI_CORE                  OFF        CACHE BOOL     "Whether to build NS regression multi-core tests")
