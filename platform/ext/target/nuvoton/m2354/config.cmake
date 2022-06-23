#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(MCUBOOT_IMAGE_NUMBER    1      CACHE STRING    "Whether to combine S and NS into either 1 image, or sign each separately")
set(PS_MAX_ASSET_SIZE       512    CACHE STRING    "The maximum asset size to be stored in the Protected Storage area")
set(PS_NUM_ASSETS           12     CACHE STRING    "The maximum number of assets to be stored in the Protected Storage area")
set(ITS_NUM_ASSETS          12     CACHE STRING    "The maximum number of assets to be stored in the Internal Trusted Storage area")
set(BL2_TRAILER_SIZE        0x800  CACHE STRING    "Trailer size")

set(CONFIG_TFM_USE_TRUSTZONE ON    CACHE BOOL      "Enable use of TrustZone to transition between NSPE and SPE")
set(TFM_MULTI_CORE_TOPOLOGY  OFF   CACHE BOOL      "Whether to build for a dual-cpu architecture")
