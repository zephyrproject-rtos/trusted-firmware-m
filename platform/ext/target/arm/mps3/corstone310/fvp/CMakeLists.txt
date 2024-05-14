#-------------------------------------------------------------------------------
# Copyright (c) 2022-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)

include(${CORSTONE310_COMMON_DIR}/common.cmake)

add_library(platform_fvp)
add_library(flash_drivers INTERFACE)
add_library(flash_drivers_s STATIC)
add_library(flash_drivers_bl2 STATIC)

target_sources(platform_fvp
    PRIVATE
    ${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350/dma350_ch_drv.c
    ${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350/dma350_drv.c
    ${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350/dma350_lib.c
    $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350/dma350_checker_layer.c>
    $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/dma350_checker_device_defs.c>
    $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/dma350_address_remap.c>
)

target_compile_definitions(platform_fvp
    PRIVATE
        # Needed for DMA-350 library
        CMSIS_device_header=<corstone310.h>
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:PLATFORM_SVC_HANDLERS>
)

target_include_directories(platform_fvp
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_SOURCE_DIR}
        device/config
        ${PLATFORM_DIR}/ext/driver
        ${CORSTONE310_COMMON_DIR}/cmsis_drivers/config/secure
        ${CORSTONE310_COMMON_DIR}/device/include
        ${CORSTONE310_COMMON_DIR}/partition
        ${CORSTONE310_COMMON_DIR}/native_drivers
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpc_sie
        ${PLATFORM_DIR}/ext/target/arm/drivers/counter/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/timer/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350
)

target_compile_options(platform_fvp
    PUBLIC
        ${COMPILER_CP_FLAG}
        ${COMPILER_CMSE_FLAG}
)

target_link_options(platform_fvp
    PUBLIC
        ${LINKER_CP_OPTION}
)

target_link_libraries(platform_fvp
    PRIVATE
        cmsis
        device_definition
)

target_link_libraries(platform_s
    PUBLIC
        platform_fvp
)

target_include_directories(device_definition
    INTERFACE
        ${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350
)

target_include_directories(flash_drivers
    INTERFACE
        ${PLATFORM_DIR}/ext/driver
        ${CORSTONE310_COMMON_DIR}/cmsis_drivers/config/secure
        ${CORSTONE310_COMMON_DIR}/partition
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/emulated
)

target_sources(flash_drivers
    INTERFACE
        cmsis_drivers/Driver_Flash.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/emulated/emulated_flash_drv.c
)

include(${CORSTONE310_COMMON_DIR}/libflash_drivers.cmake)

target_sources(platform_s
    PRIVATE
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${CMAKE_CURRENT_SOURCE_DIR}/platform_svc_handler.c>
)

target_compile_definitions(platform_s
    PRIVATE
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:PLATFORM_SVC_HANDLERS>
)

target_sources(tfm_spm
    PRIVATE
        $<$<OR:$<BOOL:${CONFIG_TFM_FLIH_API}>,$<BOOL:${CONFIG_TFM_SLIH_API}>>:${CMAKE_CURRENT_SOURCE_DIR}/tfm_interrupts.c>
)

#========================= Files for building NS side platform ================#

install(DIRECTORY   ${TARGET_PLATFORM_PATH}/device
        DESTINATION ${INSTALL_PLATFORM_NS_DIR})

install(FILES       ${TARGET_PLATFORM_PATH}/ns/cpuarch_ns.cmake
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}
        RENAME      cpuarch.cmake)

install(FILES       ${TARGET_PLATFORM_PATH}/ns/CMakeLists.txt
        DESTINATION ${INSTALL_PLATFORM_NS_DIR})

install(FILES       ${TARGET_PLATFORM_PATH}/config.cmake
        DESTINATION ${INSTALL_PLATFORM_NS_DIR})
