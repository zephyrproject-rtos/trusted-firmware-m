#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.15)

# This CMake script is prepard by TF-M for building the non-secure side
# application and not used in secure build a tree being for export only.
# This file is renamed to spe/CMakeList.txt during installation phase

include(spe_config)
include(spe_export)

set_target_properties(tfm_config psa_interface PROPERTIES IMPORTED_GLOBAL True)
target_link_libraries(tfm_config INTERFACE psa_interface)

# In actual NS integration, NS side build should include the source files
# exported by TF-M build.
set(INTERFACE_SRC_DIR    ${CMAKE_CURRENT_LIST_DIR}/interface/src)
set(INTERFACE_INC_DIR    ${CMAKE_CURRENT_LIST_DIR}/interface/include)

add_library(tfm_api_ns STATIC)

target_sources(tfm_api_ns
    PRIVATE
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${INTERFACE_SRC_DIR}/tfm_platform_api.c>
        $<$<BOOL:${TFM_PARTITION_PROTECTED_STORAGE}>:${INTERFACE_SRC_DIR}/tfm_ps_api.c>
        $<$<BOOL:${TFM_PARTITION_INTERNAL_TRUSTED_STORAGE}>:${INTERFACE_SRC_DIR}/tfm_its_api.c>
        $<$<BOOL:${TFM_PARTITION_CRYPTO}>:${INTERFACE_SRC_DIR}/tfm_crypto_api.c>
        $<$<BOOL:${TFM_PARTITION_INITIAL_ATTESTATION}>:${INTERFACE_SRC_DIR}/tfm_attest_api.c>
        $<$<BOOL:${TFM_PARTITION_FIRMWARE_UPDATE}>:${INTERFACE_SRC_DIR}/tfm_fwu_api.c>
)

target_sources(tfm_api_ns
    PRIVATE
        $<$<BOOL:${TFM_PARTITION_NS_AGENT_MAILBOX}>:${INTERFACE_SRC_DIR}/multi_core/tfm_multi_core_ns_api.c>
        $<$<BOOL:${TFM_PARTITION_NS_AGENT_MAILBOX}>:${INTERFACE_SRC_DIR}/multi_core/tfm_multi_core_psa_ns_api.c>
        $<$<BOOL:${CONFIG_TFM_USE_TRUSTZONE}>:${INTERFACE_SRC_DIR}/tfm_psa_ns_api.c>
)

# Include interface headers exported by TF-M
target_include_directories(tfm_api_ns
    PUBLIC
        ${INTERFACE_INC_DIR}
        ${INTERFACE_INC_DIR}/crypto_keys
        $<$<BOOL:${TFM_PARTITION_NS_AGENT_MAILBOX}>:${INTERFACE_INC_DIR}/multi_core>
)

add_library(platform_region_defs INTERFACE)

target_compile_definitions(platform_region_defs
    INTERFACE
        $<$<BOOL:${BL1}>:BL1>
        $<$<BOOL:${BL2}>:BL2>
        BL2_HEADER_SIZE=${BL2_HEADER_SIZE}
        BL2_TRAILER_SIZE=${BL2_TRAILER_SIZE}
        BL1_HEADER_SIZE=${BL1_HEADER_SIZE}
        BL1_TRAILER_SIZE=${BL1_TRAILER_SIZE}
        $<$<BOOL:${MCUBOOT_IMAGE_NUMBER}>:MCUBOOT_IMAGE_NUMBER=${MCUBOOT_IMAGE_NUMBER}>
        $<$<BOOL:${TEST_PSA_API}>:PSA_API_TEST_${TEST_PSA_API}>
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:relwithdebinfo>>:ENABLE_HEAP>
)

target_link_libraries(platform_region_defs
    INTERFACE
        tfm_config
)

add_subdirectory(platform)

target_sources(platform_ns
    PRIVATE
        $<$<BOOL:${PLATFORM_DEFAULT_UART_STDOUT}>:${CMAKE_CURRENT_SOURCE_DIR}/platform/ext/common/uart_stdout.c>
)

target_link_libraries(tfm_api_ns
    PUBLIC
        platform_region_defs
        $<$<BOOL:${CONFIG_TFM_USE_TRUSTZONE}>:${CMAKE_CURRENT_SOURCE_DIR}/interface/lib/s_veneers.o>
    PRIVATE
        platform_ns
)

