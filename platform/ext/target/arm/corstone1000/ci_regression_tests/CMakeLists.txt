#-------------------------------------------------------------------------------
# Copyright (c) 2021-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0079 NEW)

include(${CMAKE_CURRENT_SOURCE_DIR}/s_test_config.cmake)

####################### Secure #################################################

add_library(corstone1000_test_s STATIC EXCLUDE_FROM_ALL)

target_sources(corstone1000_test_s
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/s_test.c
        ../Native_Driver/firewall.c
)

target_include_directories(corstone1000_test_s
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ../Device/Include
        ../Native_Driver
)

# Example test links tfm_test_suite_extra_common to use related interface
target_link_libraries(corstone1000_test_s
    PRIVATE
        tfm_test_suite_extra_common
        tfm_sprt
)

target_compile_definitions(corstone1000_test_s
    PRIVATE
        $<$<BOOL:${PLATFORM_IS_FVP}>:PLATFORM_IS_FVP>
)

# The corstone1000_test_s library is linked by tfm_test_suite_extra_s
target_link_libraries(tfm_test_suite_extra_s
    PRIVATE
        corstone1000_test_s
)
