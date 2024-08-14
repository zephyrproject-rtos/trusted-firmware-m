#-------------------------------------------------------------------------------
# Copyright (c) 2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.21)

add_library(tfm_log_unpriv_headers INTERFACE)
add_library(tfm_log_unpriv STATIC)

target_sources(tfm_log_unpriv
    PRIVATE
        src/tfm_log_unpriv.c
)

target_include_directories(tfm_log_unpriv_headers
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

target_link_libraries(tfm_log_unpriv
    PUBLIC
        tfm_log_unpriv_headers
        tfm_vprintf
    PRIVATE
        tfm_sprt
)

# Not actually used by the tfm_log library but required
# to prevent preprocessor error on undefined LOG_LEVEL_UNPRIV
target_compile_definitions(tfm_log_unpriv
    PRIVATE
        LOG_LEVEL_UNPRIV=0
)
