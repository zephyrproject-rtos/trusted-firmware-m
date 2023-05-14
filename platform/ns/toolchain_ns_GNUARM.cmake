#-------------------------------------------------------------------------------
# Copyright (c) 2020-2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

find_program(CMAKE_C_COMPILER ${CROSS_COMPILE}-gcc)
find_program(CMAKE_CXX_COMPILER ${CROSS_COMPILE}-g++)

if(CMAKE_C_COMPILER STREQUAL "CMAKE_C_COMPILER-NOTFOUND")
    message(FATAL_ERROR "Could not find compiler: '${CROSS_COMPILE}-gcc'")
endif()

if(CMAKE_CXX_COMPILER STREQUAL "CMAKE_CXX_COMPILER-NOTFOUND")
    message(FATAL_ERROR "Could not find compiler: '${CROSS_COMPILE}-g++'")
endif()

set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})

# Set compiler ID explicitly as it's not detected at this moment
set(CMAKE_C_COMPILER_ID GNU)
# A platfomr sprecific MCPU and architecture flags for 2 toolchains
include(${CONFIG_SPE_PATH}/platform/toolchain_ext.cmake)

add_compile_options(
    -specs=nano.specs
    -Wall
    -Wno-format
    -Wno-return-type
    -Wno-unused-but-set-variable
    -c
    -fdata-sections
    -ffunction-sections
    -fno-builtin
    -fshort-enums
    -funsigned-char
    -mthumb
    -nostdlib
)

add_link_options(
    --entry=Reset_Handler
    -specs=nano.specs
    LINKER:-check-sections
    LINKER:-fatal-warnings
    LINKER:--gc-sections
    LINKER:--no-wchar-size-warning
    LINKER:--print-memory-usage
    LINKER:-Map=tfm_ns.map
    -T $<TARGET_OBJECTS:tfm_ns_scatter>
)

EXECUTE_PROCESS( COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION )
if (GCC_VERSION VERSION_LESS 7.3.1)
    message(FATAL_ERROR "Please use newer GNU Arm compiler version starting from 7.3.1.")
endif()

macro(target_add_scatter_file)

    add_library(tfm_ns_scatter OBJECT
        platform/tfm_common_ns.ld
    )

    set_source_files_properties(platform/tfm_common_ns.ld PROPERTIES
        LANGUAGE C
        KEEP_EXTENSION True)

    target_compile_options(tfm_ns_scatter
        PRIVATE
            -E
            -P
            -xc
    )

    target_link_libraries(tfm_ns_scatter platform_ns)

    add_dependencies(tfm_ns tfm_ns_scatter)

endmacro()