if(BL2 AND PLATFORM_DEFAULT_IMAGE_SIGNING)

    if (MCUBOOT_IMAGE_NUMBER GREATER 1)

        add_custom_target(tfm_app_binaries
            DEPENDS tfm_ns_binaries
            DEPENDS $<IF:$<BOOL:${MCUBOOT_GENERATE_SIGNING_KEYPAIR}>,generated_private_key,>
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/image_signing/scripts

            #Sign non-secure binary image with provided secret key
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/image_signing/scripts/wrapper/wrapper.py
                --version ${MCUBOOT_IMAGE_VERSION_NS}
                --layout ${CMAKE_CURRENT_SOURCE_DIR}/image_signing/layout_files/signing_layout_ns.o
                --key ${MCUBOOT_KEY_NS}
                --public-key-format $<IF:$<BOOL:${MCUBOOT_HW_KEY}>,full,hash>
                --align ${MCUBOOT_ALIGN_VAL}
                --pad
                --pad-header
                -H ${BL2_HEADER_SIZE}
                -s ${MCUBOOT_SECURITY_COUNTER_NS}
                -L ${MCUBOOT_ENC_KEY_LEN}
                -d \"\(0, ${MCUBOOT_S_IMAGE_MIN_VER}\)\"
                ${CMAKE_BINARY_DIR}/bin/tfm_ns.bin
                $<$<STREQUAL:${MCUBOOT_UPGRADE_STRATEGY},OVERWRITE_ONLY>:--overwrite-only>
                $<$<BOOL:${MCUBOOT_CONFIRM_IMAGE}>:--confirm>
                $<$<BOOL:${MCUBOOT_ENC_IMAGES}>:-E${MCUBOOT_KEY_ENC}>
                $<$<BOOL:${MCUBOOT_MEASURED_BOOT}>:--measured-boot-record>
                ${CMAKE_BINARY_DIR}/bin/tfm_ns_signed.bin

            # Create concatenated binary image from the two independently signed
            # binary file. This only uses the local assemble.py script (not from
            # upstream mcuboot) because that script is geared towards zephyr
            # support
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/image_signing/scripts/assemble.py
                --layout ${CMAKE_CURRENT_SOURCE_DIR}/image_signing/layout_files/signing_layout_s.o
                --secure ${CMAKE_CURRENT_SOURCE_DIR}/bin/tfm_s_signed.bin
                --non_secure ${CMAKE_BINARY_DIR}/bin/tfm_ns_signed.bin
                --output ${CMAKE_BINARY_DIR}/tfm_s_ns_signed.bin
            # merge bootloader and application into Hex image for upload
            COMMAND srec_cat ${CMAKE_CURRENT_SOURCE_DIR}/bin/bl2.bin -Binary -offset 0xA000000
                ${CMAKE_BINARY_DIR}/tfm_s_ns_signed.bin -Binary -offset 0xA020000
                -o ${CMAKE_BINARY_DIR}/tfm.hex -Intel
        )
    else()
        add_custom_target(tfm_app_binaries
            DEPENDS tfm_ns_binaries
            DEPENDS $<IF:$<BOOL:${MCUBOOT_GENERATE_SIGNING_KEYPAIR}>,generated_private_key,>
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/image_signing/scripts

            # concatenate S + NS binaries into tfm_s_ns.bin
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/image_signing/scripts/assemble.py
                --layout ${CMAKE_CURRENT_SOURCE_DIR}/image_signing/layout_files/signing_layout_s_ns.o
                --secure ${CMAKE_CURRENT_SOURCE_DIR}/bin/tfm_s.bin
                --non_secure ${CMAKE_BINARY_DIR}/bin/tfm_ns.bin
                --output ${CMAKE_BINARY_DIR}/bin/tfm_s_ns.bin

            # sign the combined tfm_s_ns.bin file
            COMMAND ${Python3_EXECUTABLE}
                ${CMAKE_CURRENT_SOURCE_DIR}/image_signing/scripts/wrapper/wrapper.py
                --version ${MCUBOOT_IMAGE_VERSION_S}
                --layout ${CMAKE_CURRENT_SOURCE_DIR}/image_signing/layout_files/signing_layout_s_ns.o
                --key ${MCUBOOT_KEY_S}
                --public-key-format $<IF:$<BOOL:${MCUBOOT_HW_KEY}>,full,hash>
                --align ${MCUBOOT_ALIGN_VAL}
                --pad
                --pad-header
                -H ${BL2_HEADER_SIZE}
                -s ${MCUBOOT_SECURITY_COUNTER_S}
                -L ${MCUBOOT_ENC_KEY_LEN}
                $<$<STREQUAL:${MCUBOOT_UPGRADE_STRATEGY},OVERWRITE_ONLY>:--overwrite-only>
                $<$<BOOL:${MCUBOOT_CONFIRM_IMAGE}>:--confirm>
                $<$<BOOL:${MCUBOOT_ENC_IMAGES}>:-E${MCUBOOT_KEY_ENC}>
                $<$<BOOL:${MCUBOOT_MEASURED_BOOT}>:--measured-boot-record>
                ${CMAKE_BINARY_DIR}/bin/tfm_s_ns.bin
                ${CMAKE_BINARY_DIR}/tfm_s_ns_signed.bin
        )
    endif()
endif()
