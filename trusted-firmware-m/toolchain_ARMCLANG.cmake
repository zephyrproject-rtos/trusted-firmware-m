#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)

set(TFM_CMAKE_TOOLCHAIN_FILE_LOADED YES)

SET(CMAKE_SYSTEM_NAME Generic)
# This setting is overridden in ${TFM_PLATFORM}/preload.cmake. It can be set to
# any value here.
set(CMAKE_SYSTEM_PROCESSOR cortex-m23)

set(CMAKE_C_COMPILER armclang)
set(CMAKE_ASM_COMPILER armclang)

set(LINKER_VENEER_OUTPUT_FLAG --import_cmse_lib_out=)
set(COMPILER_CMSE_FLAG -mcmse)

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
    set(CMAKE_SYSTEM_PROCESSOR ${TFM_SYSTEM_PROCESSOR})
    set(CMAKE_SYSTEM_ARCHITECTURE ${TFM_SYSTEM_ARCHITECTURE})

    set_property(DIRECTORY PROPERTY COMPILE_OPTIONS "")
    set_property(DIRECTORY PROPERTY LINK_OPTIONS "")
    if(${CMAKE_C_COMPILER_VERSION} VERSION_GREATER_EQUAL "6.11")
        add_compile_options(
            --target=arm-arm-none-eabi
            -Wno-ignored-optimization-argument
            -Wno-unused-command-line-argument
            -c
            -fdata-sections
            -ffunction-sections
            -fno-builtin
            -fshort-enums
            -funsigned-char
            -masm=auto
            -nostdlib
            -std=c99
            $<$<NOT:$<BOOL:${TFM_SYSTEM_FP}>>:-mfpu=none>
        )
    else()
        # Compile options for legacy compiler 6.10.1. To be removed as soon that
        # that compiler can be deprecated.
        add_compile_options(
            $<$<COMPILE_LANGUAGE:C>:--target=arm-arm-none-eabi>
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
            $<$<COMPILE_LANGUAGE:C>:-funsigned-char>
            $<$<COMPILE_LANGUAGE:C>:-masm=auto>
            $<$<COMPILE_LANGUAGE:C>:-nostdlib>
            $<$<COMPILE_LANGUAGE:C>:-std=c99>
            $<$<AND:$<COMPILE_LANGUAGE:C>,$<NOT:$<BOOL:${TFM_SYSTEM_FP}>>>:-mfpu=none>
            $<$<COMPILE_LANGUAGE:ASM>:--cpu=${CMAKE_SYSTEM_PROCESSOR}>
            $<$<AND:$<COMPILE_LANGUAGE:ASM>,$<NOT:$<BOOL:${TFM_SYSTEM_FP}>>>:--fpu=none>
        )
        set(COMPILER_CMSE_FLAG $<$<COMPILE_LANGUAGE:C>:-mcmse>)
    endif()
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

    if (DEFINED TFM_SYSTEM_FP)
        if(TFM_SYSTEM_FP)
            # TODO Whether a system requires these extensions appears to depend
            # on the system in question, with no real rule. Since adding +fp
            # will cause compile failures on systems that already have fp
            # enabled, this is commented out for now to avoid those failures. In
            # future, better handling should be implemented.
            # string(APPEND CMAKE_SYSTEM_PROCESSOR "+fp")
        else()
            string(APPEND CMAKE_SYSTEM_PROCESSOR "+nofp")
        endif()
    endif()

    if (DEFINED TFM_SYSTEM_DSP)
        if(TFM_SYSTEM_DSP)
            # TODO Whether a system requires these extensions appears to depend
            # on the system in question, with no real rule. Since adding +dsp
            # will cause compile failures on systems that already have dsp
            # enabled, this is commented out for now to avoid those failures. In
            # future, better handling should be implemented.
            # string(APPEND CMAKE_SYSTEM_PROCESSOR "+dsp")
        else()
            string(APPEND CMAKE_SYSTEM_PROCESSOR "+nodsp")
        endif()
    endif()

    unset(CMAKE_C_FLAGS_INIT)
    unset(CMAKE_C_LINK_FLAGS)
    unset(CMAKE_ASM_FLAGS_INIT)
    unset(CMAKE_ASM_LINK_FLAGS)

    # cmake does not allow the use of +dsp / +nofpu syntax. An override is added
    # here to enable it. First reset the supported CPU list in case this has
    # already been run.
    __armclang_set_processor_list(C CMAKE_C_COMPILER_PROCESSOR_LIST)
    __armclang_set_processor_list(ASM CMAKE_ASM_COMPILER_PROCESSOR_LIST)
    # Then use regex to add +dsp +nodsp +fpu and +nofp options
    # TODO generate this combinatorially
    list(TRANSFORM CMAKE_C_COMPILER_PROCESSOR_LIST REPLACE "^(.+)$" "\\1;\\1+fp;\\1+nofp;")
    list(TRANSFORM CMAKE_C_COMPILER_PROCESSOR_LIST REPLACE "^(.+)$" "\\1;\\1+dsp;\\1+nodsp;")
    list(TRANSFORM CMAKE_ASM_COMPILER_PROCESSOR_LIST REPLACE "^(.+)$" "\\1;\\1+fp;\\1+nofp;")
    list(TRANSFORM CMAKE_ASM_COMPILER_PROCESSOR_LIST REPLACE "^(.+)$" "\\1;\\1+dsp;\\1+nodsp;")

    __compiler_armclang(C)

    if(${CMAKE_C_COMPILER_VERSION} VERSION_GREATER_EQUAL "6.11")
        __compiler_armclang(ASM)
    else()
        # Because armclang<6.11 does not have an integrated assembler that
        # supports the ASM syntax used by the CMSIS startup files, it is
        # required to manuall invoke armasm. CMake does not support that when
        # using armclang as the declared compiler, so instead we use some of the
        # internals from ArmCC which uses armasm by default.
        find_program(ARMASM_PATH armasm HINTS "${_CMAKE_C_TOOLCHAIN_LOCATION}")
        set(CMAKE_ASM_COMPILER ${ARMASM_PATH})
        include(${CMAKE_ROOT}/Modules/Compiler/ARMCC.cmake)
        include(${CMAKE_ROOT}/Modules/Compiler/ARMCC-ASM.cmake)
        __compiler_armcc(ASM)
    endif()

    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS_INIT})
    set(CMAKE_ASM_FLAGS ${CMAKE_ASM_FLAGS_INIT})

    # But armlink doesn't support this +dsp syntax, so take the cpu flag and
    # throw away the plus and everything after.
    string(REGEX REPLACE "(--cpu=.*)\\+[a-z\\+]*[ ]?" "\\1" CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS}")
    string(REGEX REPLACE "(--cpu=.*)\\+[a-z\\+]*[ ]?" "\\1" CMAKE_ASM_LINK_FLAGS "${CMAKE_ASM_LINK_FLAGS}")
endmacro()

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
