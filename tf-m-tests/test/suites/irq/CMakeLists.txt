#-------------------------------------------------------------------------------
# Copyright (c) 2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0079 NEW)

if (NOT TFM_ENABLE_SLIH_TEST AND NOT TFM_ENABLE_FLIH_TEST)
    return()
endif()

####################### Non Secure #############################################

add_library(tfm_test_suite_irq STATIC EXCLUDE_FROM_ALL)

target_sources(tfm_test_suite_irq
    PRIVATE
        ./irq_testsuite.c
)

target_include_directories(tfm_test_suite_irq
    PUBLIC
        ./
)

target_compile_definitions(tfm_test_suite_irq
    PUBLIC
        $<$<BOOL:${TFM_ENABLE_SLIH_TEST}>:TFM_ENABLE_SLIH_TEST>
        $<$<BOOL:${TFM_ENABLE_FLIH_TEST}>:TFM_ENABLE_FLIH_TEST>
)

target_link_libraries(tfm_test_suite_irq
    PRIVATE
        tfm_test_framework_ns
        $<$<BOOL:${TFM_ENABLE_SLIH_TEST}>:tfm_slih_test_service_interface>
        $<$<BOOL:${TFM_ENABLE_FLIH_TEST}>:tfm_flih_test_service_interface>
        platform_ns
)

target_link_libraries(tfm_ns_tests
    INTERFACE
        tfm_test_suite_irq
)
