#-------------------------------------------------------------------------------
# Copyright (c) 2017-2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definitions to compile the "secure_storage" module.
#This file assumes it will be included from a project specific cmakefile, and
#will not create a library or executable.
#Inputs:
#	MBEDTLS_INSTALL_DIR - directory where mbedtls headers and libraries can be found.
#	TFM_ROOT_DIR		- root directory of the TF-M repository.
#Outputs:
#	Will modify include directories to make the source compile.
#	ALL_SRC_C: C source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	ALL_SRC_CXX: C++ source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	ALL_SRC_ASM: assembly source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	Include directories will be modified by using the include_directories() commands as needed.

#Get the current directory where this file is located.
set(SECURE_STORAGE_DIR ${CMAKE_CURRENT_LIST_DIR})

#Check input variables
if (NOT DEFINED ENABLE_SECURE_STORAGE)
	message(FATAL_ERROR "Incomplete build configuration: ENABLE_SECURE_STORAGE is undefined. ")
endif()

if (ENABLE_SECURE_STORAGE)
	if (NOT DEFINED MBEDTLS_INSTALL_DIR)
		message(FATAL_ERROR "Please set MBEDTLS_INSTALL_DIR before including this file.")
	endif()

	if (NOT DEFINED TFM_ROOT_DIR)
		message(FATAL_ERROR "Please set TFM_ROOT_DIR before including this file.")
	endif()

	if (NOT DEFINED SST_ENCRYPTION)
		message(FATAL_ERROR "Incomplete build configuration: SST_ENCRYPTION is undefined. ")
	endif()

	if (NOT DEFINED SST_RAM_FS)
		message(FATAL_ERROR "Incomplete build configuration: SST_RAM_FS is undefined. ")
	endif()

	if (NOT DEFINED SST_VALIDATE_METADATA_FROM_FLASH)
		message(FATAL_ERROR "Incomplete build configuration: SST_VALIDATE_METADATA_FROM_FLASH is undefined. ")
	endif()

	set (SECURE_STORAGE_C_SRC "${SECURE_STORAGE_DIR}/sst_core.c"
		"${SECURE_STORAGE_DIR}/sst_object_system.c"
		"${SECURE_STORAGE_DIR}/sst_asset_management.c"
		"${SECURE_STORAGE_DIR}/sst_utils.c"
		"${SECURE_STORAGE_DIR}/assets/sst_asset_defs.c"
		"${SECURE_STORAGE_DIR}/flash/sst_flash.c")

	if (SST_ENCRYPTION)
		list(APPEND SECURE_STORAGE_C_SRC
			"${SECURE_STORAGE_DIR}/crypto/sst_crypto_interface.c"
			"${SECURE_STORAGE_DIR}/sst_encrypted_object.c"
			)
		set_property(SOURCE ${SECURE_STORAGE_C_SRC} APPEND PROPERTY COMPILE_DEFINITIONS SST_ENCRYPTION)
		set_property(DIRECTORY ${TEST_DIR} APPEND PROPERTY COMPILE_DEFINITIONS SST_ENCRYPTION)
	endif()

	if (SST_VALIDATE_METADATA_FROM_FLASH)
		set_property(SOURCE ${SECURE_STORAGE_C_SRC} APPEND PROPERTY COMPILE_DEFINITIONS SST_VALIDATE_METADATA_FROM_FLASH)
	endif()

	if (SST_RAM_FS)
		set_property(SOURCE ${SECURE_STORAGE_C_SRC} APPEND PROPERTY COMPILE_DEFINITIONS SST_RAM_FS)
	endif()

	if (SST_ENABLE_PARTIAL_ASSET_RW)
		set_property(SOURCE ${SECURE_STORAGE_C_SRC} APPEND PROPERTY COMPILE_DEFINITIONS SST_ENABLE_PARTIAL_ASSET_RW)
	endif()

	#Append all our source files to global lists.
	list(APPEND ALL_SRC_C ${SECURE_STORAGE_C_SRC})
	unset(SECURE_STORAGE_C_SRC)

	#Setting include directories
	embedded_include_directories(PATH ${TFM_ROOT_DIR} ABSOLUTE)
	embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)
	embedded_include_directories(PATH ${MBEDTLS_INSTALL_DIR}/include ABSOLUTE)
	embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/spm ABSOLUTE)
	embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/core ABSOLUTE)
	embedded_include_directories(PATH ${TFM_ROOT_DIR}/platform/ext/common ABSOLUTE)
	embedded_include_directories(PATH ${TFM_ROOT_DIR}/platform/ext/driver ABSOLUTE)

	set(BUILD_CMSIS_CORE Off)
	set(BUILD_RETARGET Off)
	set(BUILD_NATIVE_DRIVERS Off)
	set(BUILD_STARTUP Off)
	set(BUILD_TARGET_CFG Off)
	set(BUILD_TARGET_HARDWARE_KEYS Off)
	set(BUILD_CMSIS_DRIVERS Off)
	set(BUILD_TIME Off)
	set(BUILD_UART_STDOUT Off)
	set(BUILD_FLASH Off)
	if(NOT DEFINED PLATFORM_CMAKE_FILE)
		message (FATAL_ERROR "Platform specific CMake is not defined. Please set PLATFORM_CMAKE_FILE.")
	elseif(NOT EXISTS ${PLATFORM_CMAKE_FILE})
		message (FATAL_ERROR "Platform specific CMake \"${PLATFORM_CMAKE_FILE}\" file does not exist. Please fix value of PLATFORM_CMAKE_FILE.")
	else()
		include(${PLATFORM_CMAKE_FILE})
	endif()

else()
	message(FATAL_ERROR "Build system currently doesn't support selectively disabling of a service.")
endif()
