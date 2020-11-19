#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.15)

# Don't load this file if it is specified as a cmake toolchain file
if(NOT TFM_TOOLCHAIN_FILE)
    message(DEPRECATION "SETTING CMAKE_TOOLCHAIN_FILE is deprecated. It has been replaced with TFM_TOOLCHAIN_FILE.")
    return()
endif()

SET(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER armclang)
set(CMAKE_ASM_COMPILER armasm)

set(LINKER_VENEER_OUTPUT_FLAG --import_cmse_lib_out=)
set(COMPILER_CMSE_FLAG $<$<COMPILE_LANGUAGE:C>:-mcmse>)

# This variable name is a bit of a misnomer. The file it is set to is included
# at a particular step in the compiler initialisation. It is used here to
# configure the extensions for object files. Despite the name, it also works
# with the Ninja generator.
set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_LIST_DIR}/cmake/set_extensions.cmake)

macro(tfm_toolchain_reset_compiler_flags)
    set_property(DIRECTORY PROPERTY COMPILE_OPTIONS "")
    string(REGEX REPLACE "\\+nodsp" ".no_dsp" CMAKE_ASM_CPU_FLAG "${CMAKE_SYSTEM_PROCESSOR}")

    add_compile_options(
        $<$<COMPILE_LANGUAGE:C>:-Wno-ignored-optimization-argument>
        $<$<COMPILE_LANGUAGE:C>:-Wno-unused-command-line-argument>
        $<$<COMPILE_LANGUAGE:C>:-Wall>
        # Don't error when the MBEDTLS_NULL_ENTROPY warning is shown
        $<$<COMPILE_LANGUAGE:C>:-Wno-error=cpp>
        $<$<COMPILE_LANGUAGE:C>:-c>
        $<$<COMPILE_LANGUAGE:C>:-fdata-sections>
        $<$<COMPILE_LANGUAGE:C>:-ffunction-sections>
        $<$<COMPILE_LANGUAGE:C>:-fno-builtin>
        $<$<COMPILE_LANGUAGE:C>:-fshort-enums>
        $<$<COMPILE_LANGUAGE:C>:-fshort-wchar>
        $<$<COMPILE_LANGUAGE:C>:-funsigned-char>
        $<$<COMPILE_LANGUAGE:C>:-masm=auto>
        $<$<COMPILE_LANGUAGE:C>:-nostdlib>
        $<$<COMPILE_LANGUAGE:C>:-std=c99>
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<NOT:$<BOOL:${TFM_SYSTEM_FP}>>>:-mfpu=none>
        $<$<AND:$<COMPILE_LANGUAGE:ASM>,$<NOT:$<BOOL:${TFM_SYSTEM_FP}>>>:--fpu=none>
        $<$<COMPILE_LANGUAGE:ASM>:--cpu=${CMAKE_ASM_CPU_FLAG}>
    )
endmacro()

macro(tfm_toolchain_reset_linker_flags)
    set_property(DIRECTORY PROPERTY LINK_OPTIONS "")

    add_link_options(
        --info=summarysizes,sizes,totals,unused,veneers
        --strict
        --symbols
        --xref
        # Suppress link warnings that are consistant (and therefore hopefully
        # harmless)
        # https://developer.arm.com/documentation/100074/0608/linker-errors-and-warnings/list-of-the-armlink-error-and-warning-messages
        # Empty region description
        --diag_suppress=6312
        # Ns section matches pattern
        --diag_suppress=6314
        # Duplicate input files
        --diag_suppress=6304
        $<$<NOT:$<BOOL:${TFM_SYSTEM_FP}>>:--fpu=softvfp>
    )
endmacro()

macro(tfm_toolchain_set_processor_arch)
    set(CMAKE_SYSTEM_PROCESSOR       ${TFM_SYSTEM_PROCESSOR})
    set(CMAKE_SYSTEM_ARCHITECTURE    ${TFM_SYSTEM_ARCHITECTURE})

    set(CMAKE_C_COMPILER_TARGET      arm-${CROSS_COMPILE})
    set(CMAKE_ASM_COMPILER_TARGET    arm-${CROSS_COMPILE})

    if (DEFINED TFM_SYSTEM_MVE)
        if(NOT TFM_SYSTEM_MVE)
            string(APPEND CMAKE_SYSTEM_PROCESSOR "+nomve")
        endif()
    endif()

    if (DEFINED TFM_SYSTEM_FP)
        if(NOT TFM_SYSTEM_FP)
            string(APPEND CMAKE_SYSTEM_PROCESSOR "+nofp")
        endif()
    endif()

    if (DEFINED TFM_SYSTEM_DSP)
        if(NOT TFM_SYSTEM_DSP)
            string(APPEND CMAKE_SYSTEM_PROCESSOR "+nodsp")
        endif()
    endif()

    # Cmake's ARMClang support has several issues with compiler validation. To
    # avoid these, we set the list of supported -mcpu and -march variables to
    # the ones we intend to use so that the validation will never fail.
    include(Compiler/ARMClang)
    set(CMAKE_C_COMPILER_PROCESSOR_LIST ${CMAKE_SYSTEM_PROCESSOR})
    set(CMAKE_C_COMPILER_ARCH_LIST ${CMAKE_SYSTEM_PROCESSOR})
    set(CMAKE_ASM_COMPILER_PROCESSOR_LIST ${CMAKE_SYSTEM_PROCESSOR})
    set(CMAKE_ASM_COMPILER_ARCH_LIST ${CMAKE_SYSTEM_PROCESSOR})
