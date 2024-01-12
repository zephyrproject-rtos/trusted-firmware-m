#-------------------------------------------------------------------------------
# Copyright (c) 2023-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Note: Cached varaibles always take the first value set, while normal
# variables always take the last.

set(TFM_BL1_MEMORY_MAPPED_FLASH         OFF      CACHE BOOL    "Whether BL1 can directly access flash content")
set(PLATFORM_HAS_BOOT_DMA               OFF      CACHE BOOL    "Enable dma support for memory transactions for bootloader")
set(PLAT_MHU_VERSION                    3        CACHE STRING  "Supported MHU version by platform")
set(MCUBOOT_IMAGE_NUMBER                2        CACHE STRING  "Number of images supported by MCUBoot")
set(RSS_USE_HOST_UART                   OFF      CACHE BOOL    "Whether RSS should use the UART from the host system (opposed to dedicated UART private to RSS)")
set(RSS_USE_HOST_FLASH                  OFF      CACHE BOOL    "Enable RSS using the host flash.")
set(RSS_HAS_EXPANSION_PERIPHERALS       ON       CACHE BOOL    "Whether RSS has sub-platform specific peripherals in the expansion layer")
set(RSS_LOAD_NS_IMAGE                   OFF      CACHE BOOL    "Whether to load an NS image")

# Once all cache options are set, set common options as fallback
include(${CMAKE_CURRENT_LIST_DIR}/../common/config.cmake)
