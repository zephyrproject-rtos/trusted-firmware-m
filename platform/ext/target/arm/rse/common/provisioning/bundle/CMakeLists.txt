#-------------------------------------------------------------------------------
# SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

find_package(Python3)


file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/config)

add_custom_target(provisioning_tooling_otp_config
    ALL
    SOURCES ${CMAKE_CURRENT_BINARY_DIR}/config/otp_config.pickle
)

add_custom_target(provisioning_tooling_message_config
    ALL
    SOURCES ${CMAKE_CURRENT_BINARY_DIR}/config/message_config.pickle
)

add_custom_target(provisioning_tooling_provisioning_config
    ALL
    SOURCES ${CMAKE_CURRENT_BINARY_DIR}/config/provisioning_config.pickle
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/config/otp_config.pickle
    DEPENDS ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/modules/otp_config.py
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/modules/otp_config.py
        --rse_otp_layout_h_file=${CMAKE_CURRENT_SOURCE_DIR}/../../rse_otp_layout.h
        --compile_commands_file=${CMAKE_BINARY_DIR}/compile_commands.json
        --otp_lcm_c_file=${CMAKE_CURRENT_SOURCE_DIR}/cm_provisioning_code.c
        --otp_config_output_file=${CMAKE_CURRENT_BINARY_DIR}/config/otp_config.pickle
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/config/message_config.pickle
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/modules/provisioning_message_config.py
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/modules/provisioning_message_config.py
        --rse_provisioning_message_h_file=${CMAKE_CURRENT_SOURCE_DIR}/../rse_provisioning_message.h
        --compile_commands_file=${CMAKE_BINARY_DIR}/compile_commands.json
        --otp_lcm_c_file=${CMAKE_CURRENT_SOURCE_DIR}/cm_provisioning_code.c
        --provisioning_message_config_output_file=${CMAKE_CURRENT_BINARY_DIR}/config/message_config.pickle
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/config/provisioning_config.pickle
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/modules/provisioning_config.py
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/modules/provisioning_config.py
        --rse_provisioning_layout_h_file=${CMAKE_CURRENT_SOURCE_DIR}/../rse_provisioning_values.h
        --compile_commands_file=${CMAKE_BINARY_DIR}/compile_commands.json
        --otp_lcm_c_file=${CMAKE_CURRENT_SOURCE_DIR}/cm_provisioning_code.c
        --provisioning_config_output_file=${CMAKE_CURRENT_BINARY_DIR}/config/provisioning_config.pickle
)

install(FILES       ${CMAKE_CURRENT_BINARY_DIR}/config/otp_config.pickle
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/provisioning/config)

install(FILES       ${CMAKE_CURRENT_BINARY_DIR}/config/message_config.pickle
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/provisioning/config)

install(FILES       ${CMAKE_CURRENT_BINARY_DIR}/config/provisioning_config.pickle
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/provisioning/config)

macro(create_provisioning_code_target target)
    add_executable(${target}_provisioning_code)

    set_target_properties(${target}_provisioning_code
        PROPERTIES
            SUFFIX ".axf"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
    )

    target_add_scatter_file(${target}_provisioning_code
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/provisioning_code.sct>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/provisioning_code.ld>
    )

    target_link_options(${target}_provisioning_code
        PRIVATE
        $<$<C_COMPILER_ID:GNU>:-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/bin/${target}_provisioning_code.map>
            $<$<C_COMPILER_ID:ARMClang>:--map>
            $<$<C_COMPILER_ID:IAR>:--map\;${CMAKE_CURRENT_BINARY_DIR}/bin/${target}_provisioning_code.map>
    )

    target_link_options(${target}_provisioning_code
        PRIVATE
            --entry=do_provision
    )

    target_sources(${target}_provisioning_code
        PRIVATE
            $<$<BOOL:${CONFIG_GNU_SYSCALL_STUB_ENABLED}>:${CMAKE_SOURCE_DIR}/platform/ext/common/syscalls_stub.c>
    )

    target_include_directories(${target}_provisioning_code
        PRIVATE
            ..
    )

    target_link_libraries(${target}_provisioning_code
        PRIVATE
            platform_bl1_1_interface
            bl1_1_shared_lib_interface
    )

    add_dependencies(${target}_provisioning_code
        bl1_1
    )

    # The provisioning code should link the ROM lib from ROM, not from SRAM. This
    # variable set only applies in this file scope
    set(CODE_SHARING_INPUT_FILE_SUFFIX ${CODE_SHARING_OUTPUT_FILE_SUFFIX})
    target_link_shared_code(${target}_provisioning_code
        bl1_1
    )
