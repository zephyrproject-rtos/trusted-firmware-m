#-------------------------------------------------------------------------------
# Copyright (c) 2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.21)

if (NOT CONFIG_TFM_BACKTRACE_ON_CORE_PANIC)
    return()
endif()

add_library(tfm_backtrace_headers INTERFACE)
add_library(tfm_backtrace STATIC)

target_sources(tfm_backtrace
    PRIVATE
        src/backtrace.c
)

target_include_directories(tfm_backtrace_headers
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

target_link_libraries(tfm_backtrace
    PUBLIC
        tfm_backtrace_headers
    PRIVATE
        tfm_vprintf
)

target_compile_options(tfm_backtrace_headers
    INTERFACE
        -funwind-tables
)
