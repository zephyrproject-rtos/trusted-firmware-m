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

# A platfomr sprecific MCPU and architecture flags for NS side
include(${CONFIG_SPE_PATH}/platform/cpuarch.cmake)

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

# Specify the linker script used to link `target`.
# Behaviour for handling linker scripts is so wildly divergent between compilers
# that this macro is required.
#
# Vendor platform can set a linker script as property INTERFACE_LINK_DEPENDS of platform_ns.
# `target` can fetch the linker script from platform_ns.
#
# Alternatively, NS build can call target_add_scatter_file() with the install directory of
# linker script.
#     target_add_scatter_file(target, install_dir)
#
# target_add_scatter_file() fetch a linker script from the install directory.
macro(target_add_scatter_file target)

    get_target_property(scatter_file
                        platform_ns
                        INTERFACE_LINK_DEPENDS
    )

    # If scatter_file is not passed from platform_ns
    # Try if any linker script is exported in install directory
    # The intall directory is passed as an optinal argument
    if(${scatter_file} STREQUAL "scatter_file-NOTFOUND")
        set(install_dir ${ARGN})
        list(LENGTH install_dir nr_install_dir)

        # If nr_install_dir == 1, search for sct file under install dir
        if(${nr_install_dir} EQUAL 1)
            file(GLOB scatter_file "${install_dir}/*.ld")
        endif()
    endif()

    if(NOT EXISTS ${scatter_file})
        message(FATAL_ERROR "Unable to find NS scatter file ${scatter_file}")
    endif()

    add_library(${target}_scatter OBJECT)
    target_sources(${target}_scatter
        PRIVATE
            ${scatter_file}
    )

    set_source_files_properties(${scatter_file} PROPERTIES
        LANGUAGE C
        KEEP_EXTENSION True)

    target_compile_options(${target}_scatter
        PRIVATE
            -E
            -P
            -xc
    )

    target_link_libraries(${target}_scatter platform_ns)

    add_dependencies(${target} ${target}_scatter)

endmacro()
