#-------------------------------------------------------------------------------
# Copyright (c) 2024, Arm Limited. All rights reserved.
# Copyright (c) 2024 ArmChina. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)

set(CONFIG_TFM_FP_ARCH "fpv5-d16")
set(CONFIG_TFM_FP_ARCH_ASM "FPv5_D16")

#========================= Platform region defs ===============================#

target_include_directories(platform_region_defs
    INTERFACE
        ${ALCOR_COMMON_DIR}/partition
        ${PLATFORM_DIR}/partition
        ${PLATFORM_DIR}
)

target_sources(tfm_ns
    PRIVATE
        ${ALCOR_COMMON_DIR}/device/source/startup_alcor_mps3.c
        ${ALCOR_COMMON_DIR}/device/source/system_core_init.c
)

#========================= Device definition lib ===============================#

add_library(device_definition INTERFACE)
target_include_directories(device_definition
    INTERFACE
        ${ALCOR_COMMON_DIR}
        ${ALCOR_COMMON_DIR}/device/config
        ${ALCOR_COMMON_DIR}/device/include
        ${ALCOR_COMMON_DIR}/native_drivers
        ${ALCOR_COMMON_DIR}/partition
        ${CMAKE_CURRENT_SOURCE_DIR}/partition
        ${PLATFORM_DIR}/ext/target/arm/drivers/flash/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpc_sie
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpu/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/counter/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/timer/armv8m
)

add_library(device_definition_ns STATIC)
target_sources(device_definition_ns
    PUBLIC
        ${ALCOR_COMMON_DIR}/device/source/platform_ns_device_definition.c
)

#========================= CMSIS lib ===============================#

add_library(cmsis_includes INTERFACE)
target_include_directories(cmsis_includes
    INTERFACE
        ${ALCOR_COMMON_DIR}/device/config
        ${ALCOR_COMMON_DIR}/device/include
        ${ALCOR_COMMON_DIR}/cmsis_drivers
        ${PLATFORM_DIR}/ext/cmsis/Include
        ${PLATFORM_DIR}/ext/cmsis/Include/m-profile
        ${ALCOR_COMMON_DIR}/partition
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/partition
        ${CMAKE_CURRENT_SOURCE_DIR}/ext/driver
        ${PLATFORM_DIR}/ext/common
        ${ALCOR_COMMON_DIR}
)

add_library(cmsis_includes_ns INTERFACE)
target_link_libraries(cmsis_includes_ns INTERFACE cmsis_includes)
target_include_directories(cmsis_includes_ns
    INTERFACE
        ${ALCOR_COMMON_DIR}/cmsis_drivers/config/non_secure
)

#========================= Platform Non-Secure ================================#

target_sources(platform_ns
    PRIVATE
        ${ALCOR_COMMON_DIR}/cmsis_drivers/Driver_USART.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ext/target/arm/drivers/usart/cmsdk/uart_cmsdk_drv.c
    INTERFACE
        $<$<BOOL:${TEST_NS_FPU}>:${ALCOR_COMMON_DIR}/device/source/alcor_ns_init.c>
        $<$<BOOL:${TEST_NS_FPU}>:${PLATFORM_DIR}/ext/common/test_interrupt.c>
)

target_compile_definitions(platform_ns
    PUBLIC
        $<$<BOOL:${TEST_NS_FPU}>:TEST_NS_FPU>
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
