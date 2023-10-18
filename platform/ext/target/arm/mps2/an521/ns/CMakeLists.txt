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
        arm_uart_drv.c
        timer_cmsdk.c
        uart_stdout.c
        Driver_USART.c
        cmsis_core/startup_an521.c
        cmsis_core/system_core_init.c
        retarget/platform_retarget_dev.c
)

target_include_directories(platform_ns
    PRIVATE
        retarget
    PUBLIC
        include
        ext/cmsis
        cmsis_core
)

target_compile_definitions(platform_ns
    PUBLIC
        $<$<BOOL:${PLATFORM_DEFAULT_CRYPTO_KEYS}>:PLATFORM_DEFAULT_CRYPTO_KEYS>
)

target_compile_definitions(platform_ns
    PUBLIC
        BL2=${BL2}
        BL2_HEADER_SIZE=${BL2_HEADER_SIZE}
        BL2_TRAILER_SIZE=${BL2_TRAILER_SIZE}
)

if(${CMAKE_C_COMPILER_ID} STREQUAL GNU)
    set(scatter_file ${CMAKE_CURRENT_LIST_DIR}/linker_scripts/tfm_common_ns.ld)
elseif(${CMAKE_C_COMPILER_ID} STREQUAL ARMClang)
    set(scatter_file ${CMAKE_CURRENT_LIST_DIR}/linker_scripts/tfm_common_ns.sct)
endif()

set_target_properties(platform_ns
                      PROPERTIES INTERFACE_LINK_DEPENDS
                      ${scatter_file}
)
