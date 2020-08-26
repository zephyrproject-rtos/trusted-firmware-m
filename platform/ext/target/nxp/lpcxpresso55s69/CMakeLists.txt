#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
# Copyright (c) 2020, Linaro. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)
set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER)
if (BL2 AND ${BUILD_TYPE_LOWER} STREQUAL "debug")
    Message(FATAL_ERROR "If BL2 is enabled, \"${CMAKE_BUILD_TYPE}\" build type cannot be selected as it is currently not \
                         supported on ${TFM_PLATFORM}. For more information please refer to the platform's README file.")
endif()

#========================= Platform region defs ===============================#

target_include_directories(platform_region_defs
    INTERFACE
        partition
)

#========================= Platform common defs ===============================#

# Specify the location of platform specific build dependencies.
target_sources(tfm_s
    PRIVATE
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armgcc/startup_LPC55S69_cm33_core0_s.S>
        $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/iar/startup_LPC55S69_cm33_core0_s.s>
)
target_add_scatter_file(tfm_s
    $<$<C_COMPILER_ID:GNU>:${CMAKE_BINARY_DIR}/generated/platform/ext/common/gcc/tfm_common_s.ld>
    $<$<C_COMPILER_ID:IAR>:${CMAKE_BINARY_DIR}/generated/platform/ext/common/iar/tfm_common_s.icf>
)

if(NS)
    target_sources(tfm_ns
        PRIVATE
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armgcc/startup_LPC55S69_cm33_core0_ns.S>
            $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/iar/startup_LPC55S69_cm33_core0_ns.s>
    )
    target_add_scatter_file(tfm_ns
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armgcc/LPC55S69_cm33_core0_ns.ld>
            $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/iar/LPC55S69_cm33_core0_ns.icf>
    )
    target_link_libraries(CMSIS_5_tfm_ns
        INTERFACE
            CMSIS_5_RTX_V8MMN
    )
endif()

if(BL2)
    target_sources(bl2
        PRIVATE
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armgcc/startup_LPC55S69_cm33_core0_bl2.S>
    )
    target_add_scatter_file(bl2
            $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/Device/Source/armgcc/LPC55S69_cm33_core0_bl2.ld>
    )
endif()

#========================= Platform Secure ====================================#

target_include_directories(platform_s
    PUBLIC
        .
        Native_Driver
        Native_Driver/project_template/s
        Device/Config
        Device/Include
        partition
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/lists
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/serial_manager
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/uart
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/debug_console
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/str
)

target_sources(platform_s
    PRIVATE
        target_cfg.c
        spm_hal.c
        tfm_hal_isolation.c
        Native_Driver/project_template/s/board.c
        Native_Driver/project_template/s/clock_config.c
        Native_Driver/project_template/s/peripherals.c
        Native_Driver/project_template/s/pin_mux.c
        Native_Driver/system_LPC55S69_cm33_core0.c
        ../common/CMSIS_Driver/Driver_Flash.c
        ../common/CMSIS_Driver/Driver_USART.c
        ../common/Native_Driver/components/lists/generic_list.c
        ../common/Native_Driver/components/serial_manager/serial_manager.c
        ../common/Native_Driver/components/serial_manager/serial_port_uart.c
        ../common/Native_Driver/components/uart/usart_adapter.c
        ../common/Native_Driver/drivers/fsl_casper.c
        ../common/Native_Driver/drivers/fsl_clock.c
        ../common/Native_Driver/drivers/fsl_common.c
        ../common/Native_Driver/drivers/fsl_ctimer.c
        ../common/Native_Driver/drivers/fsl_flexcomm.c
        ../common/Native_Driver/drivers/fsl_gpio.c
        ../common/Native_Driver/drivers/fsl_hashcrypt.c
        ../common/Native_Driver/drivers/fsl_iap.c
        ../common/Native_Driver/drivers/fsl_power.c
        ../common/Native_Driver/drivers/fsl_reset.c
        ../common/Native_Driver/drivers/fsl_rng.c
        ../common/Native_Driver/drivers/fsl_usart.c
        ../common/Native_Driver/utilities/fsl_assert.c
        ../common/Native_Driver/utilities/fsl_sbrk.c
        ../common/Native_Driver/utilities/debug_console/fsl_debug_console.c
        ../common/Native_Driver/utilities/str/fsl_str.c
        ../common/Native_Driver/mpu_armv8m_drv.c
        ${CMAKE_SOURCE_DIR}/platform/ext/common/tfm_hal_isolation_mpu_v8m.c
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
)

