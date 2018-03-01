#-------------------------------------------------------------------------------
# Copyright (c) 2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)

function(mcuboot_create_boot_payload)
	set( _OPTIONS_ARGS)                                          #Option (on/off) arguments (e.g. IGNORE_CASE)
	set( _ONE_VALUE_ARGS S_BIN NS_BIN FULL_BIN SIGN_BIN POSTFIX) #Single option arguments (e.g. PATH "./foo/bar")
	set( _MULTI_VALUE_ARGS)                                      #List arguments (e.g. LANGUAGES C ASM CXX)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN})

	if (NOT DEFINED _MY_PARAMS_S_BIN)
		message(FATAL_ERROR "mcuboot_create_boot_payload(): mandatory parameter 'S_BIN' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_NS_BIN)
		message(FATAL_ERROR "mcuboot_create_boot_payload(): mandatory parameter 'NS_BIN' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_FULL_BIN)
		message(FATAL_ERROR "mcuboot_create_boot_payload(): mandatory parameter 'FULL_BIN' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_SIGN_BIN)
		message(FATAL_ERROR "mcuboot_create_boot_payload(): mandatory parameter 'SIGN_BIN' missing.")
	endif()

	#Find Python3.x interpreter
	find_package(PythonInterp 3)
	if (NOT PYTHONINTERP_FOUND)
		message(FATAL_ERROR "Failed to find Python3.x interpreter. Pyhton3 must be installed and available on the PATH.")
	endif()

	if(NOT DEFINED FLASH_LAYOUT)
		message(FATAL_ERROR "ERROR: Incomplete Configuration: FLASH_LAYOUT is not defined.")
	endif()

	add_custom_command(TARGET ${_MY_PARAMS_NS_BIN}
						POST_BUILD

						#Create concatenated binary image from the two binary file
						COMMAND ${PYTHON_EXECUTABLE} ${MCUBOOT_DIR}/scripts/assemble.py
						ARGS -l ${FLASH_LAYOUT}
							 -s $<TARGET_FILE_DIR:${_MY_PARAMS_S_BIN}>/${_MY_PARAMS_S_BIN}.bin
							 -n $<TARGET_FILE_DIR:${_MY_PARAMS_NS_BIN}>/${_MY_PARAMS_NS_BIN}.bin
							 -o ${CMAKE_BINARY_DIR}/${_MY_PARAMS_FULL_BIN}.bin

						#Sign concatenated binary image with default public key in mcuboot folder
						COMMAND ${PYTHON_EXECUTABLE} ${MCUBOOT_DIR}/scripts/imgtool.py
						ARGS sign
							 -k ${MCUBOOT_DIR}/root-rsa-2048.pem
							 --align 1
							 -v 1.0
							 -H 0x400
							 --pad ${SIGN_BIN_SIZE}
							 ${CMAKE_BINARY_DIR}/${_MY_PARAMS_FULL_BIN}.bin
							 ${CMAKE_BINARY_DIR}/${_MY_PARAMS_SIGN_BIN}.bin)
endfunction()