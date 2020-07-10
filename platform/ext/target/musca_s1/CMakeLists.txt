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
    $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armclang/startup_cmsdk_musca_s.s>
    $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/gcc/startup_cmsdk_musca_s.S>
)
target_add_scatter_file(tfm_s
    $<$<C_COMPILER_ID:ARMClang>:${CMAKE_BINARY_DIR}/generated/platform/ext/common/armclang/tfm_common_s.sct>
    $<$<C_COMPILER_ID:GNU>:${CMAKE_BINARY_DIR}/generated/platform/ext/common/gcc/tfm_common_s.ld>
)

if(NS)
    target_sources(tfm_ns
        PRIVATE
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armclang/startup_cmsdk_musca_ns.s>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/gcc/startup_cmsdk_musca_ns.S>
    )
    target_add_scatter_file(tfm_ns
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armclang/musca_ns.sct>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/gcc/musca_ns.ld>
    )
    target_link_libraries(tfm_ns
        PRIVATE
            $<$<C_COMPILER_ID:ARMClang>:${CMSIS_5_PATH}/RTOS2/RTX/Library/ARM/RTX_V8MMN.lib>
            $<$<C_COMPILER_ID:GNU>:${CMSIS_5_PATH}/RTOS2/RTX/Library/GCC/libRTX_V8MMN.a>
            # These libraries require linking to the veneers, so the veneers are
            # marked again here as a link dependency to get the ordering right
            tfm_s_veneers
    )
endif()

if(BL2)
    target_sources(bl2
        PRIVATE
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armclang/startup_cmsdk_musca_bl2.s>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/gcc/startup_cmsdk_musca_bl2.S>
    )
    target_add_scatter_file(bl2
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armclang/musca_bl2.sct>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/gcc/musca_bl2.ld>
    )
endif()

#========================= Platform Secure ====================================#

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
        CMSIS_Driver/Driver_Flash_MRAM.c
        CMSIS_Driver/Driver_MPC.c
        CMSIS_Driver/Driver_PPC.c
        CMSIS_Driver/Driver_USART.c
        Device/Source/device_definition.c
        Device/Source/system_core_init.c
        Native_Driver/mpc_sie200_drv.c
        Native_Driver/mpu_armv8m_drv.c
        Native_Driver/ppc_sse200_drv.c
        Native_Driver/gpio_cmsdk_drv.c
        Native_Driver/uart_pl011_drv.c
        Native_Driver/musca_s1_scc_drv.c
        Native_Driver/cache_drv.c
        spm_hal.c
        target_cfg.c
        $<$<BOOL:TFM_PARTITION_PLATFORM>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
        Native_Driver/timer_cmsdk_drv.c
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
        Native_Driver/uart_pl011_drv.c
        CMSIS_Driver/Driver_USART.c
        Device/Source/system_core_init.c
        Device/Source/device_definition.c
)

target_include_directories(platform_ns
    PUBLIC
        .
        CMSIS_Driver/Config
        Device/Config
        Device/Include
        Native_Driver
)

#========================= Platform BL2 =======================================#

if(BL2)
    target_sources(platform_bl2
        PRIVATE
            $<$<STREQUAL:${CRYPTO_HW_ACCELERATOR_OTP_STATE},ENABLED>:crypto_keys.c>
            boot_hal.c
            Device/Source/system_core_init.c
            Device/Source/device_definition.c
            CMSIS_Driver/Driver_Flash_MRAM.c
            Native_Driver/uart_pl011_drv.c
            Native_Driver/cache_drv.c
            Native_Driver/musca_s1_scc_drv.c
            CMSIS_Driver/Driver_USART.c
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
    )
endif()
