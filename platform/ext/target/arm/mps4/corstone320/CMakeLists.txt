#-------------------------------------------------------------------------------
# Copyright (c) 2022-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)

include(${MPS4_COMMON_DIR}/common.cmake)

#========================= Device definition lib ===============================#

target_include_directories(device_definition
    INTERFACE
        device/include
        native_drivers
        ${CMAKE_SOURCE_DIR}
)

#========================= Platform Secure ====================================#

target_include_directories(platform_s
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        cmsis_drivers/config/secure
        cmsis_drivers
        device
)

target_sources(platform_s
    PRIVATE
        cmsis_drivers/Driver_PPC.c
        device/source/platform_s_device_definition.c
        native_drivers/ppc_corstone320_drv.c
        ${ETHOS_DRIVER_PATH}/src/ethosu_device_u85.c
)

target_compile_definitions(platform_s
    PRIVATE
        # Needed for DMA-350 library
        CMSIS_device_header=<corstone320.h>
)

#========================= Platform BL2 =======================================#

if(BL2)
target_sources(platform_bl2
        PRIVATE
            device/source/platform_s_device_definition.c
    )

    target_include_directories(platform_bl2
        PUBLIC
            device/include
        PRIVATE
            .
            device
            services/src
            ${CMAKE_CURRENT_SOURCE_DIR}/device/config
            cmsis_drivers/config/secure
    )
endif()

#========================= tfm_spm ============================================#

target_sources(tfm_spm
    PRIVATE
        target_cfg.c
)

#========================= platform_region_defs ===============================#
target_include_directories(platform_region_defs
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

#========================= Flash driver library ===============================#
target_include_directories(flash_drivers
    INTERFACE
        cmsis_drivers/config/secure
)

#========================= Platform BL1_1 =======================================#

target_sources(platform_bl1_1
    PRIVATE
        device/source/platform_s_device_definition.c
)

target_include_directories(platform_bl1_1
    PRIVATE
        cmsis_drivers/config/secure
)

#========================= Platform BL1_2 =====================================#

target_include_directories(platform_bl1_2
    PRIVATE
        cmsis_drivers/config/secure
)

#========================= Files for building NS side platform ================#
install(DIRECTORY   device
                    native_drivers
                    cmsis_drivers
        DESTINATION ${INSTALL_PLATFORM_NS_DIR})

install(FILES       config.cmake
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/include)
