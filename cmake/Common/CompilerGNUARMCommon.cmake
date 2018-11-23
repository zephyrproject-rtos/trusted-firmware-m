#-------------------------------------------------------------------------------
# Copyright (c) 2017-2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file contains settings to specify how GNUARM shall be used

function(check_gnuarm_input_vars MY_VERSION)
	#Specify where gnuarm is
	if (NOT DEFINED GNUARM_PATH)
		message(FATAL_ERROR "Please set GNUARM_PATH to the root directory of the gnuarm installation. e.g. set(GNUARM_PATH \"C:/Program Files (x86)/GNU Tools ARM Embedded/6 2017-q1-update/\"")
	endif()

	STRING(REGEX REPLACE "([0-9]+).([0-9]+).*" "\\1.\\2" _MY_MAJOR_MINOR "${MY_VERSION}")
	STRING(REGEX REPLACE "([0-9]+).([0-9]+).*" "\\1.\\2" _GNUARM_MAJOR_MINOR "${GNUARM_VER}")

	#Check gnuarm version.
	if (NOT "${_MY_MAJOR_MINOR}" VERSION_EQUAL "${_GNUARM_MAJOR_MINOR}")
		message(FATAL_ERROR "GNUARM version (GNUARM_VER=${GNUARM_VER}) does not match ${MY_VERSION}")
	endif()

	if (NOT DEFINED ARM_CPU_ARCHITECTURE AND NOT DEFINED ARM_CPU_TYPE)
		message(FATAL_ERROR "ARM_CPU_TYPE and ARM_CPU_ARCHITECTURE is not defined! Please include the CPU specific config file before this one.")
	endif()

endfunction()

message(STATUS "Using gcc compiler package v${GNUARM_VER} from ${GNUARM_PATH}")


#Tell cmake which compiler we use
if (EXISTS "c:/")
	set (CMAKE_C_COMPILER "${GNUARM_PATH}/bin/arm-none-eabi-gcc.exe")
	set (CMAKE_CXX_COMPILER "${GNUARM_PATH}/bin/arm-none-eabi-g++.exe")
	set (CMAKE_ASM_COMPILER "${GNUARM_PATH}/bin/arm-none-eabi-gcc.exe")
else()
	set (CMAKE_C_COMPILER "${GNUARM_PATH}/bin/arm-none-eabi-gcc")
	set (CMAKE_CXX_COMPILER "${GNUARM_PATH}/bin/arm-none-eabi-g++")
	set (CMAKE_ASM_COMPILER "${GNUARM_PATH}/bin/arm-none-eabi-gcc")
endif()

if("CXX" IN_LIST languages)
	set(CMAKE_CXX_COMPILER_ID "GNUARM" CACHE INTERNAL "CXX compiler ID" FORCE)
	include(Compiler/GNUARM-CXX)
endif()

if("C" IN_LIST languages)
	set(CMAKE_C_COMPILER_ID "GNUARM" CACHE INTERNAL "C compiler ID" FORCE)
	include(Compiler/GNUARM-C)
endif()