target_compile_options(platform_s
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

# GNU ld has some strange behaviour to do with weak functions, and does not deal
# well with strong functions in static libraries overriding weak functions in
# object files. For this reason, the file hardware_init is linked directly to
# the s binary as an object file. This issue does not affect armclang, but the
# fix is compatible.
target_sources(tfm_s
    PRIVATE
        Native_Driver/project_template/s/hardware_init.c
)

#========================= Platform Non-Secure ================================#

target_include_directories(platform_ns
    PUBLIC
        .
        Device/Config
        Device/Include
        Native_Driver
        Native_Driver/project_template/ns
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/lists
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/serial_manager
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/uart
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/debug_console
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/str
)

target_sources(platform_ns
    PRIVATE
        Native_Driver/system_LPC55S69_cm33_core0.c
        Native_Driver/project_template/ns/board.c
        Native_Driver/project_template/ns/hardware_init.c
        Native_Driver/project_template/ns/clock_config.c
        Native_Driver/project_template/ns/peripherals.c
        Native_Driver/project_template/ns/pin_mux.c
        ${PLATFORM_DIR}/ext/target/nxp/common/CMSIS_Driver/Driver_USART.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/lists/generic_list.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/serial_manager/serial_manager.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/serial_manager/serial_port_uart.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/uart/usart_adapter.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_casper.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_clock.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_common.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_ctimer.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_flexcomm.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_gpio.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_hashcrypt.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_iap.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_power.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_reset.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_rng.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers/fsl_usart.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/fsl_assert.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/fsl_sbrk.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/debug_console/fsl_debug_console.c
        ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/str/fsl_str.c
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
)

#========================= Platform BL2 =======================================#
if(BL2)

    target_include_directories(platform_bl2
        PUBLIC
            partition
            Device/Include
            Native_Driver
            Native_Driver/project_template/bl2
            ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/drivers
        PRIVATE
            .
            Device/Config
            ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver
            ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/serial_manager
            ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/components/uart
            ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/debug_console
            ${PLATFORM_DIR}/ext/target/nxp/common/Native_Driver/utilities/str
    )
    target_sources(platform_bl2
        PRIVATE
            boot_hal.c
            Native_Driver/project_template/bl2/board.c
            Native_Driver/project_template/bl2/clock_config.c
            Native_Driver/project_template/bl2/pin_mux.c
            Native_Driver/system_LPC55S69_cm33_core0.c
            ../common/CMSIS_Driver/Driver_Flash.c
            ../common/CMSIS_Driver/Driver_USART.c
            ../common/Native_Driver/components/serial_manager/serial_manager.c
            ../common/Native_Driver/components/serial_manager/serial_port_uart.c
            ../common/Native_Driver/components/uart/usart_adapter.c
            ../common/Native_Driver/drivers/fsl_casper.c
            ../common/Native_Driver/drivers/fsl_clock.c
            ../common/Native_Driver/drivers/fsl_common.c
            ../common/Native_Driver/drivers/fsl_flexcomm.c
            ../common/Native_Driver/drivers/fsl_hashcrypt.c
            ../common/Native_Driver/drivers/fsl_iap.c
            ../common/Native_Driver/drivers/fsl_power.c
            ../common/Native_Driver/drivers/fsl_reset.c
            ../common/Native_Driver/drivers/fsl_rng.c
            ../common/Native_Driver/drivers/fsl_usart.c
            ../common/Native_Driver/utilities/fsl_assert.c
            ../common/Native_Driver/utilities/fsl_sbrk.c
            ../common/Native_Driver/utilities/debug_console/fsl_debug_console.c
            ../common/Native_Driver/utilities/str/fsl_str.c
    )

    # GNU ld has some strange behaviour to do with weak functions, and does not deal
    # well with strong functions in static libraries overriding weak functions in
    # object files. For this reason, the file hardware_init is linked directly to
    # the s binary as an object file. This issue does not affect armclang, but the
    # fix is compatible.
    target_sources(bl2
    PRIVATE
        Native_Driver/project_template/bl2/hardware_init.c
    )
endif()
