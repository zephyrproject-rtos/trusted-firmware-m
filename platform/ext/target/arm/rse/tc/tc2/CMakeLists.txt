#-------------------------------------------------------------------------------
# Copyright (c) 2022-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)
set(TC_COMMON_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../common)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../common common)

#========================= Platform region defs ===============================#

target_include_directories(platform_region_defs
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

#========================= Platform_s =========================================#

target_sources(platform_s
    PRIVATE
        ${TC_COMMON_SOURCE_DIR}/cmsis_drivers/Driver_Flash.c
        ${TC_COMMON_SOURCE_DIR}/device/host_device_definition.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/cfi/cfi_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata/spi_strataflashj3_flash_lib.c
        $<$<NOT:$<BOOL:${RSE_DEBUG_UART}>>:${TC_COMMON_SOURCE_DIR}/cmsis_drivers/Driver_USART.c>
        $<$<NOT:$<BOOL:${RSE_DEBUG_UART}>>:${PLATFORM_DIR}/ext/target/arm/drivers/usart/pl011/uart_pl011_drv.c>
        ${TC_COMMON_SOURCE_DIR}/rse_comms/rse_comms_permissions_hal.c
)

target_include_directories(platform_s
    PUBLIC
        ${TC_COMMON_SOURCE_DIR}/cmsis_drivers
        ${TC_COMMON_SOURCE_DIR}/device
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/cfi
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata
        $<$<NOT:$<BOOL:${RSE_DEBUG_UART}>>:${PLATFORM_DIR}/ext/target/arm/drivers/usart/pl011>
)

#========================= Platform BL2 =======================================#

target_sources(platform_bl2
    PRIVATE
        ${TC_COMMON_SOURCE_DIR}/bl2/boot_hal_bl2.c
        ${TC_COMMON_SOURCE_DIR}/bl2/flash_map_bl2.c
        ${TC_COMMON_SOURCE_DIR}/cmsis_drivers/Driver_Flash.c
        ${TC_COMMON_SOURCE_DIR}/cmsis_drivers/Driver_Flash.c
        ${TC_COMMON_SOURCE_DIR}/device/host_device_definition.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/cfi/cfi_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata/spi_strataflashj3_flash_lib.c
        $<$<NOT:$<BOOL:${RSE_DEBUG_UART}>>:${TC_COMMON_SOURCE_DIR}/cmsis_drivers/Driver_USART.c>
        $<$<NOT:$<BOOL:${RSE_DEBUG_UART}>>:${PLATFORM_DIR}/ext/target/arm/drivers/usart/pl011/uart_pl011_drv.c>
)

target_include_directories(platform_bl2
    PUBLIC
        ${TC_COMMON_SOURCE_DIR}/cmsis_drivers
        ${TC_COMMON_SOURCE_DIR}/device
        ${TC_COMMON_SOURCE_DIR}/bl2
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/cfi
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata
        $<$<NOT:$<BOOL:${RSE_DEBUG_UART}>>:${PLATFORM_DIR}/ext/target/arm/drivers/usart/pl011>
)

#========================= Platform BL1 =======================================#

target_sources(platform_bl1_1
    PRIVATE
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/cfi/cfi_drv.c
        ${TC_COMMON_SOURCE_DIR}/cmsis_drivers/Driver_Flash.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata/spi_strataflashj3_flash_lib.c
        $<$<NOT:$<BOOL:${RSE_DEBUG_UART}>>:${TC_COMMON_SOURCE_DIR}/cmsis_drivers/Driver_USART.c>
        $<$<NOT:$<BOOL:${RSE_DEBUG_UART}>>:${PLATFORM_DIR}/ext/target/arm/drivers/usart/pl011/uart_pl011_drv.c>
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata/spi_strataflashj3_flash_lib.c
        ${TC_COMMON_SOURCE_DIR}/device/host_device_definition.c
        $<$<BOOL:${RSE_ENABLE_BRINGUP_HELPERS}>:${CMAKE_CURRENT_SOURCE_DIR}/bl1/rse_bringup_helpers_hal.c>
)

target_include_directories(platform_bl1_1_interface
    INTERFACE
        ${TC_COMMON_SOURCE_DIR}/cmsis_drivers
        ${TC_COMMON_SOURCE_DIR}/device
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/cfi
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata
        $<$<NOT:$<BOOL:${RSE_DEBUG_UART}>>:${PLATFORM_DIR}/ext/target/arm/drivers/usart/pl011>
)

target_include_directories(platform_bl1_2
    PUBLIC
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/cfi
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata
        $<$<NOT:$<BOOL:${RSE_DEBUG_UART}>>:${PLATFORM_DIR}/ext/target/arm/drivers/usart/pl011>
)

#========================= Files for building NS side platform ================#

install(FILES       $<$<NOT:$<BOOL:${RSE_DEBUG_UART}>>:${PLATFORM_DIR}/ext/target/arm/drivers/usart/pl011/uart_pl011_drv.c>
                    $<$<NOT:$<BOOL:${RSE_DEBUG_UART}>>:${PLATFORM_DIR}/ext/target/arm/drivers/usart/pl011/uart_pl011_drv.h>
                    ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata/spi_strataflashj3_flash_lib.c
                    ${PLATFORM_DIR}/ext/target/arm/drivers/flash/strata/spi_strataflashj3_flash_lib.h
                    ${PLATFORM_DIR}/ext/target/arm/drivers/flash/cfi/cfi_drv.c
                    ${PLATFORM_DIR}/ext/target/arm/drivers/flash/cfi/cfi_drv.h
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/native_drivers
)

install(DIRECTORY   ${TC_COMMON_SOURCE_DIR}/cmsis_drivers
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}
)

install(DIRECTORY   ${TC_COMMON_SOURCE_DIR}/device
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}
)

install(FILES       rse_memory_sizes.h
                    flash_layout.h
                    host_base_address.h
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/partition)

install(DIRECTORY   ${TC_COMMON_SOURCE_DIR}/ns/
        DESTINATION ${INSTALL_PLATFORM_NS_DIR})

install(FILES       tests/tfm_tests_ns_config.cmake
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/tests
        RENAME      tfm_tests_config.cmake)
