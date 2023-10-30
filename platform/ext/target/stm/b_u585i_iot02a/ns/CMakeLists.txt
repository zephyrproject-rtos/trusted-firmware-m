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
        Device/Source/startup_stm32u5xx_ns.c
        hal/Src/stm32u5xx_hal.c
        Device/Source/system_stm32u5xx.c
        CMSIS_Driver/low_level_com.c
        hal/Src/stm32u5xx_hal_dma.c
        hal/Src/stm32u5xx_hal_dma_ex.c
        hal/Src/stm32u5xx_hal_pwr.c
        hal/Src/stm32u5xx_hal_pwr_ex.c
        hal/Src/stm32u5xx_hal_rcc.c
        hal/Src/stm32u5xx_hal_gpio.c
        hal/Src/stm32u5xx_hal_uart.c
        hal/Src/stm32u5xx_hal_uart_ex.c
        hal/Src/stm32u5xx_hal_cortex.c
        hal/Src/stm32u5xx_hal_rcc_ex.c
)

target_include_directories(platform_ns
    PUBLIC
        include
        ext/cmsis
        Device/Include
        hal/Inc
)

# Include region_defs.h and flash_layout.h
target_include_directories(platform_region_defs
    INTERFACE
        partition
)

if(${CMAKE_C_COMPILER_ID} STREQUAL GNU)
    set(scatter_file ${CMAKE_CURRENT_LIST_DIR}/linker_scripts/appli_ns.ld)
elseif(${CMAKE_C_COMPILER_ID} STREQUAL ARMClang)
    set(scatter_file ${CMAKE_CURRENT_LIST_DIR}/linker_scripts/appli_ns.sct)
elseif(${CMAKE_C_COMPILER_ID} STREQUAL IAR)
    set(scatter_file ${CMAKE_CURRENT_LIST_DIR}/linker_scripts/appli_ns.icf)
endif()
set_target_properties(platform_ns
                      PROPERTIES INTERFACE_LINK_DEPENDS
                      ${scatter_file}
)
