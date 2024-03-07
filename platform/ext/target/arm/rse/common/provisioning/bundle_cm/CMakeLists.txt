#-------------------------------------------------------------------------------
# Copyright (c) 2023-2024, Arm Limited. All rights reserved.
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
        $<$<BOOL:${CONFIG_GNU_SYSCALL_STUB_ENABLED}>:${CMAKE_SOURCE_DIR}/platform/ext/common/syscalls_stub.c>
)

target_include_directories(cm_provisioning_bundle
    PRIVATE
        ../bundle_common
)

target_link_libraries(cm_provisioning_bundle
    PRIVATE
        platform_bl1_1_interface
        bl1_1_shared_lib_interface
)

# The provisioning code should link the ROM lib from ROM, not from SRAM. This
# variable set only applies in this file scope
set(CODE_SHARING_INPUT_FILE_SUFFIX ${CODE_SHARING_OUTPUT_FILE_SUFFIX})
target_link_shared_code(cm_provisioning_bundle
    bl1_1
)

math(EXPR RSE_AMOUNT_LOOP_RANGE "${RSE_AMOUNT} - 1")
foreach(RSE_ID RANGE ${RSE_AMOUNT_LOOP_RANGE})
    add_custom_target(encrypted_cm_provisioning_bundle_${RSE_ID}
        ALL
        SOURCES encrypted_cm_provisioning_bundle_${RSE_ID}.bin
    )

    add_custom_command(OUTPUT encrypted_cm_provisioning_bundle_${RSE_ID}.bin
        DEPENDS $<TARGET_FILE_DIR:cm_provisioning_bundle>/cm_provisioning_bundle.axf
        DEPENDS $<TARGET_FILE_DIR:bl1_2>/bl1_2_padded_hash.bin
        DEPENDS $<TARGET_FILE_DIR:bl1_2>/bl1_2_padded.bin
        bl1_2_padded_bin bl2_signed_bin cm_provisioning_bundle
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/create_cm_provisioning_bundle.py
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/../cm_provisioning_key.bin
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE_DIR:cm_provisioning_bundle>/cm_provisioning_bundle.axf cm_provisioning_bundle_${RSE_ID}.axf
        COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE_DIR:cm_provisioning_bundle>/cm_provisioning_bundle_${RSE_ID}.axf
                        --dump-section CODE=cm_provisioning_code.bin
        COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE_DIR:cm_provisioning_bundle>/cm_provisioning_bundle_${RSE_ID}.axf
                        --dump-section DATA=cm_provisioning_data.bin
        COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE_DIR:cm_provisioning_bundle>/cm_provisioning_bundle_${RSE_ID}.axf
                        --dump-section VALUES=cm_provisioning_values.bin
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/create_cm_provisioning_bundle.py
                        --provisioning_code cm_provisioning_code.bin
                        --provisioning_data cm_provisioning_data.bin
                        --provisioning_values cm_provisioning_values.bin
                        --bl1_2_padded_hash_input_file $<TARGET_FILE_DIR:bl1_2>/bl1_2_padded_hash.bin
                        --bl1_2_input_file $<TARGET_FILE_DIR:bl1_2>/bl1_2_padded.bin
                        --otp_dma_ics_input_file=$<TARGET_FILE_DIR:bl1_2>/otp_dma_ics.bin
                        --cm_provisioning_key_file=${CMAKE_CURRENT_BINARY_DIR}/../cm_provisioning_key.bin
                        --bundle_output_file encrypted_cm_provisioning_bundle_${RSE_ID}.bin
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/encrypted_cm_provisioning_bundle_${RSE_ID}.bin ${CMAKE_BINARY_DIR}/bin/encrypted_cm_provisioning_bundle_${RSE_ID}.bin
    )
endforeach()
