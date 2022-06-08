#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
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
        cc312
)

#========================= Platform common defs ===============================#

# Specify the location of platform specific build dependencies.
target_sources(tfm_s
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rss_s.c
)
target_add_scatter_file(tfm_s
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_SOURCE_DIR}/platform/ext/common/armclang/tfm_common_s.sct>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_SOURCE_DIR}/platform/ext/common/gcc/tfm_common_s.ld>
)

if(NS)
    target_sources(tfm_ns
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rss_ns.c
    )
    target_add_scatter_file(tfm_ns
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/rss_ns.sct>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/rss_ns.ld>
    )
    target_link_libraries(CMSIS_5_tfm_ns
        INTERFACE
            CMSIS_5_RTX_V8MMN
    )
endif()

target_sources(bl2
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rss_bl2.c
)
target_add_scatter_file(bl2
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/rss_bl2.sct>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/rss_bl2.ld>
)
target_compile_definitions(bootutil
    PRIVATE
        MULTIPLE_EXECUTABLE_RAM_REGIONS
)

target_sources(bl1_1
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rss_bl2.c
)
target_add_scatter_file(bl1_1
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/rss_bl1_1.sct>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/rss_bl1_1.ld>
)

target_sources(bl1_2
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rss_bl2.c
)
target_add_scatter_file(bl1_2
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/rss_bl1_2.sct>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/rss_bl1_2.ld>
)

#========================= Platform Secure ====================================#

add_subdirectory(rss_comms)

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
        ${PLATFORM_DIR}/..
)

target_sources(platform_s
    PRIVATE
        cmsis_drivers/Driver_Flash.c
        cmsis_drivers/Driver_MPC.c
        cmsis_drivers/Driver_PPC.c
        cmsis_drivers/Driver_USART.c
        device/source/device_definition.c
        device/source/system_core_init.c
        native_drivers/atu_rss_drv.c
        native_drivers/kmu_drv.c
        native_drivers/mhu_v2_x.c
        native_drivers/mhu_wrapper_v2_x.c
        native_drivers/mpc_sie_drv.c
        native_drivers/mpu_armv8m_drv.c
        native_drivers/ppc_rss_drv.c
        native_drivers/syscounter_armv8-m_cntrl_drv.c
        native_drivers/systimer_armv8-m_drv.c
        native_drivers/uart_pl011_drv.c
        tfm_peripherals_def.c
        $<$<OR:$<BOOL:${TEST_NS_SLIH_IRQ}>,$<BOOL:${TEST_NS_FLIH_IRQ}>>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
        native_drivers/lcm_drv.c
        otp_lcm.c
        nv_counters.c
        attest_hal.c
        tfm_hal_platform_reset.c
)

target_sources(tfm_sprt
    PRIVATE
        # SLIH test Partition and FLIH test Partition access the timer as ARoT Partitions.
        # Put the driver to SPRT so that both SLIH and FLIH tests can access it.
        $<$<OR:$<BOOL:${TEST_NS_SLIH_IRQ}>,$<BOOL:${TEST_NS_FLIH_IRQ}>>:${CMAKE_CURRENT_SOURCE_DIR}/native_drivers/systimer_armv8-m_drv.c>
)

target_compile_definitions(platform_s
    INTERFACE
        ATTEST_KEY_BITS=${ATTEST_KEY_BITS}
)

target_compile_options(platform_s
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

#========================= Platform Crypto Keys ===============================#

target_sources(platform_crypto_keys
    PRIVATE
        crypto_keys.c
)

#========================= Platform Non-Secure ================================#

target_sources(platform_ns
    PRIVATE
        cmsis_drivers/Driver_Flash.c
        cmsis_drivers/Driver_USART.c
        device/source/device_definition.c
        device/source/system_core_init.c
        native_drivers/systimer_armv8-m_drv.c
        native_drivers/uart_pl011_drv.c
        $<$<BOOL:${TFM_MULTI_CORE_TOPOLOGY}>:${CMAKE_CURRENT_SOURCE_DIR}/platform_ns_mailbox.c>
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
        partition
)

#========================= Platform BL2 =======================================#

target_sources(platform_bl2
    PRIVATE
        tfm_hal_platform_reset.c
        bl2/boot_hal_bl2.c
        cmsis_drivers/Driver_Flash.c
        cmsis_drivers/Driver_USART.c
        device/source/device_definition.c
        device/source/system_core_init.c
        native_drivers/atu_rss_drv.c
        native_drivers/kmu_drv.c
        native_drivers/mhu_v2_x.c
        native_drivers/uart_pl011_drv.c
        native_drivers/lcm_drv.c
        otp_lcm.c
        nv_counters.c
)

target_include_directories(platform_bl2
    PUBLIC
        .
        cmsis_drivers
        cmsis_drivers/config
        device
        device/config
        device/include
        device/source/armclang
        native_drivers
        partition
        services/src

    PRIVATE
        ${PLATFORM_DIR}/..
        native_drivers
)

target_sources(bl2
    PRIVATE
        bl2/flash_map_bl2.c
)

target_link_libraries(platform_bl2
    PRIVATE
        $<$<AND:$<BOOL:${CONFIG_TFM_BOOT_STORE_MEASUREMENTS}>,$<BOOL:${TFM_PARTITION_MEASURED_BOOT}>>:tfm_boot_status>
)

target_compile_definitions(platform_bl2
    PUBLIC
        $<$<AND:$<BOOL:${CONFIG_TFM_BOOT_STORE_MEASUREMENTS}>,$<BOOL:${TFM_PARTITION_MEASURED_BOOT}>>:MEASURED_BOOT_API>
)

#========================= Platform BL1 =======================================#

target_sources(platform_bl1
    PRIVATE
        ./bl1/boot_hal_bl1.c
        ./cmsis_drivers/Driver_Flash.c
        ./cmsis_drivers/Driver_USART.c
        ./device/source/device_definition.c
        ./device/source/system_core_init.c
        ./native_drivers/atu_rss_drv.c
        ./native_drivers/kmu_drv.c
        ./native_drivers/uart_pl011_drv.c
        ./native_drivers/lcm_drv.c
        ./otp_lcm.c
        ./nv_counters.c
        ./tfm_hal_platform_reset.c
)

target_include_directories(platform_bl1_interface
    INTERFACE
        .
        ./cmsis_drivers
        ./cmsis_drivers/config
        ./device/config
        ./device/include
        ./native_drivers
)

target_compile_definitions(platform_bl1
    PUBLIC
        $<$<AND:$<BOOL:${CONFIG_TFM_BOOT_STORE_MEASUREMENTS}>,$<BOOL:${TFM_PARTITION_MEASURED_BOOT}>>:MEASURED_BOOT_API>
)

#========================= ns_agent_mailbox ===================================#

if(TFM_MULTI_CORE_TOPOLOGY)
    target_sources(tfm_psa_rot_partition_ns_agent_mailbox
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/tfm_hal_multi_core.c
    )
endif()

#========================= tfm_spm ============================================#

target_sources(tfm_spm
    PRIVATE
        spm_hal.c
        target_cfg.c
        tfm_hal_isolation.c
        tfm_hal_platform.c
        tfm_interrupts.c
)
