#-------------------------------------------------------------------------------
# Copyright (c) 2020-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.21)


include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

# Default configuration of T_COSE repository
set(T_COSE_PATH     "DOWNLOAD"      CACHE PATH      "Path to t_cose (or DOWNLOAD to fetch automatically")
set(T_COSE_VERSION  "v2.0-alpha-2"  CACHE STRING    "The version of t_cose to use")

fetch_remote_library(
    LIB_NAME                t_cose
    LIB_SOURCE_PATH_VAR     T_COSE_PATH
    LIB_PATCH_DIR           ${CMAKE_CURRENT_LIST_DIR}
    LIB_BASE_DIR            "${CMAKE_BINARY_DIR}/lib/ext"
    FETCH_CONTENT_ARGS
        GIT_REPOSITORY      https://github.com/laurencelundblade/t_cose.git
        GIT_TAG             ${T_COSE_VERSION}
        GIT_SHALLOW         TRUE
        GIT_PROGRESS        TRUE
)

include(${CMAKE_CURRENT_LIST_DIR}/tfm_t_cose.cmake)

############################ t_cose secure #####################################

add_library(tfm_t_cose_s STATIC EXCLUDE_FROM_ALL)

target_link_libraries(tfm_t_cose_s
    PUBLIC
        tfm_t_cose_defs
    PRIVATE
        tfm_t_cose_common
        psa_interface
        qcbor
        platform_s
)

target_compile_options(tfm_t_cose_s
    PRIVATE
        ${COMPILER_CP_FLAG}
)
