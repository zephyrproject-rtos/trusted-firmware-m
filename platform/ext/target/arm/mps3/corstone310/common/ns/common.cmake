#-------------------------------------------------------------------------------
# Copyright (c) 2020-2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)

#========================= Platform region defs ===============================#

target_include_directories(platform_region_defs
    INTERFACE
        ${CORSTONE310_COMMON_DIR}/partition
        ${CMAKE_CURRENT_SOURCE_DIR}
)

#========================= Platform common defs ===============================#

target_sources(tfm_ns
    PRIVATE
        ${CORSTONE310_COMMON_DIR}/device/source/startup_corstone310.c
)

#========================= Device definition lib ===============================#

add_library(device_definition INTERFACE)
target_include_directories(device_definition
    INTERFACE
        ${CORSTONE310_COMMON_DIR}/device/include
        ${CORSTONE310_COMMON_DIR}/native_drivers
        ${CORSTONE310_COMMON_DIR}/partition
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpc_sie
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpu/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/counter/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/timer/armv8m
        ${ETHOS_DRIVER_PATH}/src
        ${ETHOS_DRIVER_PATH}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/device/config
)

add_library(device_definition_ns STATIC)
target_sources(device_definition_ns
    PUBLIC
        ${CORSTONE310_COMMON_DIR}/device/source/platform_ns_device_definition.c
)

#========================= CMSIS lib ===============================#

add_library(cmsis_includes INTERFACE)
target_include_directories(cmsis_includes
    INTERFACE
        ${CORSTONE310_COMMON_DIR}/device/include
        ${CORSTONE310_COMMON_DIR}/cmsis_drivers
        ${PLATFORM_DIR}/ext/cmsis
        ${CORSTONE310_COMMON_DIR}/partition
)

add_library(cmsis_includes_ns INTERFACE)
target_link_libraries(cmsis_includes_ns INTERFACE cmsis_includes)
target_include_directories(cmsis_includes_ns
    INTERFACE
        ${CORSTONE310_COMMON_DIR}/cmsis_drivers/config/non_secure
)

#========================= Linking ===============================#

target_link_libraries(device_definition_ns PUBLIC device_definition)
target_link_libraries(device_definition_ns PRIVATE cmsis_includes_ns)

target_link_libraries(platform_ns
    PUBLIC
        cmsis_includes_ns
    PRIVATE
        device_definition_ns
)

#========================= Platform Non-Secure ================================#

target_sources(platform_ns
    PRIVATE
        ${CORSTONE310_COMMON_DIR}/cmsis_drivers/Driver_USART.c
        ${CORSTONE310_COMMON_DIR}/device/source/system_core_init.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/emulated/emulated_flash_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk/uart_cmsdk_drv.c
)

target_include_directories(platform_ns
    PUBLIC
        ${CORSTONE310_COMMON_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/device/config
    PRIVATE
        ${CORSTONE310_COMMON_DIR}/device
        ${PLATFORM_DIR}/ext/common
)

#========================= platform_region_defs ===============================#
target_compile_definitions(platform_region_defs
    INTERFACE
        FLASH_S_PARTITION_SIZE=${FLASH_S_PARTITION_SIZE}
        FLASH_NS_PARTITION_SIZE=${FLASH_NS_PARTITION_SIZE}
        PROVISIONING_CODE_PADDED_SIZE=${PROVISIONING_CODE_PADDED_SIZE}
        PROVISIONING_VALUES_PADDED_SIZE=${PROVISIONING_VALUES_PADDED_SIZE}
        PROVISIONING_DATA_PADDED_SIZE=${PROVISIONING_DATA_PADDED_SIZE}
)
