#-------------------------------------------------------------------------------
# Copyright (c) 2020-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.21)

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
