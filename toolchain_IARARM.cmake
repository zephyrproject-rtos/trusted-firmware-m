#-------------------------------------------------------------------------------
# Copyright (c) 2020, IAR Systems AB. All rights reserved.
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.14)

set(TFM_CMAKE_TOOLCHAIN_FILE_LOADED YES)

SET(CMAKE_SYSTEM_NAME Generic)
# This setting is overridden in ${TFM_PLATFORM}/preload.cmake. It can be set to
# any value here.
set(CMAKE_SYSTEM_PROCESSOR cortex-m23)

set(CMAKE_C_COMPILER iccarm)
set(CMAKE_ASM_COMPILER iasmarm)

set(COMPILER_CMSE_FLAG --cmse)
set(LINKER_VENEER_OUTPUT_FLAG --import_cmse_lib_out= )

# This variable name is a bit of a misnomer. The file it is set to is included
# at a particular step in the compiler initialisation. It is used here to
# configure the extensions for object files. Despite the name, it also works
# with the Ninja generator.
set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_LIST_DIR}/cmake/set_extensions.cmake)

# Cmake makes it _really_ hard to dynamically set the cmake_system_processor
# (used for setting mcpu flags etc). Instead we load
# platform/ext/target/${TFM_PLATFORM}/preload.cmake, which should run this macro
# to reload the compiler autoconfig. Note that it can't be loaded in this file
# as cmake does not allow the use of command-line defined variables in toolchain
# files and the path is platform dependent.
macro(_compiler_reload)
    if(${TFM_SYSTEM_PROCESSOR} STREQUAL "cortex-m0plus")
      set(CMAKE_SYSTEM_PROCESSOR Cortex-M0+)
    else()
      set(CMAKE_SYSTEM_PROCESSOR ${TFM_SYSTEM_PROCESSOR})
    endif()
    set(CMAKE_SYSTEM_ARCHITECTURE ${TFM_SYSTEM_ARCHITECTURE})

    if (DEFINED TFM_SYSTEM_FP)
        if(TFM_SYSTEM_FP)
            # TODO Whether a system requires these extensions appears to depend
            # on the system in question, with no real rule. Since adding .fp
            # will cause compile failures on systems that already have fp
            # enabled, this is commented out for now to avoid those failures. In
            # future, better handling should be implemented.
            # string(APPEND CMAKE_SYSTEM_PROCESSOR ".fp")
        endif()
    endif()

    if (DEFINED TFM_SYSTEM_DSP)
        if(TFM_SYSTEM_DSP)
            # TODO Whether a system requires these extensions appears to depend
            # on the system in question, with no real rule. Since adding .dsp
            # will cause compile failures on systems that already have dsp
            # enabled, this is commented out for now to avoid those failures. In
            # future, better handling should be implemented.
            # string(APPEND CMAKE_SYSTEM_PROCESSOR ".dsp")
        else()
            string(APPEND CMAKE_SYSTEM_PROCESSOR ".no_dsp")
        endif()
    endif()

    set_property(DIRECTORY PROPERTY COMPILE_OPTIONS "")
    set_property(DIRECTORY PROPERTY LINK_OPTIONS "")
    add_compile_options(
        $<$<COMPILE_LANGUAGE:C,CXX>:-e>
        $<$<COMPILE_LANGUAGE:C,CXX>:--dlib_config=full>
        $<$<COMPILE_LANGUAGE:C,CXX>:--vla>
        $<$<COMPILE_LANGUAGE:C,CXX>:--silent>
        $<$<COMPILE_LANGUAGE:C,CXX>:-DNO_TYPEOF>
        $<$<COMPILE_LANGUAGE:C,CXX>:-D_NO_DEFINITIONS_IN_HEADER_FILES>
        $<$<COMPILE_LANGUAGE:C,CXX>:--diag_suppress=Pe546,Pe940,Pa082,Pa084>
    )
    add_link_options(
      --silent
      --semihosting
      --redirect __write=__write_buffered
    )

    unset(CMAKE_C_FLAGS_INIT)
    unset(CMAKE_C_LINK_FLAGS)
    unset(CMAKE_ASM_FLAGS_INIT)
    unset(CMAKE_ASM_LINK_FLAGS)

    set(CMAKE_C_FLAGS_INIT "--cpu ${CMAKE_SYSTEM_PROCESSOR}")
    set(CMAKE_ASM_FLAGS_INIT "--cpu ${CMAKE_SYSTEM_PROCESSOR}")
    set(CMAKE_C_LINK_FLAGS "--cpu ${CMAKE_SYSTEM_PROCESSOR}")
    set(CMAKE_ASM_LINK_FLAGS "--cpu ${CMAKE_SYSTEM_PROCESSOR}")

    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS_INIT})
    set(CMAKE_ASM_FLAGS ${CMAKE_ASM_FLAGS_INIT})

endmacro()

# Behaviour for handling scatter files is so wildly divergent between compilers
# that this macro is required.
macro(target_add_scatter_file target)
    target_link_options(${target}
        PRIVATE
        --config $<TARGET_OBJECTS:${target}_scatter>
    )
    add_dependencies(${target}
        ${target}_scatter
    )

    add_library(${target}_scatter OBJECT)
    foreach(scatter_file ${ARGN})
        target_sources(${target}_scatter
            PRIVATE
                ${scatter_file}
        )
        # Cmake cannot use generator expressions in the
        # set_source_file_properties command, so instead we just parse the regex
        # for the filename and set the property on all files, regardless of if
        # the generator expression would evaluate to true or not.
        string(REGEX REPLACE ".*>:(.*)>$" "\\1" SCATTER_FILE_PATH "${scatter_file}")
        set_source_files_properties(${SCATTER_FILE_PATH}
            PROPERTIES
            LANGUAGE C
        )
    endforeach()

    target_link_libraries(${target}_scatter
        platform_region_defs
        psa_interface
        tfm_partition_defs
    )

    target_compile_options(${target}_scatter
        PRIVATE
            --preprocess=sn $<TARGET_OBJECTS:${target}_scatter>
    )
endmacro()

macro(add_convert_to_bin_target target)
    get_target_property(bin_dir ${target} RUNTIME_OUTPUT_DIRECTORY)

    add_custom_target(${target}_bin
        SOURCES ${bin_dir}/${target}.bin
    )
    add_custom_command(OUTPUT ${bin_dir}/${target}.bin
        DEPENDS ${target}
        COMMAND ielftool
            --silent
            --bin $<TARGET_FILE:${target}>
            ${bin_dir}/${target}.bin
    )

    add_custom_target(${target}_elf
        SOURCES ${bin_dir}/${target}.elf
    )
    add_custom_command(OUTPUT ${bin_dir}/${target}.elf
        DEPENDS ${target}
        COMMAND ielftool
            --silent
            $<TARGET_FILE:${target}>
            ${bin_dir}/${target}.elf
    )

    add_custom_target(${target}_hex
        SOURCES ${bin_dir}/${target}.hex
    )
    add_custom_command(OUTPUT ${bin_dir}/${target}.hex
        DEPENDS ${target}
        COMMAND ielftool
            --silent
            --ihex $<TARGET_FILE:${target}>
            ${bin_dir}/${target}.hex
    )

    add_custom_target(${target}_binaries
        ALL
        DEPENDS ${target}_bin
        DEPENDS ${target}_elf
        DEPENDS ${target}_hex
    )
endmacro()
