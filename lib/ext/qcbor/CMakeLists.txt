#-------------------------------------------------------------------------------
# Copyright (c) 2020-2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

# Default configuration of QCBOR repository
set(QCBOR_PATH      "DOWNLOAD"  CACHE PATH      "Path to qcbor (or DOWNLOAD to fetch automatically")
set(QCBOR_VERSION   "92d3f890"  CACHE STRING    "The version of qcbor to use")

# Entirely disable the floating-point support
# in the qcbor library to reduce code size.
set(QCBOR_OPT_DISABLE_FLOAT_HW_USE     ON  CACHE BOOL  "Eliminate dependency on FP hardware and FP instructions" FORCE)
set(QCBOR_OPT_DISABLE_FLOAT_PREFERRED  ON  CACHE BOOL  "Eliminate support for half-precision and CBOR preferred serialization" FORCE)
set(QCBOR_OPT_DISABLE_FLOAT_ALL        ON  CACHE BOOL  "Eliminate floating-point support completely" FORCE)

if ("${QCBOR_PATH}" STREQUAL "DOWNLOAD")
    FetchContent_Declare(qcbor
        GIT_REPOSITORY https://github.com/laurencelundblade/QCBOR.git
        GIT_TAG ${QCBOR_VERSION}
        GIT_PROGRESS TRUE
    )

    FetchContent_GetProperties(qcbor)
    if(NOT qcbor_POPULATED)
        FetchContent_Populate(qcbor)
        set(QCBOR_PATH ${qcbor_SOURCE_DIR} CACHE PATH "Path to qcbor (or DOWNLOAD to get automatically" FORCE)
    endif()
endif()

add_subdirectory(${QCBOR_PATH} ${CMAKE_CURRENT_BINARY_DIR}/QCBOR)

set_target_properties(qcbor PROPERTIES EXCLUDE_FROM_ALL TRUE)

target_compile_options(qcbor
    PUBLIC
        ${COMPILER_CP_FLAG}
)

target_include_directories(qcbor
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
