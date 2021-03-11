#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(MCUBOOT_UPGRADE_STRATEGY            "RAM_LOAD" CACHE STRING   "Upgrade strategy when multiple boot images are loaded [OVERWRITE_ONLY, SWAP, DIRECT_XIP, RAM_LOAD]")
set(MCUBOOT_IMAGE_NUMBER                1          CACHE STRING   "Whether to combine S and NS into either 1 image, or sign each separately")
set(PS_MAX_ASSET_SIZE                   "512"      CACHE STRING   "The maximum asset size to be stored in the Protected Storage area")
