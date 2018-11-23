#-------------------------------------------------------------------------------
# Copyright (c) 2017-2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file contains settings to specify how ARMCLANG shall be used

function(check_armclang_input_vars MY_VERSION)
	#Specify where armclang is
	if (NOT DEFINED ARMCLANG_PATH)
		message(FATAL_ERROR "Please set ARMCLANG_PATH to the root directory of the armclang installation. e.g. set(ARMCLANG_PATH \"C:/Program Files/ARMCompiler${MY_VERSION}\")")
	endif()

	STRING(REGEX REPLACE "([0-9]+).([0-9]+).*" "\\1.\\2" _MY_MAJOR_MINOR "${MY_VERSION}")
	STRING(REGEX REPLACE "([0-9]+).([0-9]+).*" "\\1.\\2" _ARMCLANG_MAJOR_MINOR "${ARMCLANG_VER}")

	#Check armclang version.
	if (NOT "${_MY_MAJOR_MINOR}" VERSION_EQUAL "${_ARMCLANG_MAJOR_MINOR}")
		message(FATAL_ERROR "ARMClang version (ARMCLANG_VER=${ARMCLANG_VER}) does not match ${MY_VERSION}")
	endif()

	#Emit warning if needed environment variables are not set.
	if(NOT DEFINED ENV{ARM_TOOL_VARIANT} OR NOT DEFINED ENV{ARM_PRODUCT_PATH})
		message(WARNING "ARM_TOOL_VARIANT or ARM_PRODUCT_PATH environment variables are not set!")
	endif()

	if (NOT DEFINED ARM_CPU_ARCHITECTURE AND NOT DEFINED ARM_CPU_TYPE)
		message(FATAL_ERROR "ARM_CPU_TYPE and ARM_CPU_ARCHITECTURE is not defined! Please include the CPU specific config file before this one.")
	endif()

endfunction()

message(STATUS "Using armclang compiler package v${ARMCLANG_VER} from ${ARMCLANG_PATH}")


#Tell cmake which compiler we use
if (EXISTS "c:/")
	set (CMAKE_C_COMPILER "${ARMCLANG_PATH}/bin/armclang.exe")
	set (CMAKE_CXX_COMPILER "${ARMCLANG_PATH}/bin/armclang.exe")
	set (CMAKE_ASM_COMPILER "${ARMCLANG_PATH}/bin/armasm.exe")
else()
	set (CMAKE_C_COMPILER "${ARMCLANG_PATH}/bin/armclang")
	set (CMAKE_CXX_COMPILER "${ARMCLANG_PATH}/bin/armclang")
	set (CMAKE_ASM_COMPILER "${ARMCLANG_PATH}/bin/armasm")
endif()

if("CXX" IN_LIST languages)
	set(CMAKE_CXX_COMPILER_ID "ARMCLANG" CACHE INTERNAL "CXX compiler ID" FORCE)
	include(Compiler/ARMClang-CXX)
endif()

if("C" IN_LIST languages)
	set(CMAKE_C_COMPILER_ID "ARMCLANG" CACHE INTERNAL "C compiler ID" FORCE)
	include(Compiler/ARMClang-C)
endif()

if("ASM" IN_LIST languages)
	set(CMAKE_C_COMPILER_ID "ARMCLANG" CACHE INTERNAL "ASM compiler ID" FORCE)
	include(Compiler/ARMClang-ASM)
endif()

