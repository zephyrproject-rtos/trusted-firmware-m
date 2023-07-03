#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------


find_package(Python3)

add_executable(provisioning_bundle)

set_target_properties(provisioning_bundle
    PROPERTIES
        SUFFIX ".axf"
)

target_add_scatter_file(provisioning_bundle
    $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/provisioning_bundle.sct>
    $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/provisioning_bundle.ld>
)

target_link_options(provisioning_bundle
    PRIVATE
        $<$<C_COMPILER_ID:GNU>:-Wl,-Map=${CMAKE_BINARY_DIR}/bin/provisioning_bundle.map>
        $<$<C_COMPILER_ID:ARMClang>:--map>
        $<$<C_COMPILER_ID:IAR>:--map\;${CMAKE_BINARY_DIR}/bin/provisioning_bundle.map>
)

target_link_options(provisioning_bundle
    PRIVATE
        --entry=do_provision
)

target_sources(provisioning_bundle
    PRIVATE
        ./provisioning_code.c
        $<$<BOOL:${TFM_DUMMY_PROVISIONING}>:./dummy_provisioning_data.c>
)

target_include_directories(provisioning_bundle
    PRIVATE
        .
)

target_link_libraries(provisioning_bundle
    platform_s
    psa_interface
)

target_compile_definitions(provisioning_bundle
    PRIVATE
        $<$<BOOL:${PLATFORM_DEFAULT_CRYPTO_KEYS}>:PLATFORM_DEFAULT_CRYPTO_KEYS>
        $<$<BOOL:${PLATFORM_DEFAULT_OTP}>:PLATFORM_DEFAULT_OTP>
        $<$<BOOL:${SYMMETRIC_INITIAL_ATTESTATION}>:SYMMETRIC_INITIAL_ATTESTATION>
        $<$<BOOL:${TFM_DUMMY_PROVISIONING}>:TFM_DUMMY_PROVISIONING>
        $<$<BOOL:${PLATFORM_DEFAULT_NV_COUNTERS}>:PLATFORM_DEFAULT_NV_COUNTERS>
        $<$<BOOL:${PLATFORM_DEFAULT_OTP_WRITEABLE}>:OTP_WRITEABLE>
)

add_custom_target(encrypted_provisioning_bundle
    ALL
    SOURCES encrypted_provisioning_bundle.bin
)

add_custom_command(OUTPUT encrypted_provisioning_bundle.bin
    DEPENDS $<TARGET_FILE_DIR:provisioning_bundle>/provisioning_bundle.axf
    DEPENDS provisioning_bundle
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/create_provisioning_bundle.py
    COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE_DIR:provisioning_bundle>/provisioning_bundle.axf
                    --dump-section CODE=provisioning_code.bin
    COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE_DIR:provisioning_bundle>/provisioning_bundle.axf
                    --dump-section RO_DATA=provisioning_rodata.bin
    COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE_DIR:provisioning_bundle>/provisioning_bundle.axf
                    --dump-section RW_DATA=provisioning_rwdata.bin
    COMMAND ${CMAKE_OBJCOPY} $<TARGET_FILE_DIR:provisioning_bundle>/provisioning_bundle.axf
                    --dump-section VALUES=provisioning_values.bin
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/create_provisioning_bundle.py
                    --provisioning_code provisioning_code.bin
                    --provisioning_rwdata provisioning_rwdata.bin
                    --provisioning_rodata provisioning_rodata.bin
                    --provisioning_values provisioning_values.bin
                    --bundle_output_file encrypted_provisioning_bundle.bin
                    --magic "0xC0DEFEED"
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/encrypted_provisioning_bundle.bin ${CMAKE_BINARY_DIR}/bin/encrypted_provisioning_bundle.bin
)



target_sources(platform_s
    PRIVATE
        ./runtime_stub_provisioning.c
)

target_sources(platform_bl2
    PRIVATE
        ./bl2_provisioning.c
)

target_include_directories(platform_bl2
    INTERFACE
        .
)
