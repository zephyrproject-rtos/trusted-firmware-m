#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------


set(TFM_CRYPTO_TEST_ALG_CFB             OFF         CACHE BOOL      "Test CFB cryptography mode")
set(PLATFORM_GPLED_ENABLED              OFF         CACHE BOOL      "Use the general purpose LEDs on the platform board")

set(CONFIG_TFM_USE_TRUSTZONE            ON          CACHE BOOL      "Enable use of TrustZone to transition between NSPE and SPE")
set(TFM_MULTI_CORE_TOPOLOGY             OFF         CACHE BOOL      "Whether to build for a dual-cpu architecture")

set(MCUBOOT_DATA_SHARING                ON          CACHE BOOL      "Add sharing of application specific data using the same shared data area as for the measured boot")
set(PLATFORM_HAS_FIRMWARE_UPDATE_SUPPORT ON         CACHE BOOL      "Platform supports firmware update, such as network connectivities and bootloader support")

set(CRYPTO_HW_ACCELERATOR               ON          CACHE BOOL      "Whether to enable the crypto hardware accelerator on supported platforms")

set(TFM_EXTRA_GENERATED_FILE_LIST_PATH  ${CMAKE_SOURCE_DIR}/platform/ext/target/arm/musca_b1/generated_file_list.yaml  CACHE PATH "Path to extra generated file list. Appended to stardard TFM generated file list." FORCE)

set(PSA_API_TEST_TARGET                 "musca_b1"   CACHE STRING    "Target to use when building the PSA API tests")

set(PLATFORM_HAS_ISOLATION_L3_SUPPORT ON    CACHE BOOL    "Platform supports Isolation level 3")
set(PLATFORM_SLIH_IRQ_TEST_SUPPORT    ON    CACHE BOOL    "Platform supports SLIH IRQ tests")

if(BL2)
    set(BL2_TRAILER_SIZE 0x800 CACHE STRING "Trailer size")
else()
    #No header if no bootloader, but keep IMAGE_CODE_SIZE the same
    set(BL2_TRAILER_SIZE 0xC00 CACHE STRING "Trailer size")
endif()

if(PLATFORM_GPLED_ENABLED)
    #If required, Musca_b1 platform supports GPLED services
    set(PLATFORM_GPLED_SUPPORTED        ON          CACHE BOOL      "Whether to enable the gpled functionality on supported platforms")
else()
    #By default, GPLED support is disabled
    set(PLATFORM_GPLED_SUPPORTED        OFF         CACHE BOOL      "Whether to enable the gpled functionality on supported platforms")
endif()

set(PLATFORM_DEFAULT_SYSTEM_RESET_HALT  OFF         CACHE BOOL      "Use default system reset/halt implementation")
