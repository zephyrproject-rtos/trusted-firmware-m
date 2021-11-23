#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
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

find_program(CMAKE_C_COMPILER ${CROSS_COMPILE}-gcc)

if(CMAKE_C_COMPILER STREQUAL "CMAKE_C_COMPILER-NOTFOUND")
    message(FATAL_ERROR "Could not find compiler: '${CROSS_COMPILE}-gcc'")
endif()

set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})

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
        $<$<OR:$<BOOL:${TFM_DEBUG_SYMBOLS}>,$<BOOL:${TFM_CODE_COVERAGE}>>:-g>
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
    if (DEFINED TFM_SYSTEM_PROCESSOR)
        set(CMAKE_SYSTEM_PROCESSOR ${TFM_SYSTEM_PROCESSOR})

        if (DEFINED TFM_SYSTEM_DSP)
            if (NOT TFM_SYSTEM_DSP)
                string(APPEND CMAKE_SYSTEM_PROCESSOR "+nodsp")
            endif()
        endif()
        if(GCC_VERSION VERSION_GREATER_EQUAL "8.0.0")
            if (DEFINED CONFIG_TFM_SPE_FP)
                if(CONFIG_TFM_SPE_FP STREQUAL "0")
                    string(APPEND CMAKE_SYSTEM_PROCESSOR "+nofp")
                endif()
            endif()
        endif()
    endif()

    # CMAKE_SYSTEM_ARCH variable is not a built-in CMAKE variable. It is used to
    # set the compile and link flags when TFM_SYSTEM_PROCESSOR is not specified.
    # The variable name is choosen to align with the ARMCLANG toolchain file.
    set(CMAKE_SYSTEM_ARCH         ${TFM_SYSTEM_ARCHITECTURE})

    if (DEFINED TFM_SYSTEM_DSP)
        # +nodsp modifier is only supported from GCC version 8.
        # CMAKE_C_COMPILER_VERSION is not guaranteed to be defined.
        EXECUTE_PROCESS( COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION )
        if(GCC_VERSION VERSION_GREATER_EQUAL "8.0.0")
            # armv8.1-m.main arch does not have +nodsp option
            if ((NOT TFM_SYSTEM_ARCHITECTURE STREQUAL "armv8.1-m.main") AND
                NOT TFM_SYSTEM_DSP)
                string(APPEND CMAKE_SYSTEM_ARCH "+nodsp")
            endif()
        endif()
    endif()

    if(GCC_VERSION VERSION_GREATER_EQUAL "8.0.0")
        if (DEFINED CONFIG_TFM_SPE_FP)
            if(CONFIG_TFM_SPE_FP STRGREATER 0)
                string(APPEND CMAKE_SYSTEM_ARCH "+fp")
            endif()
        endif()
    endif()
endmacro()

macro(tfm_toolchain_reload_compiler)
    tfm_toolchain_set_processor_arch()
    tfm_toolchain_reset_compiler_flags()
    tfm_toolchain_reset_linker_flags()

    if (CMAKE_C_COMPILER_VERSION VERSION_EQUAL 10.2.1)
        message(FATAL_ERROR "GNU Arm compiler version 10-2020-q4-major has an issue in CMSE support."
                            " Select other GNU Arm compiler versions instead."
                            " See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99157 for the issue detail.")
    endif()

    unset(CMAKE_C_FLAGS_INIT)
    unset(CMAKE_ASM_FLAGS_INIT)

    if (DEFINED TFM_SYSTEM_PROCESSOR)
        set(CMAKE_C_FLAGS_INIT "-mcpu=${CMAKE_SYSTEM_PROCESSOR}")
        set(CMAKE_ASM_FLAGS_INIT "-mcpu=${CMAKE_SYSTEM_PROCESSOR}")
        set(CMAKE_C_LINK_FLAGS "-mcpu=${CMAKE_SYSTEM_PROCESSOR}")
        set(CMAKE_ASM_LINK_FLAGS "-mcpu=${CMAKE_SYSTEM_PROCESSOR}")
    else()
        set(CMAKE_C_FLAGS_INIT "-march=${CMAKE_SYSTEM_ARCH}")
        set(CMAKE_ASM_FLAGS_INIT "-march=${CMAKE_SYSTEM_ARCH}")
        set(CMAKE_C_LINK_FLAGS "-march=${CMAKE_SYSTEM_ARCH}")
        set(CMAKE_ASM_LINK_FLAGS "-march=${CMAKE_SYSTEM_ARCH}")
    endif()

    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS_INIT})
    set(CMAKE_ASM_FLAGS ${CMAKE_ASM_FLAGS_INIT})

    set(BL2_COMPILER_CP_FLAG -mfloat-abi=soft)

    if (CONFIG_TFM_SPE_FP STREQUAL "2")
        set(COMPILER_CP_FLAG -mfloat-abi=hard -mfpu=${CONFIG_TFM_FP_ARCH})
        set(LINKER_CP_OPTION -mfloat-abi=hard -mfpu=${CONFIG_TFM_FP_ARCH})
    elseif (CONFIG_TFM_SPE_FP STREQUAL "1")
        set(COMPILER_CP_FLAG -mfloat-abi=softfp -mfpu=${CONFIG_TFM_FP_ARCH})
        set(LINKER_CP_OPTION -mfloat-abi=softfp -mfpu=${CONFIG_TFM_FP_ARCH})
    else()
        set(COMPILER_CP_FLAG -mfloat-abi=soft)
        set(LINKER_CP_OPTION -mfloat-abi=soft)
    endif()
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
            KEEP_EXTENSION True # Don't use .o extension for the preprocessed file
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

