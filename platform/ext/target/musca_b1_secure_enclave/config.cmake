#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Configuration values forced due to the Secure Enclave topology
set(TFM_MULTI_CORE_TOPOLOGY             ON           CACHE BOOL      "Whether to build for a dual-cpu architecture" FORCE)
set(TFM_PSA_API                         ON           CACHE BOOL      "Use PSA api (IPC mode) instead of secure library mode" FORCE)
set(NS                                  FALSE        CACHE BOOL      "Whether to build NS app" FORCE)
set(TEST_NS                             OFF          CACHE BOOL      "Whether to build NS regression tests" FORCE)

# Serial output is not available for Secure Enclave
set(PLATFORM_DEFAULT_UART_STDOUT        FALSE        CACHE BOOL      "Use default uart stdout implementation." FORCE)
set(MCUBOOT_LOG_LEVEL                   "NONE"       CACHE STRING    "Level of logging to use for MCUboot [OFF, ERROR, WARNING, INFO, DEBUG]" FORCE)

# Test services are inaccessible via Proxy service
set(TEST_S                              OFF          CACHE BOOL      "Whether to build S regression tests" FORCE)

# Currently only level 1 isolation is supported
set(TFM_ISOLATION_LEVEL                 1            CACHE STRING    "Isolation level" FORCE)

# Crypto hardware accelerator is turned on by default
set(CRYPTO_HW_ACCELERATOR               ON           CACHE BOOL      "Whether to enable the crypto hardware accelerator on supported platforms")
