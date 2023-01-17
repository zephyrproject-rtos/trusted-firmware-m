#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(MCUBOOT_IMAGE_NUMBER                1            CACHE STRING "Whether to combine S and NS into either 1 image, or sign each separately")
set(MCUBOOT_UPGRADE_STRATEGY            "DIRECT_XIP" CACHE STRING "Upgrade strategy for images")
if(NOT BL2)
    set(BL2_TRAILER_SIZE                0x800        CACHE STRING "Trailer size")
endif()
################################## Dependencies ################################

set(CONFIG_TFM_USE_TRUSTZONE            ON          CACHE BOOL   "Enable use of TrustZone to transition between NSPE and SPE")
set(TFM_MULTI_CORE_TOPOLOGY             OFF         CACHE BOOL   "Whether to build for a dual-cpu architecture")

set(MBEDCRYPTO_BUILD_TYPE               minsizerel  CACHE STRING "Build type of Mbed Crypto library")