if("ASM" IN_LIST languages)
	set(CMAKE_C_COMPILER_ID "GNUARM" CACHE INTERNAL "ASM compiler ID" FORCE)
	include(Compiler/GNUARM-ASM)
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
			list(APPEND _FLAGS "-D${_DEFINE}")
		endforeach()
	endif()
	#Compose additional command line switches from include paths.
	if (_MY_PARAMS_INCLUDES)
		foreach(_INCLUDE_P IN LISTS _MY_PARAMS_INCLUDES)
			list(APPEND _FLAGS "-I${_INCLUDE_P}")
		endforeach()
	endif()

	#Create additional target if linker script needs to be pre-processed.
	if (_MY_PARAMS_DEFINES OR _MY_PARAMS_INCLUDES)
		#Name of pre-processed linker script file.
		set(FINAL_LD_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${_MY_PARAMS_TARGET}.ld.i")
		#Name of the target doing the pre-processing
		set(LD_PP_TARGET_NAME "${_MY_PARAMS_TARGET}_ldpp")
		#The target definition.
		add_custom_target(${LD_PP_TARGET_NAME}
			COMMENT "Pre-processing linker command file ${_MY_PARAMS_PATH}..."
			COMMAND ${CMAKE_C_COMPILER} -E -P -xc ${_FLAGS} -o ${FINAL_LD_FILE_NAME} ${_MY_PARAMS_PATH}
			DEPENDS ${_MY_PARAMS_PATH}
			BYPRODUCTS ${FINAL_LD_FILE_NAME}
		)
		#Make the original target depend on the new one.
		add_dependencies(${_MY_PARAMS_TARGET} ${LD_PP_TARGET_NAME})
		#Tell cmake to delete the intermediate linker script when the clean rule
		#is executed.
		get_directory_property(_ADDITIONAL_MAKE_CLEAN_FILES DIRECTORY "./" ADDITIONAL_MAKE_CLEAN_FILES)
		set_directory_properties(PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${_ADDITIONAL_MAKE_CLEAN_FILES} ${FINAL_LD_FILE_NAME}")
		#Set the path to linker script point to the intermediate file.
		set(_FILE_PATH ${FINAL_LD_FILE_NAME})
	endif()

	#Note: the space before the option is important!
	set_property(TARGET ${_MY_PARAMS_TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -T${_FILE_PATH}")
	set_property(TARGET ${_MY_PARAMS_TARGET} APPEND PROPERTY LINK_DEPENDS ${_FILE_PATH})
	#Tell cmake .map files shall be removed when project is cleaned (make clean)
	get_filename_component(_TARGET_BASE_NAME ${_MY_PARAMS_TARGET} NAME_WE)
	get_directory_property(_ADDITIONAL_MAKE_CLEAN_FILES DIRECTORY "./" ADDITIONAL_MAKE_CLEAN_FILES)
	set_directory_properties(PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${_ADDITIONAL_MAKE_CLEAN_FILES} ${_TARGET_BASE_NAME}.map")
endfunction()

function(compiler_set_cmse_output TARGET FILE_PATH)
	#Note: the space before the option is important!
	set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -Wl,--cmse-implib,--out-implib=${FILE_PATH}")
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
		message(FATAL_ERROR "compiler_merge_library: no destination target specified. Please see the DEST parameter.")
	endif()
	#Check if destination is a library
	get_target_property(_tmp ${_MY_PARAMS_DEST} TYPE)
	if(NOT "${_tmp}" STREQUAL "STATIC_LIBRARY")
		message(FATAL_ERROR "compiler_merge_library: parameter DEST must be a static library target.")
	endif()
	set(_DEST ${_MY_PARAMS_DEST})

	if(NOT _MY_PARAMS_LIBS)
		message(FATAL_ERROR "compiler_merge_library: no source libraries specified. Please see the LIBS parameter.")
	endif()
	set(_LIBS ${_MY_PARAMS_LIBS})

	##Find the cmake script doing the merge.
	find_file(_MERGE_SCRIPT "GNUArMerge.cmake" PATHS ${CMAKE_MODULE_PATH} PATH_SUFFIXES Common NO_DEFAULT_PATH)

 	#Now add a custom command for each source library to our custom target to
 	#merge into the destination.
	foreach(SRC_LIB ${_LIBS})
		get_filename_component(_SRC_LIB_NAME "${SRC_LIB}"  NAME)
		add_custom_command(TARGET ${_DEST} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -DCMAKE_AR=${CMAKE_AR} -DSOURCE=${SRC_LIB} -DDESTINATION=$<TARGET_FILE:${_DEST}> -P ${_MERGE_SCRIPT}
			COMMENT "\t\tmerging objects from ${_SRC_LIB_NAME}")
	endforeach()
endfunction()

function(compiler_generate_binary_output TARGET)
	add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_GNUARM_OBJCOPY} ARGS -O binary $<TARGET_FILE:${TARGET}> $<TARGET_FILE_DIR:${TARGET}>/${TARGET}.bin)
endfunction()
