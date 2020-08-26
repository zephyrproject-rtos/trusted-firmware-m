#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)
set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})


message(DEPRECATION "${TFM_PLATFORM} has been marked for deprecation.")

#========================= Platform region defs ===============================#

target_include_directories(platform_region_defs
    INTERFACE
        partition
)

#========================= Platform common defs ===============================#

# Specify the location of platform specific build dependencies.
target_sources(tfm_s
    PRIVATE
    $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/startup_cmsdk_an539_mps2_s.s>
    $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/startup_cmsdk_an539_mps2_s.S>
    $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/iar/startup_cmsdk_an539_mps2_s.s>
)
target_add_scatter_file(tfm_s
    $<$<C_COMPILER_ID:ARMClang>:${CMAKE_BINARY_DIR}/generated/platform/ext/common/armclang/tfm_common_s.sct>
    $<$<C_COMPILER_ID:GNU>:${CMAKE_BINARY_DIR}/generated/platform/ext/common/gcc/tfm_common_s.ld>
    $<$<C_COMPILER_ID:IAR>:${CMAKE_BINARY_DIR}/generated/platform/ext/common/iar/tfm_common_s.icf>
)

if(NS)
    target_sources(tfm_ns
        PRIVATE
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/startup_cmsdk_an539_mps2_ns.s>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/startup_cmsdk_an539_mps2_ns.S>
            $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/iar/startup_cmsdk_an539_mps2_ns.s>
    )
    target_add_scatter_file(tfm_ns
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/an539_mps2_ns.sct>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/an539_mps2_ns.ld>
        $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/iar/an539_mps2_ns.icf>
    )
    target_link_libraries(CMSIS_5_tfm_ns
        INTERFACE
            CMSIS_5_RTX_V8MBN
    )
endif()

if(BL2)
    target_sources(bl2
        PRIVATE
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/startup_cmsdk_an539_mps2_bl2.s>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/startup_cmsdk_an539_mps2_bl2.S>
            $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/iar/startup_cmsdk_an539_mps2_bl2.s>
    )
    target_add_scatter_file(bl2
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/an539_mps2_bl2.sct>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/an539_mps2_bl2.ld>
            $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/iar/an539_mps2_bl2.icf>
    )
endif()

#========================= Platform Secure ====================================#

target_include_directories(platform_s
    PUBLIC
        .
        ../common
        cmsis_drivers
        cmsis_drivers/config
        native_drivers
        partition
        device/include
        device/config
        ${PLATFORM_DIR}/..
)

target_sources(platform_s
    PRIVATE
        device/source/system_core_init.c
        device/source/device_definition.c
        cmsis_drivers/Driver_Flash.c
        cmsis_drivers/Driver_MPC.c
        cmsis_drivers/Driver_USART.c
        cmsis_drivers/Driver_SSE123_PPC_APB.c
        cmsis_drivers/Driver_SSE123_PPC_Common.c
        cmsis_drivers/Driver_SSE123_PPC_APB_EXP0.c
        cmsis_drivers/Driver_SSE123_PPC_APB_EXP1.c
        cmsis_drivers/Driver_SSE123_PPC_APB_EXP2.c
        cmsis_drivers/Driver_SSE123_PPC_APB_EXP3.c
        cmsis_drivers/Driver_SSE123_PPC_AHB_EXP0.c
        native_drivers/mpc_sie200_drv.c
        native_drivers/mpu_armv8m_drv.c
        native_drivers/ppc_sse123_drv.c
        native_drivers/uart_cmsdk_drv.c
        native_drivers/systimer_armv8-m_drv.c
        native_drivers/syscounter_armv8-m_cntrl_drv.c
        spm_hal.c
        tfm_hal_isolation.c
        target_cfg.c
        ${CMAKE_SOURCE_DIR}/platform/ext/common/tfm_hal_isolation_mpu_v8m.c
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
)

target_compile_options(platform_s
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

#========================= Platform Non-Secure ================================#

target_sources(platform_ns
    PRIVATE
        device/source/system_core_init.c
        device/source/device_definition.c
        native_drivers/uart_cmsdk_drv.c
        cmsis_drivers/Driver_USART.c
        native_drivers/systimer_armv8-m_drv.c
        native_drivers/syscounter_armv8-m_cntrl_drv.c
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
)

target_include_directories(platform_ns
    PUBLIC
        .
        ../common
        ${PLATFORM_DIR}/..
        device/include
        device/config
        native_drivers
        cmsis_drivers/config
)

#========================= Platform BL2 =======================================#

if(BL2)
    target_sources(platform_bl2
        PRIVATE
            boot_hal.c
            device/source/system_core_init.c
            device/source/device_definition.c
            native_drivers/uart_cmsdk_drv.c
            cmsis_drivers/Driver_Flash.c
            cmsis_drivers/Driver_USART.c
    )

    target_include_directories(platform_bl2
        PUBLIC
            partition
            device/include
            device/config
            cmsis_drivers/config
        PRIVATE
            .
            ${PLATFORM_DIR}/..
            native_drivers
    )
endif()
