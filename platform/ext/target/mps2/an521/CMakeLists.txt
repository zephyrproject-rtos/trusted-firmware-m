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

# Specify the location of platform specific build dependencies.
target_sources(tfm_s
    PRIVATE
    $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/armclang/startup_cmsdk_mps2_an521_s.s>
    $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/gcc/startup_cmsdk_mps2_an521_s.S>
)
target_add_scatter_file(tfm_s
    $<$<C_COMPILER_ID:ARMClang>:${CMAKE_BINARY_DIR}/generated/platform/ext/common/armclang/tfm_common_s.sct>
    $<$<C_COMPILER_ID:GNU>:${CMAKE_BINARY_DIR}/generated/platform/ext/common/gcc/tfm_common_s.ld>
    $<$<C_COMPILER_ID:IAR>:${CMAKE_BINARY_DIR}/generated/platform/ext/common/iar/tfm_common_s.icf>
)

if(NS)
    target_sources(tfm_ns
        PRIVATE
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/armclang/startup_cmsdk_mps2_an521_ns.s>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/gcc/startup_cmsdk_mps2_an521_ns.S>
    )
    target_add_scatter_file(tfm_ns
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/armclang/mps2_an521_ns.sct>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/gcc/mps2_an521_ns.ld>
    )
    target_link_libraries(tfm_ns
        PRIVATE
            $<$<C_COMPILER_ID:ARMClang>:${CMSIS_5_PATH}/RTOS2/RTX/Library/ARM/RTX_V8MBN.lib>
            $<$<C_COMPILER_ID:GNU>:${CMSIS_5_PATH}/RTOS2/RTX/Library/GCC/libRTX_V8MBN.a>
            # These libraries require linking to the veneers, so the veneers are
            # marked again here as a link dependency to get the ordering right
            tfm_s_veneers
    )
endif()

if(BL2)
    target_sources(bl2
        PRIVATE
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/armclang/startup_cmsdk_mps2_an521_bl2.s>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/gcc/startup_cmsdk_mps2_an521_bl2.S>
    )
    target_add_scatter_file(bl2
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/armclang/mps2_an521_bl2.sct>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/gcc/mps2_an521_bl2.ld>
    )
endif()

#========================= Platform Secure ====================================#

target_include_directories(platform_s
    PUBLIC
        .
        ../common
        cmsis_drivers
        native_drivers
        partition
        retarget
        cmsis_core
        native_drivers/timer_cmsdk
        ${PLATFORM_DIR}/..
)

target_sources(platform_s
    PRIVATE
        cmsis_drivers/Driver_Flash.c
        cmsis_drivers/Driver_MPC.c
        cmsis_drivers/Driver_PPC.c
        cmsis_drivers/Driver_USART.c
        retarget/platform_retarget_dev.c
        cmsis_core/system_cmsdk_mps2_an521.c
        native_drivers/mpc_sie200_drv.c
        native_drivers/mpu_armv8m_drv.c
        native_drivers/ppc_sse200_drv.c
        spm_hal.c
        target_cfg.c
        native_drivers/ppc_sse200_drv.c
        native_drivers/arm_uart_drv.c
        native_drivers/timer_cmsdk/timer_cmsdk.c
        $<$<BOOL:TFM_PARTITION_PLATFORM>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
        $<$<BOOL:TFM_PARTITION_PLATFORM>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
)

target_compile_options(platform_s
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

target_compile_definitions(platform_s
    PUBLIC
        $<$<C_COMPILER_ID:GNU>:__STARTUP_CLEAR_BSS_MULTIPLE>
        $<$<C_COMPILER_ID:GNU>:__STARTUP_COPY_MULTIPLE>
)

#========================= Platform Non-Secure ================================#

target_sources(platform_ns
    PRIVATE
        native_drivers/arm_uart_drv.c
        cmsis_drivers/Driver_USART.c
        retarget/platform_retarget_dev.c
        cmsis_core/system_cmsdk_mps2_an521.c
)

target_include_directories(platform_ns
    PUBLIC
        .
        ${PLATFORM_DIR}/..
        retarget
        cmsis_core
        native_drivers
)

#========================= Platform BL2 =======================================#

if(BL2)
    target_sources(platform_bl2
        PRIVATE
            boot_hal.c
            retarget/platform_retarget_dev.c
            cmsis_core/system_cmsdk_mps2_an521.c
            cmsis_drivers/Driver_Flash.c
            native_drivers/arm_uart_drv.c
            cmsis_drivers/Driver_USART.c
    )

    target_include_directories(platform_bl2
        PUBLIC
            partition
            cmsis_core
            retarget
        PRIVATE
            .
            ${PLATFORM_DIR}/..
            native_drivers
    )
endif()
