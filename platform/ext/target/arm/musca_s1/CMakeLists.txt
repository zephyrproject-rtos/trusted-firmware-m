#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
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
    $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/iar/startup_cmsdk_musca_s.s>
)
target_add_scatter_file(tfm_s
    $<$<C_COMPILER_ID:ARMClang>:${CMAKE_SOURCE_DIR}/platform/ext/common/armclang/tfm_common_s.sct>
    $<$<C_COMPILER_ID:GNU>:${CMAKE_SOURCE_DIR}/platform/ext/common/gcc/tfm_common_s.ld>
    $<$<C_COMPILER_ID:IAR>:${CMAKE_SOURCE_DIR}/platform/ext/common/iar/tfm_common_s.icf>
)

if(NS)
    target_sources(tfm_ns
        PRIVATE
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armclang/startup_cmsdk_musca_ns.s>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/gcc/startup_cmsdk_musca_ns.S>
            $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/iar/startup_cmsdk_musca_ns.s>
    )
    target_add_scatter_file(tfm_ns
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armclang/musca_ns.sct>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/gcc/musca_ns.ld>
        $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/iar/musca_ns.icf>
    )
    target_link_libraries(CMSIS_5_tfm_ns
        INTERFACE
            CMSIS_5_RTX_V8MMN
    )
endif()

if(BL2)
    target_sources(bl2
        PRIVATE
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armclang/startup_cmsdk_musca_bl2.s>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/gcc/startup_cmsdk_musca_bl2.S>
            $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/iar/startup_cmsdk_musca_bl2.s>
    )
    target_add_scatter_file(bl2
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armclang/musca_bl2.sct>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/gcc/musca_bl2.ld>
            $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/iar/musca_bl2.icf>
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
        Libraries
        cc312
)

target_sources(platform_s
    PRIVATE
        CMSIS_Driver/Driver_QSPI_Flash.c
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
        Native_Driver/qspi_ip6514e_drv.c
        Native_Driver/cache_drv.c
        spm_hal.c
        tfm_interrupts.c
        tfm_hal_isolation.c
        tfm_hal_platform.c
        target_cfg.c
        Native_Driver/timer_cmsdk_drv.c
        Libraries/mt25ql_flash_lib.c
        ${CMAKE_SOURCE_DIR}/platform/ext/common/tfm_hal_nvic.c
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
        Native_Driver/uart_pl011_drv.c
        Native_Driver/timer_cmsdk_drv.c
        CMSIS_Driver/Driver_USART.c
        Device/Source/system_core_init.c
        Device/Source/device_definition.c
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
)

target_include_directories(platform_ns
    PUBLIC
        .
        CMSIS_Driver/Config
        Device/Config
        Device/Include
        Native_Driver
        services/include
        Libraries
)

#========================= Platform BL2 =======================================#

if(BL2)
    target_sources(platform_bl2
        PRIVATE
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
            Libraries
            cc312
        PRIVATE
            .
            CMSIS_Driver/Config
            Device/Config
            Native_Driver
            ${MCUBOOT_PATH}/boot/bootutil/include # for fault_injection_hardening.h only
            ${CMAKE_BINARY_DIR}/bl2/ext/mcuboot # for mcuboot_config.h only
    )

    target_compile_options(platform_bl2
        PRIVATE
            ${BL2_COMPILER_CP_FLAG}
    )
endif()
