#-------------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)
set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../common common)

#========================= Platform region defs ===============================#

target_include_directories(platform_region_defs
    INTERFACE
        .
)

target_compile_definitions(platform_region_defs
    INTERFACE
        $<$<BOOL:${RSS_XIP}>:RSS_XIP>
)

#========================= Platform BL2 =======================================#

target_sources(platform_bl2
    PRIVATE
        bl2/boot_hal_bl2.c
        bl2/flash_map_bl2.c
        bl2/host_flash_atu.c
        $<$<BOOL:${RSS_XIP}>:${CMAKE_CURRENT_SOURCE_DIR}/bl2/sic_boot.c>
)

target_compile_definitions(platform_bl2
    PUBLIC
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:PLATFORM_HAS_BOOT_DMA>
        $<$<BOOL:${PLATFORM_BOOT_DMA_MIN_SIZE_REQ}>:BOOT_DMA_MIN_SIZE_REQ=${PLATFORM_BOOT_DMA_MIN_SIZE_REQ}>
)

target_include_directories(platform_bl2
    PUBLIC
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:${CMAKE_CURRENT_SOURCE_DIR}/../common/bl2>
)

############################### IMAGE SIGNING ##################################

if (RSS_XIP)
    find_package(Python3)

    add_custom_target(tfm_s_sic_tables
        SOURCES bin/tfm_s_sic_tables.bin
    )

    add_custom_command(OUTPUT bin/tfm_s_sic_tables.bin
        DEPENDS $<TARGET_FILE_DIR:tfm_s>/tfm_s.bin
        DEPENDS tfm_s_bin
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bl2/create_xip_tables.py
            --input_image $<TARGET_FILE_DIR:tfm_s>/tfm_s.bin
            --table_output_file tfm_s_sic_tables.bin
            --encrypted_image_output_file tfm_s_encrypted.bin
            --image_version ${MCUBOOT_SECURITY_COUNTER_S}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tfm_s_sic_tables.bin $<TARGET_FILE_DIR:tfm_s>
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tfm_s_encrypted.bin $<TARGET_FILE_DIR:tfm_s>
    )

    add_custom_target(tfm_ns_sic_tables
        SOURCES bin/tfm_ns_sic_tables.bin
    )

    add_custom_command(OUTPUT bin/tfm_ns_sic_tables.bin
        DEPENDS $<TARGET_FILE_DIR:tfm_ns>/tfm_ns.bin
        DEPENDS tfm_ns_bin
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bl2/create_xip_tables.py
            --input_image $<TARGET_FILE_DIR:tfm_ns>/tfm_ns.bin
            --table_output_file tfm_ns_sic_tables.bin
            --encrypted_image_output_file tfm_ns_encrypted.bin
            --image_version ${MCUBOOT_SECURITY_COUNTER_NS}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tfm_ns_sic_tables.bin $<TARGET_FILE_DIR:tfm_ns>
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tfm_ns_encrypted.bin $<TARGET_FILE_DIR:tfm_s>
    )

    set(IMAGE_TYPE "S_IMAGE")
    set(FLASH_AREA_NUM ${MCUBOOT_S_IMAGE_FLASH_AREA_NUM})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bl2/signing_layout_sic_tables.c.in signing_layout_s.c @ONLY)
    add_library(signing_layout_s OBJECT ${CMAKE_CURRENT_BINARY_DIR}/signing_layout_s.c)

    target_compile_options(signing_layout_s
        PRIVATE
            $<$<C_COMPILER_ID:GNU>:-E\;-xc>
            $<$<C_COMPILER_ID:ARMClang>:-E\;-xc>
            $<$<C_COMPILER_ID:IAR>:--preprocess=ns\;$<TARGET_OBJECTS:signing_layout_s>>
    )
    target_compile_definitions(signing_layout_s
        PRIVATE
            $<$<BOOL:${BL2}>:BL2>
            $<$<BOOL:${MCUBOOT_IMAGE_NUMBER}>:MCUBOOT_IMAGE_NUMBER=${MCUBOOT_IMAGE_NUMBER}>
            $<$<STREQUAL:${MCUBOOT_UPGRADE_STRATEGY},DIRECT_XIP>:IMAGE_ROM_FIXED>
    )
    target_link_libraries(signing_layout_s
        PRIVATE
            platform_bl2
    )

    add_custom_target(tfm_s_sic_tables_signed_bin
        SOURCES bin/tfm_s_sic_tables_signed.bin
    )
    add_custom_command(OUTPUT bin/tfm_s_sic_tables_signed.bin
        DEPENDS tfm_s_sic_tables
        DEPENDS tfm_s_bin signing_layout_s
        WORKING_DIRECTORY ${MCUBOOT_PATH}/scripts

        #Sign secure binary image with provided secret key
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/bl2/ext/mcuboot/scripts/wrapper/wrapper.py
            -v ${MCUBOOT_IMAGE_VERSION_S}
            --layout $<TARGET_OBJECTS:signing_layout_s>
            -k ${MCUBOOT_KEY_S}
            --public-key-format $<IF:$<BOOL:${MCUBOOT_HW_KEY}>,full,hash>
            --align ${MCUBOOT_ALIGN_VAL}
            --pad
            --pad-header
            -H ${BL2_HEADER_SIZE}
            -s ${MCUBOOT_SECURITY_COUNTER_S}
            -L ${MCUBOOT_ENC_KEY_LEN}
            -d \"\(1,${MCUBOOT_NS_IMAGE_MIN_VER}\)\"
            $<$<STREQUAL:${MCUBOOT_UPGRADE_STRATEGY},OVERWRITE_ONLY>:--overwrite-only>
            $<$<BOOL:${MCUBOOT_ENC_IMAGES}>:-E${MCUBOOT_KEY_ENC}>
            $<$<BOOL:${MCUBOOT_MEASURED_BOOT}>:--measured-boot-record>
            $<TARGET_FILE_DIR:tfm_s>/tfm_s_sic_tables.bin
            ${CMAKE_CURRENT_BINARY_DIR}/tfm_s_sic_tables_signed.bin
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tfm_s_sic_tables_signed.bin $<TARGET_FILE_DIR:bl2>
    )

    set(IMAGE_TYPE "NS_IMAGE")
    set(FLASH_AREA_NUM ${MCUBOOT_NS_IMAGE_FLASH_AREA_NUM})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bl2/signing_layout_sic_tables.c.in signing_layout_ns.c @ONLY)
    add_library(signing_layout_ns OBJECT ${CMAKE_CURRENT_BINARY_DIR}/signing_layout_ns.c)

    target_compile_options(signing_layout_ns
        PRIVATE
            $<$<C_COMPILER_ID:GNU>:-E\;-xc>
            $<$<C_COMPILER_ID:ARMClang>:-E\;-xc>
            $<$<C_COMPILER_ID:IAR>:--preprocess=ns\;$<TARGET_OBJECTS:signing_layout_ns>>
    )
    target_compile_definitions(signing_layout_ns
        PRIVATE
            $<$<BOOL:${BL2}>:BL2>
            $<$<BOOL:${MCUBOOT_IMAGE_NUMBER}>:MCUBOOT_IMAGE_NUMBER=${MCUBOOT_IMAGE_NUMBER}>
            $<$<STREQUAL:${MCUBOOT_UPGRADE_STRATEGY},DIRECT_XIP>:IMAGE_ROM_FIXED>
    )
    target_link_libraries(signing_layout_ns
        PRIVATE
            platform_bl2
    )

    add_custom_target(tfm_ns_sic_tables_signed_bin
        SOURCES bin/tfm_ns_sic_tables_signed.bin
    )
    add_custom_command(OUTPUT bin/tfm_ns_sic_tables_signed.bin
        DEPENDS tfm_ns_sic_tables
        DEPENDS tfm_ns_bin signing_layout_ns
        WORKING_DIRECTORY ${MCUBOOT_PATH}/scripts

        #Sign non-secure binary image with provided secret key
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/bl2/ext/mcuboot/scripts/wrapper/wrapper.py
            -v ${MCUBOOT_IMAGE_VERSION_NS}
            --layout $<TARGET_OBJECTS:signing_layout_ns>
            -k ${MCUBOOT_KEY_NS}
            --public-key-format $<IF:$<BOOL:${MCUBOOT_HW_KEY}>,full,hash>
            --align ${MCUBOOT_ALIGN_VAL}
            --pad
            --pad-header
            -H ${BL2_HEADER_SIZE}
            -s ${MCUBOOT_SECURITY_COUNTER_NS}
            -L ${MCUBOOT_ENC_KEY_LEN}
            -d \"\(0, ${MCUBOOT_S_IMAGE_MIN_VER}\)\"
            $<TARGET_FILE_DIR:tfm_ns>/tfm_ns_sic_tables.bin
            $<$<STREQUAL:${MCUBOOT_UPGRADE_STRATEGY},OVERWRITE_ONLY>:--overwrite-only>
            $<$<BOOL:${MCUBOOT_ENC_IMAGES}>:-E${MCUBOOT_KEY_ENC}>
            $<$<BOOL:${MCUBOOT_MEASURED_BOOT}>:--measured-boot-record>
            ${CMAKE_CURRENT_BINARY_DIR}/tfm_ns_sic_tables_signed.bin
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tfm_ns_sic_tables_signed.bin $<TARGET_FILE_DIR:bl2>
    )

    add_custom_target(signed_images
        ALL
        DEPENDS bin/tfm_s_sic_tables_signed.bin
        DEPENDS bin/tfm_ns_sic_tables_signed.bin
    )
endif()