function(compiler_set_linkercmdfile)
	set( _OPTIONS_ARGS )							#Option (on/off) arguments.
	set( _ONE_VALUE_ARGS TARGET PATH)				#Single option arguments.
	set( _MULTI_VALUE_ARGS DEFINES INCLUDES)		#List arguments
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

	#Check passed parameters
	if(NOT _MY_PARAMS_TARGET)
		message(FATAL_ERROR "compiler_set_linkercmdfile: mandatory parameter 'TARGET' is missing.")
	endif()
	if (NOT TARGET ${_MY_PARAMS_TARGET})
		message(FATAL_ERROR "compiler_set_linkercmdfile: value of parameter 'TARGET' is invalid.")
	endif()

	if(NOT _MY_PARAMS_PATH)
		message(FATAL_ERROR "compiler_set_linkercmdfile: mandatory parameter 'PATH' is missing.")
	endif()
	set(_FILE_PATH ${_MY_PARAMS_PATH})

	#Compose additional command line switches from macro definitions.
	set(_FLAGS "")
	if (_MY_PARAMS_DEFINES)
		foreach(_DEFINE IN LISTS _MY_PARAMS_DEFINES)
			string(APPEND _FLAGS " --predefine=\"-D${_DEFINE}\"")
		endforeach()
	endif()
	#Compose additional command line switches from include paths.
	if (_MY_PARAMS_INCLUDES)
		foreach(_INCLUDE_P IN LISTS _MY_PARAMS_INCLUDES)
			string(APPEND _FLAGS " --predefine=\"-I${_INCLUDE_P}\"")
		endforeach()
	endif()

	#Note: the space before the option is important!
	set_property(TARGET ${_MY_PARAMS_TARGET} APPEND_STRING PROPERTY LINK_FLAGS " ${_FLAGS} --scatter=${_FILE_PATH}")
	set_property(TARGET ${_MY_PARAMS_TARGET} APPEND PROPERTY LINK_DEPENDS ${_FILE_PATH})
	#Tell cmake .map files shall be removed when project is cleaned (make clean)
	get_filename_component(_TARGET_BASE_NAME ${_MY_PARAMS_TARGET} NAME_WE)
	get_directory_property(_ADDITIONAL_MAKE_CLEAN_FILES DIRECTORY "./" ADDITIONAL_MAKE_CLEAN_FILES)
	set_directory_properties(PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${_ADDITIONAL_MAKE_CLEAN_FILES} ${_TARGET_BASE_NAME}.map")
endfunction()

function(compiler_set_cmse_output TARGET FILE_PATH)
	#Note: the space before the option is important!
	set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " --import_cmse_lib_out=${FILE_PATH}")
	#Tell cmake cmse output is a generated object file.
	SET_SOURCE_FILES_PROPERTIES("${FILE_PATH}" PROPERTIES EXTERNAL_OBJECT true GENERATED true)
	#Tell cmake cmse output shall be removed by clean target.
	get_directory_property(_ADDITIONAL_MAKE_CLEAN_FILES DIRECTORY "./" ADDITIONAL_MAKE_CLEAN_FILES)
	set_directory_properties(PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${_ADDITIONAL_MAKE_CLEAN_FILES} ${FILE_PATH}")
endfunction()

function(compiler_merge_library)
	set( _OPTIONS_ARGS )			#Option (on/off) arguments.
	set( _ONE_VALUE_ARGS DEST)		#Single option arguments.
	set( _MULTI_VALUE_ARGS LIBS)	#List arguments
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

	#Check passed parameters
	if(NOT _MY_PARAMS_DEST)
		message(FATAL_ERROR "embedded_merge_library: no destination library target specified.")
	endif()
	#Check if destination is a target
	if(NOT TARGET ${_MY_PARAMS_DEST})
		message(FATAL_ERROR "embedded_merge_library: parameter DEST must be a target already defined.")
	endif()
	#Check if destination is a library
	get_target_property(_tmp ${_MY_PARAMS_DEST} TYPE)
	if(NOT "${_tmp}" STREQUAL "STATIC_LIBRARY")
		message(FATAL_ERROR "embedded_merge_library: parameter DEST must be a static library target.")
	endif()

	#Check list if libraries to be merged
	if(NOT _MY_PARAMS_LIBS)
		message(FATAL_ERROR "embedded_merge_library: no source libraries specified. Please see the LIBS parameter.")
	endif()

	#Mark each library file as a generated external object. This is needed to
	#avoid error because CMake has no info how these can be built.
	SET_SOURCE_FILES_PROPERTIES(
		${_MY_PARAMS_LIBS}
		PROPERTIES
		EXTERNAL_OBJECT true
		GENERATED true)

	#Add additional input to target
	target_sources(${_MY_PARAMS_DEST} PRIVATE ${_MY_PARAMS_LIBS})
endfunction()

function(compiler_generate_binary_output TARGET)
	add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_ARMCCLANG_FROMELF} ARGS --bincombined --output=$<TARGET_FILE_DIR:${TARGET}>/${TARGET}.bin $<TARGET_FILE:${TARGET}>)
endfunction()
