#-------------------------------------------------------------------------------
# Copyright (c) 2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)

set(PLATFORM_DIR    ${CMAKE_CURRENT_LIST_DIR})

add_library(platform_ns STATIC)

set(CONFIG_TFM_FP_ARCH "fpv5-d16")
set(CONFIG_TFM_FP_ARCH_ASM "FPv5_D16")

#========================= Platform region defs ===============================#

target_include_directories(platform_region_defs
    INTERFACE
        partition
        ${CMAKE_CURRENT_SOURCE_DIR}
)

#========================= Platform common defs ===============================#

target_sources(tfm_ns
    PRIVATE
        device/source/startup_corstone315.c
        device/source/system_core_init.c
)

#========================= Device definition lib ===============================#

add_library(device_definition INTERFACE)
target_include_directories(device_definition
    INTERFACE
        .
        device/include
        native_drivers
        partition
        libraries
        native_drivers
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
        device/source/platform_ns_device_definition.c
)

#========================= CMSIS lib ===============================#

add_library(cmsis_includes INTERFACE)
target_include_directories(cmsis_includes
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/device/config
        device/include
        cmsis_drivers
        partition
        ${PLATFORM_DIR}/ext/cmsis/Include
        ${PLATFORM_DIR}/ext/cmsis/Include/m-profile
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/ext/driver
        ${CMAKE_CURRENT_SOURCE_DIR}/ext/common
)

add_library(cmsis_includes_ns INTERFACE)
target_link_libraries(cmsis_includes_ns INTERFACE cmsis_includes)
target_include_directories(cmsis_includes_ns
    INTERFACE
        cmsis_drivers/config/non_secure
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
        cmsis_drivers/Driver_USART.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk/uart_cmsdk_drv.c
    INTERFACE
        $<$<BOOL:${TEST_NS_FPU}>:${CORSTONE300_COMMON_DIR}/device/source/corstone315_ns_init.c>
        $<$<BOOL:${TEST_NS_FPU}>:${PLATFORM_DIR}/ext/common/test_interrupt.c>
)

target_compile_definitions(platform_ns
    PUBLIC
        $<$<BOOL:${TEST_NS_FPU}>:TEST_NS_FPU>
)
