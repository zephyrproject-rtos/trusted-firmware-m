#-------------------------------------------------------------------------------
# Copyright (c) 2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.21)

add_library(tfm_log_headers INTERFACE)
add_library(tfm_log STATIC)

target_sources(tfm_log
    PRIVATE
        src/tfm_log.c
)

target_include_directories(tfm_log_headers
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

target_link_libraries(tfm_log
    PUBLIC
        tfm_log_headers
        tfm_vprintf
    PRIVATE
        platform_common_interface
)

# Not actually used by the tfm_log library but required
# to prevent preprocessor error on undefined LOG_LEVEL
target_compile_definitions(tfm_log
    PRIVATE
        LOG_LEVEL=0
)
