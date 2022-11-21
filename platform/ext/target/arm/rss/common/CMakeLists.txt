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
        ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rss.c
)
target_add_scatter_file(tfm_s
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_SOURCE_DIR}/platform/ext/common/armclang/tfm_common_s.sct>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_SOURCE_DIR}/platform/ext/common/gcc/tfm_common_s.ld>
)

if(NS)
    target_sources(tfm_ns
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rss.c
    )
    target_add_scatter_file(tfm_ns
            $<$<C_COMPILER_ID:ARMClang>:${CMAKE_SOURCE_DIR}/platform/ext/common/armclang/tfm_common_ns.sct>
            $<$<C_COMPILER_ID:GNU>:${CMAKE_SOURCE_DIR}/platform/ext/common/gcc/tfm_common_ns.ld>
    )
    target_link_libraries(CMSIS_5_tfm_ns
        INTERFACE
            CMSIS_5_RTX_V8MMN
    )
endif()

target_sources(bl2
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rss.c
)
target_add_scatter_file(bl2
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_SOURCE_DIR}/platform/ext/common/armclang/tfm_common_bl2.sct>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_SOURCE_DIR}/platform/ext/common/gcc/tfm_common_bl2.ld>
)
target_compile_definitions(bootutil
    PRIVATE
        MULTIPLE_EXECUTABLE_RAM_REGIONS
)

target_sources(bl1_1
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rss.c
)
target_add_scatter_file(bl1_1
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/rss_bl1_1.sct>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/rss_bl1_1.ld>
)

target_sources(bl1_2
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rss.c
)
target_add_scatter_file(bl1_2
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/rss_bl1_2.sct>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/rss_bl1_2.ld>
)


install(FILES       platform_builtin_key_ids.h
        DESTINATION ${TFM_INSTALL_PATH}/interface/include)

#========================= Platform Secure ====================================#

if (TFM_MULTI_CORE_TOPOLOGY)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/rss_comms rss_comms)
endif()

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
        libraries
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
        libraries/dma350_lib.c
        native_drivers/atu_rss_drv.c
        native_drivers/dma350_drv.c
        native_drivers/dma350_ch_drv.c
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
        provisioning.c
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${CMAKE_CURRENT_SOURCE_DIR}/platform_svc_handler.c>
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${CMAKE_CURRENT_SOURCE_DIR}/libraries/dma350_checker_layer.c>
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/dma350_checker_device_defs.c>
        spm_dma_copy.c
)

target_link_libraries(platform_s
    PRIVATE
        tfm_sprt # For tfm_strnlen in attest HAL
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
        spm_memcpy=spm_dma_memcpy
    PRIVATE
        # Needed for DMA-350 library
        CMSIS_device_header="rss.h"
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:PLATFORM_SVC_HANDLERS>
)

target_compile_options(platform_s
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

#========================= Platform Crypto Keys ===============================#

if (TFM_PARTITION_CRYPTO)
    target_sources(platform_crypto_keys
        PRIVATE
            crypto_keys.c
    )

    target_include_directories(platform_crypto_keys
        PUBLIC
            .
            ./native_drivers
            ./device/include
            ./device/config
    )
endif()

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
        bl2/provisioning.c
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
        target_cfg.c
        tfm_hal_isolation.c
        tfm_hal_platform.c
        tfm_interrupts.c
        faults.c
)

#=========================== CPAK generator tool ===============================

find_package(Python3)

add_custom_target(cpak_generator
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cpak_generator/cpak_generator.c
)

add_dependencies(cpak_generator
    bl1_2_padded_bin
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_generator
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cpak_generator/cpak_generator.c
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator
    COMMAND ${CMAKE_COMMAND} -S ${CMAKE_CURRENT_SOURCE_DIR}/cpak_generator
                             -B ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator
                             -G ${CMAKE_GENERATOR}
                             -DMBEDCRYPTO_PATH=${MBEDCRYPTO_PATH}
                             -DBL1_2_HASH_PATH=\\\"$<TARGET_FILE_DIR:bl1_2>/bl1_2_padded_hash.bin\\\"
                             -DGUK_PATH=\\\"${TFM_GUK_PATH}\\\"
                             -DCPAK_OUTPUT_PATH=\\\"${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_public.bin\\\"
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator
)

add_custom_target(cpak_public ALL
    DEPENDS ${TFM_GUK_PATH}
    DEPENDS cpak_generator
    DEPENDS ${CMAKE_BINARY_DIR}/bin/bl1_2_padded_hash.bin
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_generator
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cpak_generator/bin_to_pem.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_generator
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/cpak_generator/bin_to_pem.py
                    --input_key_file ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_public.bin
                    --output_key_file ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_public.pem
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_public.bin $<TARGET_FILE_DIR:bl1_1>
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_public.pem $<TARGET_FILE_DIR:bl1_1>
)
