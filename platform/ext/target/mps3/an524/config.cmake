#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(MCUBOOT_IMAGE_NUMBER                1           CACHE STRING    "Whether to combine S and NS into either 1 image, or sign each seperately" FORCE)
set(MCUBOOT_UPGRADE_STRATEGY            "NO_SWAP"   CACHE STRING "Upgrade strategy for images [OVERWRITE_ONLY, SWAP, NO_SWAP, RAM_LOADING]" FORCE)

################################## Dependencies ################################

set(MBEDCRYPTO_BUILD_TYPE               minsizerel  CACHE STRING "Build type of Mbed Crypto library" FORCE)
