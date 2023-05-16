#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)
set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../common common)

# TFM_PLATFORM_VARIANT value for supported RD-Fremont platform variants:
# - '0' for RD-Fremont platform

if (TFM_PLATFORM_VARIANT STREQUAL "0")
      target_compile_definitions(platform_common_interface
                                 INTERFACE -DPLATFORM_VARIANT=0)
else()
      message(FATAL_ERROR "Please set TFM_PLATFORM_VARIANT to \n"
                                "'0' for RD-Fremont\n")
endif()

#========================= Platform Region Defs ===============================#

target_include_directories(platform_region_defs
    INTERFACE
        .
)

#========================= Platform Secure ====================================#

target_include_directories(platform_s
    PUBLIC
        cmsis_drivers
        device
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/cfi
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/pl011
)

target_sources(platform_s
    PRIVATE
        cmsis_drivers/Driver_Flash.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/cfi/cfi_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata/spi_strataflashj3_flash_lib.c
        cmsis_drivers/Driver_USART.c
        device/host_device_definition.c
        device/rse_expansion_device_definition.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/pl011/uart_pl011_drv.c
        rse_expansion_peripherals_def.c
        rse_comms/rse_comms_permissions_hal.c
)

#========================= Platform BL2 =======================================#

target_sources(platform_bl2
    PRIVATE
        bl2/boot_hal_bl2.c
        bl2/flash_map_bl2.c
        bl2/ni_tower_sysctrl_lib.c
        cmsis_drivers/Driver_Flash.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/cfi/cfi_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata/spi_strataflashj3_flash_lib.c
        cmsis_drivers/Driver_USART.c
        device/host_device_definition.c
        device/rse_expansion_device_definition.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/pl011/uart_pl011_drv.c
)

target_include_directories(platform_bl2
    PUBLIC
        bl2
        cmsis_drivers
        device
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/cfi
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/pl011
)

#========================= Platform BL1_1 =====================================#

target_sources(platform_bl1_1
    PRIVATE
        cmsis_drivers/Driver_Flash.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/cfi/cfi_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata/spi_strataflashj3_flash_lib.c
        cmsis_drivers/Driver_USART.c
        device/host_device_definition.c
        device/rse_expansion_device_definition.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/pl011/uart_pl011_drv.c
        $<$<BOOL:${RSE_ENABLE_BRINGUP_HELPERS}>:${CMAKE_CURRENT_SOURCE_DIR}/bl1/rse_bringup_helpers_hal.c>
)

target_include_directories(platform_bl1_1_interface
    INTERFACE
        cmsis_drivers
        device
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/cfi
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/pl011
)

#========================= Platform BL1_2 =====================================#

target_include_directories(platform_bl1_2
    PUBLIC
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/cfi
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/pl011
        cmsis_drivers
        device
)

#==================== Files for building NS side platform =====================#

install(FILES       ${PLATFORM_DIR}/ext/target/arm/drivers/usart/pl011/uart_pl011_drv.c
                    ${PLATFORM_DIR}/ext/target/arm/drivers/usart/pl011/uart_pl011_drv.h
                    ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata/spi_strataflashj3_flash_lib.c
                    ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata/spi_strataflashj3_flash_lib.h
                    ${PLATFORM_DIR}/ext/target/arm/drivers/flash/cfi/cfi_drv.c
                    ${PLATFORM_DIR}/ext/target/arm/drivers/flash/cfi/cfi_drv.h
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/native_drivers
)

install(DIRECTORY   cmsis_drivers
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}
)

install(DIRECTORY   device
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}
)

install(DIRECTORY   ns/
        DESTINATION ${INSTALL_PLATFORM_NS_DIR})

install(FILES       cpuarch.cmake
                    config.cmake
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/config/subplatform/)

install(FILES       tests/tfm_tests_ns_config.cmake
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/tests
        RENAME      tfm_tests_config.cmake)
