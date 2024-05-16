#-------------------------------------------------------------------------------
# Copyright (c) 2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

add_library(rse_bl1_tests_platform INTERFACE)

target_compile_options(rse_bl1_tests_platform
    INTERFACE
        # Prevent BL1_2 from using r9
        -ffixed-r9
        # Inlining causes gcc to generate reads and writes to r9. This is
        # probably a bug, but work around it by not inlining functions.
        -fno-inline
)

################################################################################

add_executable(rse_bl1_tests)

target_sources(platform_bl1_1
    PRIVATE
        $<$<BOOL:${PLATFORM_DEFAULT_BL1_TEST_EXECUTION}>:${CMAKE_CURRENT_SOURCE_DIR}/run_test_executable.c>
)

target_include_directories(platform_bl1_1
    PUBLIC
        $<$<BOOL:${PLATFORM_DEFAULT_BL1_TEST_EXECUTION}>:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_sources(platform_bl1_2
    PRIVATE
        $<$<BOOL:${PLATFORM_DEFAULT_BL1_TEST_EXECUTION}>:${CMAKE_CURRENT_SOURCE_DIR}/run_test_executable.c>
)

target_include_directories(platform_bl1_2
    PUBLIC
        $<$<BOOL:${PLATFORM_DEFAULT_BL1_TEST_EXECUTION}>:${CMAKE_CURRENT_SOURCE_DIR}>
)

################################################################################

set_target_properties(rse_bl1_tests
    PROPERTIES
        SUFFIX ".axf"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_convert_to_bin_target(rse_bl1_tests)

target_add_scatter_file(rse_bl1_tests
    $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/rse_tests.sct>
    $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/rse_tests.ld>
    $<$<C_COMPILER_ID:IAR>:${CMAKE_CURRENT_SOURCE_DIR}/rse_tests.icf>
)

target_link_options(rse_bl1_tests
    PRIVATE
        $<$<C_COMPILER_ID:GNU>:-Wl,-Map=${CMAKE_BINARY_DIR}/bin/rse_bl1_tests.map>
        $<$<C_COMPILER_ID:ARMClang>:--map>
        $<$<C_COMPILER_ID:IAR>:--map\;${CMAKE_BINARY_DIR}/bin/rse_bl1_tests.map>
)

target_link_options(rse_bl1_tests
    PRIVATE
        --entry=Reset_Handler
)

target_sources(rse_bl1_tests
    PRIVATE
        ../../device/source/startup_rse_bl.c
        ../../device/source/system_core_init.c
        ./rse_test_executable.c
        $<$<BOOL:${CONFIG_GNU_SYSCALL_STUB_ENABLED}>:${CMAKE_SOURCE_DIR}/platform/ext/common/syscalls_stub.c>
)

target_link_libraries(rse_bl1_tests
    PRIVATE
        $<$<BOOL:${TEST_BL1_1}>:bl1_1_tests>
        $<$<BOOL:${TEST_BL1_2}>:bl1_2_tests>
        platform_bl1_1_interface
)

target_compile_definitions(rse_bl1_tests
    PRIVATE
        $<$<BOOL:${TEST_BL1_1}>:TEST_BL1_1>
        $<$<BOOL:${TEST_BL1_2}>:TEST_BL1_2>
)

# The provisioning code should link the ROM lib from ROM, not from SRAM. This
# variable set only applies in this file scope
set(CODE_SHARING_INPUT_FILE_SUFFIX ${CODE_SHARING_OUTPUT_FILE_SUFFIX})
target_link_shared_code(rse_bl1_tests
    bl1_1
    bl1_2
)
