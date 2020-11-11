#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
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

if (${CMAKE_C_COMPILER_ID} STREQUAL GNU)
    message(FATAL_ERROR "GCC is currently not supported on the mps2/fvp_sse300 because TFM build system does not support the Coretex-M55 with GNUARM")
endif()

if (${CMAKE_C_COMPILER_ID} STREQUAL IAR)
    message(FATAL_ERROR "IAR is currently not supported on the mps2/fvp_sse300 due to a lack of scatter files")
endif()

if (${CMAKE_C_COMPILER_ID} STREQUAL ARMClang)
    if (${CMAKE_C_COMPILER_VERSION} VERSION_LESS "6.14")
        message(FATAL_ERROR "CPU (Cortex-M55) is only supported in ARMCLANG version 6.14 or newer.")
    endif()
endif()


# Specify the location of platform specific build dependencies.
target_sources(tfm_s
    PRIVATE
    $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_fvp_sse300_mps2_s.c>
    # $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_fvp_sse300_mps2_s.c>
    $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_fvp_sse300_mps2_s.c>
)
target_add_scatter_file(tfm_s
    $<$<C_COMPILER_ID:ARMClang>:${CMAKE_BINARY_DIR}/generated/platform/ext/common/armclang/tfm_common_s.sct>
    # $<$<C_COMPILER_ID:GNU>:${CMAKE_BINARY_DIR}/generated/platform/ext/common/gcc/tfm_common_s.ld>
    $<$<C_COMPILER_ID:IAR>:${CMAKE_BINARY_DIR}/generated/platform/ext/common/iar/tfm_common_s.icf>
)

if(NS)
    target_sources(tfm_ns
        PRIVATE
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_fvp_sse300_mps2_ns.c>
            # $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_fvp_sse300_mps2_ns.c>
            $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_fvp_sse300_mps2_ns.c>
    )
    target_add_scatter_file(tfm_ns
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/fvp_sse300_mps2_ns.sct>
        # $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/fvp_sse300_mps2_ns.ld>
        $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/iar/fvp_sse300_mps2_ns.icf>
    )
    target_link_libraries(CMSIS_5_tfm_ns
        INTERFACE
            CMSIS_5_RTX_V8MMN
    )
endif()

if(BL2)
    target_sources(bl2
        PRIVATE
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_fvp_sse300_mps2_bl2.c>
            # $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_fvp_sse300_mps2_bl2.c>
            $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_fvp_sse300_mps2_bl2.c>
    )
    target_add_scatter_file(bl2
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/fvp_sse300_mps2_bl2.sct>
            # $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/fvp_sse300_mps2_bl2.ld>
            $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/iar/fvp_sse300_mps2_bl2.icf>
    )
endif()

#========================= Platform Secure ====================================#

target_include_directories(platform_s
    PUBLIC
        .
        ../common
        cmsis_drivers
        cmsis_drivers/config
        device
        device/config
        device/include
        device/source/armclang
        native_drivers
        partition
        services/src
        native_drivers
        ${PLATFORM_DIR}/..
)

target_sources(platform_s
    PRIVATE
        device/source/system_core_init.c
        device/source/device_definition.c
        native_drivers/uart_cmsdk_drv.c
        native_drivers/systimer_armv8-m_drv.c
        target_cfg.c
        cmsis_drivers/Driver_USART.c
        cmsis_drivers/Driver_Flash.c
        native_drivers/mpc_sie_drv.c
        native_drivers/ppc_sse300_drv.c
        spm_hal.c
        tfm_hal_isolation.c
        native_drivers/mpu_armv8m_drv.c
        cmsis_drivers/Driver_MPC.c
        cmsis_drivers/Driver_SSE300_PPC.c
        ${CMAKE_SOURCE_DIR}/platform/ext/common/tfm_hal_isolation_mpu_v8m.c
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
)

target_link_libraries(platform_s
    PRIVATE
        tfm_utilities
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
        native_drivers/systimer_armv8-m_drv.c
        cmsis_drivers/Driver_USART.c
        cmsis_drivers/Driver_Flash.c
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
)

target_include_directories(platform_ns
    PUBLIC
        .
        ../common
        ${PLATFORM_DIR}/..
        cmsis_drivers
        cmsis_drivers/config
        device
        device/config
        device/include
        device/source/armclang
        native_drivers
        native_drivers/timer_cmsdk
        partition
        services/src
        native_drivers
)

#========================= Platform BL2 =======================================#

if(BL2)
    target_sources(platform_bl2
        PRIVATE
            boot_hal.c
            device/source/device_definition.c
            device/source/system_core_init.c
            cmsis_drivers/Driver_Flash.c
            native_drivers/uart_cmsdk_drv.c
            cmsis_drivers/Driver_USART.c
    )

    target_include_directories(platform_bl2
        PUBLIC
            cmsis_drivers
            cmsis_drivers/config
            device
            device/config
            device/include
            device/source/armclang
            native_drivers
            partition
            services/src
            native_drivers

        PRIVATE
            .
            ${PLATFORM_DIR}/..
            native_drivers
    )
endif()
