#-------------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (RSS_XIP)
set(PLATFORM_DEFAULT_IMAGE_SIGNING      OFF       CACHE BOOL    "Use default image signing implementation")
set(BL2_HEADER_SIZE                     0x400     CACHE STRING  "BL2 Header size")
set(BL2_TRAILER_SIZE                    0x400     CACHE STRING  "BL2 Trailer size")

set(MCUBOOT_S_IMAGE_FLASH_AREA_NUM      10        CACHE STRING  "ID of the flash area containing the primary Secure image")
set(MCUBOOT_NS_IMAGE_FLASH_AREA_NUM     11        CACHE STRING  "ID of the flash area containing the primary Non-Secure image")
endif()

# Once all options are set, set common options as fallback
include(${CMAKE_CURRENT_LIST_DIR}/../common/config.cmake)
