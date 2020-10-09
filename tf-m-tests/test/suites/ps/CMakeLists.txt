#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0079 NEW)

if (NOT TFM_PARTITION_PROTECTED_STORAGE)
    return()
endif()

####################### Non Secure #############################################

add_library(tfm_test_suite_ps_ns STATIC EXCLUDE_FROM_ALL)

target_sources(tfm_test_suite_ps_ns
    PRIVATE
        non_secure/ns_test_helpers.c
        non_secure/psa_ps_ns_interface_testsuite.c
)

target_include_directories(tfm_test_suite_ps_ns
    PUBLIC
        non_secure
)

target_compile_definitions(tfm_test_suite_ps_ns
    PRIVATE
        DOMAIN_NS=1
)

target_link_libraries(tfm_test_suite_ps_ns
    PRIVATE
        tfm_test_framework
        platform_region_defs
        tfm_test_suite_its_ns
        psa_api_ns
)

target_link_libraries(tfm_ns_tests
    INTERFACE
        tfm_test_suite_ps_ns
)

####################### Secure #################################################

target_sources(tfm_test_suite_ps_s
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/secure/nv_counters/test_ps_nv_counters.c
        ${CMAKE_CURRENT_SOURCE_DIR}/secure/psa_ps_s_interface_testsuite.c
        ${CMAKE_CURRENT_SOURCE_DIR}/secure/psa_ps_s_reliability_testsuite.c
        ${CMAKE_CURRENT_SOURCE_DIR}/secure/ps_rollback_protection_testsuite.c
)

target_include_directories(tfm_test_suite_ps_s
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/secure
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/secure/nv_counters
)

target_link_libraries(tfm_test_suite_ps_s
    PRIVATE
        tfm_test_framework
        platform_region_defs
        tfm_partition_ps
        tfm_partition_ps_test
        tfm_spm
        platform_s
)

target_link_libraries(tfm_s_tests
    INTERFACE
        tfm_test_suite_ps_s
)