# Macro for sharing code among independent binaries. This function extracts
# some parts of the code based on a symbol template file and creates a text
# file, which contains the symbols with their absolute addresses, which can be
# picked up by the linker when linking the other target.
# INPUTS:
#     TARGET -                -  Target to extract the symbols/objects from
#     SHARED_SYMBOL_TEMPLATE  -  Template with names of symbols to share
macro(compiler_create_shared_code TARGET SHARED_SYMBOL_TEMPLATE)
    # Create a temporary file, which contains all extracted symbols from 'TARGET'
    set(ALL_SYMBOLS ${CMAKE_CURRENT_BINARY_DIR}/all_symbols.txt)

    # Find the CMake script doing the symbol filtering.
    find_file(FILTER_SYMBOLS_SCRIPT "FilterSharedSymbols.cmake" PATHS ${CMAKE_MODULE_PATH} PATH_SUFFIXES Common NO_DEFAULT_PATH)
    find_file(STRIP_UNSHARED_CODE   "StripUnsharedCode.cmake"   PATHS ${CMAKE_MODULE_PATH} PATH_SUFFIXES Common NO_DEFAULT_PATH)

    find_program(GNUARM_NM arm-none-eabi-nm)
    if (GNUARM_NM STREQUAL "GNUARM_NM-NOTFOUND")
        message(FATAL_ERROR "toolchain_GNUARM.cmake: mandatory tool '${GNUARM_NM}' is missing.")
    endif()

    # Multiple steps are required:
    #   - Extract all symbols from sharing target
    #   - Filter the unwanted symbols from all_symbols.txt
    #   - Create a stripped shared_code.axf file which contains only the symbols which are meant to be shared
    add_custom_command(TARGET ${TARGET}
                        POST_BUILD

                        COMMAND ${GNUARM_NM}
                        ARGS
                            ${CMAKE_BINARY_DIR}/bin/${TARGET}.axf > ${ALL_SYMBOLS}
                        COMMENT "Dumping all symbols"

                        COMMAND ${CMAKE_COMMAND}
                            -DSHARED_SYMBOL_TEMPLATE=${SHARED_SYMBOL_TEMPLATE}
                            -DALL_SYMBOLS=${ALL_SYMBOLS}
                            -P ${FILTER_SYMBOLS_SCRIPT}
                        BYPRODUCTS
                            ${CMAKE_CURRENT_BINARY_DIR}/shared_symbols_name.txt
                            ${CMAKE_CURRENT_BINARY_DIR}/shared_symbols_addr.txt
                        COMMENT "Filtering shared symbols"

                        COMMAND ${CMAKE_COMMAND}
                            -E copy ${CMAKE_BINARY_DIR}/bin/${TARGET}.axf ${CMAKE_CURRENT_BINARY_DIR}/shared_code.axf
                        COMMENT "Copy and rename ${TARGET} to strip"

                        COMMAND ${CMAKE_COMMAND}
                            -DSHARED_SYMBOLS_FILE=${CMAKE_CURRENT_BINARY_DIR}/shared_symbols_name.txt
                            -DEXECUTABLE_TO_STRIP=${CMAKE_CURRENT_BINARY_DIR}/shared_code.axf
                            -P ${STRIP_UNSHARED_CODE}
                        COMMENT "Stripping unshared code from ${CMAKE_CURRENT_BINARY_DIR}/shared_code.axf"
    )
endmacro()

macro(compiler_weaken_symbols TARGET SHARED_CODE_PATH ORIG_TARGET LIB_LIST)
    # Find the CMake scripts
    find_file(WEAKEN_SYMBOLS_SCRIPT "WeakenSymbols.cmake" PATHS ${CMAKE_MODULE_PATH} PATH_SUFFIXES Common NO_DEFAULT_PATH)

    add_custom_command(TARGET ${TARGET}
                        PRE_LINK

                        COMMAND ${CMAKE_COMMAND}
                            -DLIB_LIST='${LIB_LIST}'
                            -DSHARED_CODE_PATH=${SHARED_CODE_PATH}
                            -P ${WEAKEN_SYMBOLS_SCRIPT}
                        COMMENT "Set conflicting symbols to be weak in the original libraries to avoid collision")

    # If sharing target is defined by TF-M build then setup dependency
    if(NOT ${ORIG_TARGET} STREQUAL "EXTERNAL_TARGET")
        add_dependencies(${TARGET} ${ORIG_TARGET})
    endif()
endmacro()

# Macro for linking shared code to given target. Location of shared code could
# be outside of the TF-M project. Its location can be defined with the CMake
# command line argument "SHARED_CODE_PATH". The file containing the shared objects
# must be named "shared_symbols_addr.txt".
# INPUTS:
#     TARGET            Target to link the shared code to
#     SHARED_CODE_PATH  Shared code located in this folder
#     ORIG_TARGET       Target that shared code was extraced from <TARGET | "EXTERNAL_TARGET">
#     LIB_LIST          List of libraries which are linked to top level target
macro(compiler_link_shared_code TARGET SHARED_CODE_PATH ORIG_TARGET LIB_LIST)
    # GNUARM requires to link a stripped version (only containing the shared symbols) of the
    # original executable to the executable which want to rely on the shared symbols.
    target_link_options(${TARGET} PRIVATE -Wl,-R${SHARED_CODE_PATH}/shared_code.axf)

    compiler_weaken_symbols(${TARGET}
                            ${SHARED_CODE_PATH}
                            ${ORIG_TARGET}
                            "${LIB_LIST}"
    )
endmacro()