endmacro()

macro(tfm_toolchain_reload_compiler)
    tfm_toolchain_set_processor_arch()
    tfm_toolchain_reset_compiler_flags()
    tfm_toolchain_reset_linker_flags()

    unset(CMAKE_C_FLAGS_INIT)
    unset(CMAKE_C_LINK_FLAGS)
    unset(CMAKE_ASM_FLAGS_INIT)
    unset(CMAKE_ASM_LINK_FLAGS)
    unset(__mcpu_flag_set)
    unset(__march_flag_set)

    include(Compiler/ARMClang)
    __compiler_armclang(C)
    include(Compiler/ARMCC)
    __compiler_armcc(ASM)

    # Cmake's armclang support will set either mcpu or march, but march gives
    # better code size so we manually set it.
    set(CMAKE_C_FLAGS   "-march=${CMAKE_SYSTEM_ARCHITECTURE}")
    set(CMAKE_ASM_FLAGS ${CMAKE_ASM_FLAGS_INIT})

    set(CMAKE_C_LINK_FLAGS   "--cpu=${CMAKE_SYSTEM_PROCESSOR}")
    set(CMAKE_ASM_LINK_FLAGS "--cpu=${CMAKE_SYSTEM_PROCESSOR}")
    # But armlink doesn't support this +dsp syntax
    string(REGEX REPLACE "\\+nodsp" "" CMAKE_C_LINK_FLAGS   "${CMAKE_C_LINK_FLAGS}")
    string(REGEX REPLACE "\\+nodsp" "" CMAKE_ASM_LINK_FLAGS "${CMAKE_ASM_LINK_FLAGS}")
    # And uses different syntax for +nofp
    string(REGEX REPLACE "\\+nofp" ".no_fp" CMAKE_C_LINK_FLAGS   "${CMAKE_C_LINK_FLAGS}")
    string(REGEX REPLACE "\\+nofp" ".no_fp" CMAKE_ASM_LINK_FLAGS "${CMAKE_ASM_LINK_FLAGS}")

    string(REGEX REPLACE "\\+nomve" ".no_mve" CMAKE_C_LINK_FLAGS   "${CMAKE_C_LINK_FLAGS}")
    string(REGEX REPLACE "\\+nomve" ".no_mve" CMAKE_ASM_LINK_FLAGS "${CMAKE_ASM_LINK_FLAGS}")

    # Workaround for issues with --depend-single-line with armasm and Ninja
    if (CMAKE_GENERATOR STREQUAL "Ninja")
        set( CMAKE_DEPFILE_FLAGS_ASM "--depend=<OBJECT>.d")
    endif()

    set(CMAKE_C_FLAGS_MINSIZEREL "-Oz -DNDEBUG")
endmacro()

# Configure environment for the compiler setup run by cmake at the first
# `project` call in <tfm_root>/CMakeLists.txt. After this mandatory setup is
# done, all further compiler setup is done via tfm_toolchain_reload_compiler()
tfm_toolchain_set_processor_arch()
tfm_toolchain_reset_compiler_flags()
tfm_toolchain_reset_linker_flags()

# Behaviour for handling scatter files is so wildly divergent between compilers
# that this macro is required.
macro(target_add_scatter_file target)
    target_link_options(${target}
        PRIVATE
        --scatter=$<TARGET_OBJECTS:${target}_scatter>
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
        COMMAND fromelf
            --bincombined $<TARGET_FILE:${target}>
            --output=${bin_dir}/${target}.bin
    )

    add_custom_target(${target}_elf
        SOURCES ${bin_dir}/${target}.elf
    )
    add_custom_command(OUTPUT ${bin_dir}/${target}.elf
        DEPENDS ${target}
        COMMAND fromelf
            --elf $<TARGET_FILE:${target}>
            --output=${bin_dir}/${target}.elf
    )

    add_custom_target(${target}_hex
        SOURCES ${bin_dir}/${target}.hex
    )
    add_custom_command(OUTPUT ${bin_dir}/${target}.hex
        DEPENDS ${target}
        COMMAND fromelf
            --i32combined $<TARGET_FILE:${target}>
            --output=${bin_dir}/${target}.hex
    )

    add_custom_target(${target}_binaries
        ALL
        DEPENDS ${target}_bin
        DEPENDS ${target}_elf
        DEPENDS ${target}_hex
    )
endmacro()
