#-------------------------------------------------------------------------------
# Copyright (c) 2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0079 NEW)

target_sources(tfm_test_suite_extra_bl1_2
    PRIVATE
        ./bl1_2_test.c
        ../rse_test_common.c
)

target_include_directories(tfm_test_suite_extra_bl1_2
    PRIVATE
        .
        ..
)
