#-------------------------------------------------------------------------------
# Copyright (c) 2020, Nordic Semiconductor ASA.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)
set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(NRF_BOARD_SELECTED True)

add_subdirectory(../common/nrf9160 nrf9160)

target_include_directories(platform_s PUBLIC .)
target_include_directories(platform_ns PUBLIC .)

if(BL2)
    target_include_directories(platform_bl2 PRIVATE .)
endif()