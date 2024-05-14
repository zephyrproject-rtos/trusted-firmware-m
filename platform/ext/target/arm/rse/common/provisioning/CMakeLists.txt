#-------------------------------------------------------------------------------
# Copyright (c) 2023-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

find_package(Python3)

add_subdirectory(bundle_cm)
add_subdirectory(bundle_dm)

target_sources(platform_s
    PRIVATE
        ./runtime_stub_provisioning.c
)

target_sources(platform_bl2
    PRIVATE
        ./bl2_stub_provisioning.c
)

target_sources(bl1_1_lib
    INTERFACE
        ./bl1_provisioning.c
)

target_include_directories(bl1_1_lib
    INTERFACE
        ./bundle_common
)

add_custom_target(cm_provisioning_key
    ALL
    SOURCES ${CMAKE_CURRENT_BINARY_DIR}/cm_provisioning_key.bin
)

add_custom_target(dm_provisioning_key
    ALL
    SOURCES ${CMAKE_CURRENT_BINARY_DIR}/dm_provisioning_key.bin
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cm_provisioning_key.bin
    DEPENDS ${RSE_RTL_KEY_PATH}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/derive_cm_and_dm_keys.py
        --tp_mode=${RSE_TP_MODE}
        --krtl_file=${RSE_RTL_KEY_PATH}
        --key_select=cm
        --output_key_file=${CMAKE_CURRENT_BINARY_DIR}/cm_provisioning_key.bin
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dm_provisioning_key.bin
    DEPENDS ${RSE_RTL_KEY_PATH}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/derive_cm_and_dm_keys.py
        --tp_mode=${RSE_TP_MODE}
        --krtl_file=${RSE_RTL_KEY_PATH}
        --key_select=dm
        --output_key_file=${CMAKE_CURRENT_BINARY_DIR}/dm_provisioning_key.bin
)
