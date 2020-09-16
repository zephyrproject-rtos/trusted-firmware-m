#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

########################## BL2 #################################################

set(MCUBOOT_IMAGE_NUMBER                2           CACHE STRING    "Whether to combine S and NS into either 1 image, or sign each seperately" FORCE)

################################## Dependencies ################################

set(MBEDCRYPTO_BUILD_TYPE               minsizerel  CACHE STRING "Build type of Mbed Crypto library" FORCE)
