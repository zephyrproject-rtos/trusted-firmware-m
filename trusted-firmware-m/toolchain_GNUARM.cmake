#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Don't load this file if it is specified as a cmake toolchain file
if(NOT TFM_TOOLCHAIN_FILE)
    message(DEPRECATION "SETTING CMAKE_TOOLCHAIN_FILE is deprecated. It has been replaced with TFM_TOOLCHAIN_FILE.")
    return()
endif()

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER ${CROSS_COMPILE}-gcc)
set(CMAKE_ASM_COMPILER ${CROSS_COMPILE}-gcc)

set(LINKER_VENEER_OUTPUT_FLAG -Wl,--cmse-implib,--out-implib=)
set(COMPILER_CMSE_FLAG -mcmse)

# This variable name is a bit of a misnomer. The file it is set to is included
# at a particular step in the compiler initialisation. It is used here to
# configure the extensions for object files. Despite the name, it also works
# with the Ninja generator.
set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_LIST_DIR}/cmake/set_extensions.cmake)

macro(tfm_toolchain_reset_compiler_flags)
    set_property(DIRECTORY PROPERTY COMPILE_OPTIONS "")

    add_compile_options(
        --specs=nano.specs
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
        -std=c99
        $<$<NOT:$<BOOL:${TFM_SYSTEM_FP}>>:-msoft-float>
    )
endmacro()

macro(tfm_toolchain_reset_linker_flags)
    set_property(DIRECTORY PROPERTY LINK_OPTIONS "")

    add_link_options(
        --entry=Reset_Handler
        --specs=nano.specs
        LINKER:-check-sections
        LINKER:-fatal-warnings
        LINKER:--gc-sections
        LINKER:--no-wchar-size-warning
        LINKER:--print-memory-usage
    )
endmacro()

macro(tfm_toolchain_set_processor_arch)
    set(CMAKE_SYSTEM_PROCESSOR ${TFM_SYSTEM_PROCESSOR})
    set(CMAKE_SYSTEM_ARCHITECTURE ${TFM_SYSTEM_ARCHITECTURE})

    if (DEFINED TFM_SYSTEM_DSP)
        if(NOT TFM_SYSTEM_DSP)
            string(APPEND CMAKE_SYSTEM_PROCESSOR "+nodsp")
        endif()
    endif()
endmacro()

macro(tfm_toolchain_reload_compiler)
    tfm_toolchain_set_processor_arch()
    tfm_toolchain_reset_compiler_flags()
    tfm_toolchain_reset_linker_flags()

    unset(CMAKE_C_FLAGS_INIT)
    unset(CMAKE_ASM_FLAGS_INIT)

    set(CMAKE_C_FLAGS_INIT "-mcpu=${CMAKE_SYSTEM_PROCESSOR}")
    set(CMAKE_ASM_FLAGS_INIT "-mcpu=${CMAKE_SYSTEM_PROCESSOR}")
    set(CMAKE_C_LINK_FLAGS "-mcpu=${CMAKE_SYSTEM_PROCESSOR}")
    set(CMAKE_ASM_LINK_FLAGS "-mcpu=${CMAKE_SYSTEM_PROCESSOR}")

    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS_INIT})
    set(CMAKE_ASM_FLAGS ${CMAKE_ASM_FLAGS_INIT})
endmacro()

# Configure environment for the compiler setup run by cmake at the first
# `project` call in <tfm_root>/CMakeLists.txt. After this mandatory setup is
# done, all further compiler setup is done via tfm_toolchain_reload_compiler()
tfm_toolchain_reload_compiler()

macro(target_add_scatter_file target)
    target_link_options(${target}
        PRIVATE
        -T $<TARGET_OBJECTS:${target}_scatter>
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
            -E
            -P
            -xc
    )
endmacro()

macro(add_convert_to_bin_target target)
    get_target_property(bin_dir ${target} RUNTIME_OUTPUT_DIRECTORY)

    add_custom_target(${target}_bin
        SOURCES ${bin_dir}/${target}.bin
    )
    add_custom_command(OUTPUT ${bin_dir}/${target}.bin
        DEPENDS ${target}
        COMMAND ${CMAKE_OBJCOPY}
            -O binary $<TARGET_FILE:${target}>
            ${bin_dir}/${target}.bin
    )

    add_custom_target(${target}_elf
        SOURCES ${bin_dir}/${target}.elf
    )
    add_custom_command(OUTPUT ${bin_dir}/${target}.elf
        DEPENDS ${target}
        COMMAND ${CMAKE_OBJCOPY}
            -O elf32-littlearm $<TARGET_FILE:${target}>
            ${bin_dir}/${target}.elf
    )

    add_custom_target(${target}_hex
        SOURCES ${bin_dir}/${target}.hex
    )
    add_custom_command(OUTPUT ${bin_dir}/${target}.hex
        DEPENDS ${target}
        COMMAND ${CMAKE_OBJCOPY}
            -O ihex $<TARGET_FILE:${target}>
            ${bin_dir}/${target}.hex
    )

    add_custom_target(${target}_binaries
        ALL
        DEPENDS ${target}_bin
        DEPENDS ${target}_elf
        DEPENDS ${target}_hex
    )
endmacro()
