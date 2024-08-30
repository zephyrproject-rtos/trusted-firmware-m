#-------------------------------------------------------------------------------
# Copyright (c) 2020-2023, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# The MUSCA_S1 has a CryptoCell-312 as an accelerator.
set(CRYPTO_HW_ACCELERATOR               ON          CACHE BOOL      "Whether to enable the crypto hardware accelerator on supported platforms")

set(PLATFORM_GPLED_ENABLED              OFF         CACHE BOOL      "Use the general purpose LEDs on the platform board")

if(BL2)
    set(BL2_TRAILER_SIZE                0x800       CACHE STRING    "Trailer size")
else()
    #No header if no bootloader, but keep IMAGE_CODE_SIZE the same
    set(BL2_TRAILER_SIZE                0xC00       CACHE STRING    "Trailer size")
endif()

# Platform-specific configurations
set(CONFIG_TFM_USE_TRUSTZONE            ON)
set(TFM_MULTI_CORE_TOPOLOGY             OFF)

if(CC3XX_RUNTIME_ENABLED)
    set(EXTRA_S_TEST_SUITE_PATH         "${CMAKE_CURRENT_LIST_DIR}/tests/secure" CACHE STRING "path to extra secure testsuite")
endif()

set(MCUBOOT_USE_PSA_CRYPTO            ON               CACHE BOOL      "Enable the cryptographic abstraction layer to use PSA Crypto APIs")
set(MCUBOOT_SIGNATURE_TYPE            "EC-P256"        CACHE STRING    "Algorithm to use for signature validation [RSA-2048, RSA-3072, EC-P256, EC-P384]")
set(MCUBOOT_HW_KEY                    OFF              CACHE BOOL      "Whether to embed the entire public key in the image metadata instead of the hash only")
set(MCUBOOT_BUILTIN_KEY               ON               CACHE BOOL      "Use builtin key(s) for validation, no public key data is embedded into the image metadata")
