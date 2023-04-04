#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

find_package(Python3)

add_executable(cm_provisioning_bundle)

set_target_properties(cm_provisioning_bundle
    PROPERTIES
        SUFFIX ".axf"
)

target_add_scatter_file(cm_provisioning_bundle
    $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/../bundle_common/provisioning_bundle.sct>
    $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/../bundle_common/provisioning_bundle.ld>
)

target_link_options(cm_provisioning_bundle
    PRIVATE
        $<$<C_COMPILER_ID:GNU>:-Wl,-Map=${CMAKE_BINARY_DIR}/bin/cm_provisioning_bundle.map>
        $<$<C_COMPILER_ID:ARMClang>:--map>
        $<$<C_COMPILER_ID:IAR>:--map\;${CMAKE_BINARY_DIR}/bin/cm_provisioning_bundle.map>
)

target_link_options(cm_provisioning_bundle
    PRIVATE
        --entry=do_provision
)

target_sources(cm_provisioning_bundle
    PRIVATE
        ./cm_provisioning_code.c
        $<$<BOOL:${TFM_DUMMY_PROVISIONING}>:./cm_dummy_provisioning_data.c>
)

target_include_directories(cm_provisioning_bundle
    PRIVATE
        ../bundle_common
)

target_link_libraries(cm_provisioning_bundle
    platform_bl1
    bl1_1_shared_lib
)

add_custom_target(encrypted_cm_provisioning_bundle
    ALL
    SOURCES encrypted_cm_provisioning_bundle.bin
)

add_custom_command(OUTPUT encrypted_cm_provisioning_bundle.bin
    DEPENDS $<TARGET_FILE_DIR:cm_provisioning_bundle>/cm_provisioning_bundle.axf
    DEPENDS $<TARGET_FILE_DIR:bl1_2>/bl1_2_padded_hash.bin
    DEPENDS $<TARGET_FILE_DIR:bl1_2>/bl1_2_padded.bin
    bl1_2_padded_bin bl2_signed_bin cm_provisioning_bundle
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../rss_create_provisioning_bundle.py
    COMMAND ${CROSS_COMPILE}-objcopy $<TARGET_FILE_DIR:cm_provisioning_bundle>/cm_provisioning_bundle.axf
                    --dump-section CODE=cm_provisioning_code.bin
    COMMAND ${CROSS_COMPILE}-objcopy $<TARGET_FILE_DIR:cm_provisioning_bundle>/cm_provisioning_bundle.axf
                    --dump-section DATA=cm_provisioning_data.bin
    COMMAND ${CROSS_COMPILE}-objcopy $<TARGET_FILE_DIR:cm_provisioning_bundle>/cm_provisioning_bundle.axf
                    --dump-section VALUES=cm_provisioning_values.bin
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../rss_create_provisioning_bundle.py
                    --provisioning_code cm_provisioning_code.bin
                    --provisioning_data cm_provisioning_data.bin
                    --provisioning_values cm_provisioning_values.bin
                    --bl1_2_padded_hash_input_file $<TARGET_FILE_DIR:bl1_2>/bl1_2_padded_hash.bin
                    --bl1_2_input_file $<TARGET_FILE_DIR:bl1_2>/bl1_2_padded.bin
                    --bundle_output_file encrypted_cm_provisioning_bundle.bin
                    --magic "0xC0DEFEED"
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/encrypted_cm_provisioning_bundle.bin ${CMAKE_BINARY_DIR}/bin/encrypted_cm_provisioning_bundle.bin
)
