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
    INTERFACE
        $<$<BOOL:${TEST_NS_FPU}>:${CMAKE_CURRENT_LIST_DIR}/cmsis_core/an521_ns_init.c>
        $<$<BOOL:${TEST_NS_FPU}>:${CMAKE_CURRENT_LIST_DIR}/test_interrupt.c>
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
        $<$<BOOL:${TEST_NS_FPU}>:TEST_NS_FPU>
        $<$<STREQUAL:${CONFIG_TFM_FLOAT_ABI},hard>:CONFIG_TFM_FLOAT_ABI=2>
        $<$<STREQUAL:${CONFIG_TFM_FLOAT_ABI},soft>:CONFIG_TFM_FLOAT_ABI=0>
        $<$<BOOL:${CONFIG_TFM_ENABLE_CP10CP11}>:CONFIG_TFM_ENABLE_CP10CP11>
)

# Include region_defs.h and flash_layout.h
target_include_directories(platform_region_defs
    INTERFACE
        include
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
