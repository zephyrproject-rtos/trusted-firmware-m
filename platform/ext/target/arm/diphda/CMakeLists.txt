#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)
set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

#========================= Platform region defs ===============================#

target_include_directories(platform_region_defs
    INTERFACE
        partition
)

#========================= Platform common defs ===============================#

# Specify the location of platform specific build dependencies.
target_sources(tfm_s
    PRIVATE
    $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/gcc/startup_diphda_s.S>
)
target_add_scatter_file(tfm_s
    $<$<C_COMPILER_ID:GNU>:${CMAKE_SOURCE_DIR}/platform/ext/common/gcc/tfm_common_s.ld>
)

target_sources(bl2
    PRIVATE
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/gcc/startup_diphda_bl.S>
)
target_add_scatter_file(bl2
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/gcc/diphda_bl2.ld>
)

#========================= Platform Secure ====================================#

add_subdirectory(openamp)

target_include_directories(platform_s
    PUBLIC
        .
        CMSIS_Driver
        CMSIS_Driver/Config
        Device/Config
        Device/Include
        Native_Driver
        partition
        services/include
)

target_sources(platform_s
    PRIVATE
        CMSIS_Driver/Driver_Flash.c
        CMSIS_Driver/Driver_USART.c
        Device/Source/device_definition.c
        Device/Source/system_core_init.c
        Native_Driver/uart_pl011_drv.c
        Native_Driver/mhu_v2_x.c
        spm_hal.c
        tfm_hal_isolation.c
        $<$<BOOL:TFM_PARTITION_PLATFORM>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
)

target_link_libraries(platform_s
    PUBLIC
        tfm_spm
)

#========================= Platform BL2 =======================================#

target_sources(platform_bl2
    PRIVATE
        CMSIS_Driver/Driver_Flash.c
        CMSIS_Driver/Driver_USART.c
        Device/Source/device_definition.c
        Device/Source/system_core_init.c
        Native_Driver/uart_pl011_drv.c
        bl2_boot_hal.c
        bl2_security_cnt.c
        tfm_rotpk.c
)

target_sources(bl2
    PRIVATE
        bl2_flash_map.c
)

target_compile_definitions(bootutil
    PRIVATE
        MULTIPLE_EXECUTABLE_RAM_REGIONS
)

target_include_directories(platform_bl2
    PUBLIC
        partition
        Device/Include
    PRIVATE
        .
        CMSIS_Driver/Config
        Device/Config
        Native_Driver
        ${MCUBOOT_PATH}/boot/bootutil/include # for fault_injection_hardening.h only
        ${CMAKE_BINARY_DIR}/bl2/ext/mcuboot # for mcuboot_config.h only
)

#========================= BL1 component =======================================#

add_subdirectory(bl1)
