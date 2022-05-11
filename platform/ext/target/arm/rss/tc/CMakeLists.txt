#-------------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)
set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../common common)

#========================= Platform region defs ===============================#

target_include_directories(platform_region_defs
    INTERFACE
        .
)

#========================= Platform BL2 =======================================#

target_sources(platform_bl2
    PRIVATE
        bl2/boot_hal_bl2.c
        bl2/flash_map_bl2.c
)

target_compile_definitions(platform_bl2
    PUBLIC
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:PLATFORM_HAS_BOOT_DMA>
        $<$<BOOL:${PLATFORM_BOOT_DMA_MIN_SIZE_REQ}>:BOOT_DMA_MIN_SIZE_REQ=${PLATFORM_BOOT_DMA_MIN_SIZE_REQ}>
)

target_include_directories(platform_bl2
    PUBLIC
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:${CMAKE_CURRENT_SOURCE_DIR}/../common/bl2>
)
