#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(CRYPTO_HW_ACCELERATOR               ON          CACHE BOOL      "Whether to enable the crypto hardware accelerator on supported platforms")
set(TFM_CRYPTO_TEST_ALG_CFB             OFF         CACHE BOOL      "Test CFB cryptography mode")
set(PLATFORM_GPLED_ENABLED              OFF         CACHE BOOL      "Use the general purpose LEDs on the platform board")

if(BL2)
    set(BL2_TRAILER_SIZE                0x800       CACHE STRING    "Trailer size")
else()
    #No header if no bootloader, but keep IMAGE_CODE_SIZE the same
    set(BL2_TRAILER_SIZE                0xC00       CACHE STRING    "Trailer size")
endif()

set(CONFIG_TFM_USE_TRUSTZONE            ON          CACHE BOOL     "Enable use of TrustZone to transition between NSPE and SPE")
set(TFM_MULTI_CORE_TOPOLOGY             OFF         CACHE BOOL     "Whether to build for a dual-cpu architecture")

if(PLATFORM_GPLED_ENABLED)
    #If required, Musca_s1 platform supports GPLED services
    set(PLATFORM_GPLED_SUPPORTED        ON          CACHE BOOL      "Whether to enable the gpled functionality on supported platforms")
else()
    #By default, GPLED support is disabled
    set(PLATFORM_GPLED_SUPPORTED        OFF         CACHE BOOL      "Whether to enable the gpled functionality on supported platforms")
endif()