endmacro()

create_provisioning_code_target(cm)
create_provisioning_code_target(dm)
create_provisioning_code_target(combined)

target_sources(cm_provisioning_code
    PRIVATE
        ./cm_provisioning_code.c
)

target_sources(dm_provisioning_code
    PRIVATE
        ./dm_provisioning_code.c
)

target_compile_definitions(combined_provisioning_code
    PRIVATE
        RSE_COMBINED_PROVISIONING_BUNDLES
)

target_sources(combined_provisioning_code
    PRIVATE
        ./combined_provisioning_code.c
        ./cm_provisioning_code.c
        ./dm_provisioning_code.c
)

if (TFM_DUMMY_PROVISIONING)

    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/provisioning)

    set(CM_BUNDLE_SCRIPT_ARGS
    )


    add_custom_target(cm_provisioning_message
        ALL
        SOURCES ${CMAKE_BINARY_DIR}/bin/provisioning/cm_provisioning_message.bin
    )

    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/bin/provisioning/cm_provisioning_message.bin
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/create_cm_provisioning_bundle.py
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/config/otp_config.pickle
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/config/message_config.pickle
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/config/provisioning_config.pickle
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/bin/cm_provisioning_code.axf
        DEPENDS ${CMAKE_BINARY_DIR}/bin/keys/kprov_cm.bin
        DEPENDS ${CMAKE_BINARY_DIR}/bin/bl1_2_padded_hash.bin
        DEPENDS cm_provisioning_code
        DEPENDS cm_provisioning_key
        DEPENDS ${CMAKE_BINARY_DIR}/bin/bl1_2_padded_hash.bin
        DEPENDS ${CMAKE_BINARY_DIR}/bin/bl1_2_padded.bin
        DEPENDS bl1_2_padded_bin
        DEPENDS ${TFM_BL2_ENCRYPTION_KEY_PATH}
        COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/create_cm_provisioning_bundle.py
            --otp_config_file=${CMAKE_CURRENT_BINARY_DIR}/config/otp_config.pickle
            --provisioning_message_config_file=${CMAKE_CURRENT_BINARY_DIR}/config/message_config.pickle
            --provisioning_config_file=${CMAKE_CURRENT_BINARY_DIR}/config/provisioning_config.pickle
            --provisioning_code_elf_file=${CMAKE_CURRENT_BINARY_DIR}/bin/cm_provisioning_code.axf
            --authentication_symmetric_key_file=${CMAKE_BINARY_DIR}/bin/keys/kprov_cm.bin
            --bundle_output_file ${CMAKE_BINARY_DIR}/bin/provisioning/cm_provisioning_message.bin
            --tp_mode=${RSE_TP_MODE}

            --bl1_2_hash_path=${CMAKE_BINARY_DIR}/bin/bl1_2_padded_hash.bin
            --bl1_2.bl1_2_path=${CMAKE_BINARY_DIR}/bin/bl1_2_padded.bin
            --guk_path=${TFM_GUK_PATH}
            --otp_dma_ics_path=${CMAKE_BINARY_DIR}/bin/otp_dma_ics.bin
            --kce_cm_path=${TFM_BL2_ENCRYPTION_KEY_PATH}
            --cm.rotpk_0_path=${TFM_BL2_SIGNING_KEY_PATH}.pub
    )

    add_custom_target(dm_provisioning_message
        ALL
        SOURCES ${CMAKE_BINARY_DIR}/bin/provisioning/dm_provisioning_message.bin
    )

    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/bin/provisioning/dm_provisioning_message.bin
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/create_dm_provisioning_bundle.py
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/config/otp_config.pickle
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/config/message_config.pickle
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/config/provisioning_config.pickle
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/bin/dm_provisioning_code.axf
        DEPENDS ${CMAKE_BINARY_DIR}/bin/keys/kprov_dm.bin
        DEPENDS dm_provisioning_code
        DEPENDS dm_provisioning_key
        COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/create_dm_provisioning_bundle.py
            --otp_config_file=${CMAKE_CURRENT_BINARY_DIR}/config/otp_config.pickle
            --provisioning_message_config_file=${CMAKE_CURRENT_BINARY_DIR}/config/message_config.pickle
            --provisioning_config_file=${CMAKE_CURRENT_BINARY_DIR}/config/provisioning_config.pickle
            --provisioning_code_elf_file=${CMAKE_CURRENT_BINARY_DIR}/bin/dm_provisioning_code.axf
            --authentication_symmetric_key_file=${CMAKE_BINARY_DIR}/bin/keys/kprov_dm.bin
            --bundle_output_file ${CMAKE_BINARY_DIR}/bin/provisioning/dm_provisioning_message.bin
            --tp_mode=${RSE_TP_MODE}
    )

    add_custom_target(combined_provisioning_message
        ALL
        SOURCES ${CMAKE_BINARY_DIR}/bin/provisioning/combined_provisioning_message.bin
    )

    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/bin/provisioning/combined_provisioning_message.bin
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/create_combined_provisioning_bundle.py
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/config/otp_config.pickle
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/config/message_config.pickle
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/config/provisioning_config.pickle
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/bin/combined_provisioning_code.axf
        DEPENDS ${CMAKE_BINARY_DIR}/bin/keys/kprov_cm.bin
        DEPENDS combined_provisioning_code
        DEPENDS cm_provisioning_key
        DEPENDS ${CMAKE_BINARY_DIR}/bin/bl1_2_padded_hash.bin
        DEPENDS ${CMAKE_BINARY_DIR}/bin/bl1_2_padded.bin
        DEPENDS bl1_2_padded_bin
        COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/create_combined_provisioning_bundle.py
            --otp_config_file=${CMAKE_CURRENT_BINARY_DIR}/config/otp_config.pickle
            --provisioning_message_config_file=${CMAKE_CURRENT_BINARY_DIR}/config/message_config.pickle
            --provisioning_config_file=${CMAKE_CURRENT_BINARY_DIR}/config/provisioning_config.pickle
            --provisioning_code_elf_file=${CMAKE_CURRENT_BINARY_DIR}/bin/combined_provisioning_code.axf
            --authentication_symmetric_key_file=${CMAKE_BINARY_DIR}/bin/keys/kprov_cm.bin
            --bundle_output_file ${CMAKE_BINARY_DIR}/bin/provisioning/combined_provisioning_message.bin
            --tp_mode=${RSE_TP_MODE}
            --cm:bl1_2_hash_path=${CMAKE_BINARY_DIR}/bin/bl1_2_padded_hash.bin
            --cm:bl1_2.bl1_2_path=${CMAKE_BINARY_DIR}/bin/bl1_2_padded.bin
            --cm:guk_path=${TFM_GUK_PATH}
            --cm:otp_dma_ics_path=${CMAKE_BINARY_DIR}/bin/otp_dma_ics.bin
            --cm:kce_cm_path=${TFM_BL2_ENCRYPTION_KEY_PATH}
            --cm:cm.rotpk_0_path=${TFM_BL2_SIGNING_KEY_PATH}.pub
    )
endif()
