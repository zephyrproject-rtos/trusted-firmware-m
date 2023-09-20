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

add_library(tfm_api_ns)

file(GLOB spe_sources "interface/src/*.c")

target_sources(tfm_api_ns
    PRIVATE
        ${spe_sources}
        # NS specific implementation of NS interface dispatcher
        $<$<BOOL:${CONFIG_TFM_USE_TRUSTZONE}>:interface/src/os_wrapper/tfm_ns_interface_rtos.c>
)

target_include_directories(tfm_api_ns
    PUBLIC
        interface/include
)

add_subdirectory(platform)

target_link_libraries(tfm_api_ns
    PUBLIC
        platform_ns
        tfm_config
        $<$<BOOL:${CONFIG_TFM_USE_TRUSTZONE}>:${CMAKE_CURRENT_SOURCE_DIR}/interface/lib/s_veneers.o>
)

target_add_scatter_file()

add_custom_target(tfm_ns_binaries
    DEPENDS tfm_ns
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:tfm_ns> ${CMAKE_BINARY_DIR}/tfm_ns.bin
    COMMAND ${CMAKE_OBJCOPY} -O elf32-littlearm $<TARGET_FILE:tfm_ns>  ${CMAKE_BINARY_DIR}/tfm_ns.elf
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:tfm_ns> ${CMAKE_BINARY_DIR}/tfm_ns.hex
)

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
            ${CMAKE_BINARY_DIR}/tfm_ns.bin
            $<$<STREQUAL:${MCUBOOT_UPGRADE_STRATEGY},OVERWRITE_ONLY>:--overwrite-only>
            $<$<BOOL:${MCUBOOT_CONFIRM_IMAGE}>:--confirm>
            $<$<BOOL:${MCUBOOT_ENC_IMAGES}>:-E${MCUBOOT_KEY_ENC}>
            $<$<BOOL:${MCUBOOT_MEASURED_BOOT}>:--measured-boot-record>
            ${CMAKE_BINARY_DIR}/tfm_ns_signed.bin

        # Create concatenated binary image from the two independently signed
        # binary file. This only uses the local assemble.py script (not from
        # upstream mcuboot) because that script is geared towards zephyr
        # support
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/image_signing/scripts/assemble.py
            --layout ${CMAKE_CURRENT_SOURCE_DIR}/image_signing/layout_files/signing_layout_s.o
            --secure ${CMAKE_CURRENT_SOURCE_DIR}/bin/tfm_s_signed.bin
            --non_secure ${CMAKE_BINARY_DIR}/tfm_ns_signed.bin
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
            --non_secure ${CMAKE_BINARY_DIR}/tfm_ns.bin
            --output ${CMAKE_BINARY_DIR}/tfm_s_ns.bin

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
            ${CMAKE_BINARY_DIR}/tfm_s_ns.bin
            ${CMAKE_BINARY_DIR}/tfm_s_ns_signed.bin
    )
endif()

