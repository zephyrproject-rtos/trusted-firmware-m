#-------------------------------------------------------------------------------
# Copyright (c) 2023-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# This file is exported to NS side during CMake installation phase and renamed
# to CMakeLists.txt. It instructs how to build a platform on non-secture side.
# The structure and sources list are fully platform specific.

add_library(platform_ns)

target_sources(platform_ns
    PRIVATE
        Device/Source/startup_musca.c
        Device/Source/system_core_init.c
        Device/Source/device_definition.c
        uart_pl011_drv.c
        Native_Driver/timer_cmsdk_drv.c
        CMSIS_Driver/Driver_USART.c
        $<$<BOOL:${PLATFORM_GPLED_ENABLED}>:services/src/tfm_ioctl_ns_api.c>
        $<$<BOOL:${PLATFORM_GPLED_ENABLED}>:services/src/tfm_gpled_api.c>
)

target_include_directories(platform_ns
    PUBLIC
        include
        ext/cmsis/Include
        ext/cmsis/Include/m-profile
        ext/common
        Native_Driver
        CMSIS_Driver/Config
        Device/Include
        partition
        $<$<BOOL:${PLATFORM_GPLED_ENABLED}>:services/include>
)

target_compile_definitions(platform_ns
    PRIVATE
        $<$<BOOL:${CONFIG_ENABLE_NS_UART_TX_RX_CONTROL}>:UART_TX_RX_CONTROL_ENABLED>
)

target_include_directories(platform_region_defs
    INTERFACE
        partition
)
