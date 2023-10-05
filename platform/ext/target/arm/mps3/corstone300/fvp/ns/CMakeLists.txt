#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
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
        common/cmsis_drivers/Driver_USART.c
        common/device/source/startup_corstone300.c
        common/device/source/system_core_init.c
        common/device/source/platform_ns_device_definition.c
        ext/target/arm/drivers/uart_cmsdk_drv.c
        ext/common/uart_stdout.c
        ext/target/arm/drivers/systimer_armv8-m_drv.c
)

target_include_directories(platform_ns
    PRIVATE
        retarget
    PUBLIC
        common/include
        common/device/include
        device
        device/config
        device/include
        ext/cmsis
        ext/common
        ext/driver
        ext/target/arm/drivers
        include
        partition
)

# Include region_defs.h and flash_layout.h
target_include_directories(platform_region_defs
    INTERFACE
        partition
)
