#-------------------------------------------------------------------------------
# Copyright (c) 2017-2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definitions to compile the "secure storage test" module.
#This file assumes it will be included from a project specific cmakefile, and
#will not create a library or executable.
#Inputs:
#	TFM_ROOT_DIR - root directory of the TF-M repo.
#
#Outputs:
#	Will modify include directories to make the source compile.
#	ALL_SRC_C: C source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	ALL_SRC_CXX: C++ source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	ALL_SRC_ASM: assembly source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	Include directories will be modified by using the include_directories() commands as needed.

#Get the current directory where this file is located.
set(SECURE_STORAGE_TEST_DIR ${CMAKE_CURRENT_LIST_DIR})
if(NOT DEFINED TFM_ROOT_DIR)
	message(FATAL_ERROR "Please set TFM_ROOT_DIR before including this file.")
endif()

if(NOT DEFINED CMSIS_5_DIR)
	message(FATAL_ERROR "Please set CMSIS_5_DIR before including this file.")
endif()

if (NOT DEFINED ENABLE_SECURE_STORAGE_SERVICE_TESTS)
	message(FATAL_ERROR "Incomplete build configuration: ENABLE_SECURE_STORAGE_SERVICE_TESTS is undefined. ")
elseif (ENABLE_SECURE_STORAGE_SERVICE_TESTS)
	list(APPEND ALL_SRC_C_S "${SECURE_STORAGE_TEST_DIR}/secure/s_test_helpers.c"
			"${SECURE_STORAGE_TEST_DIR}/secure/sst_sec_interface_testsuite.c"
			"${SECURE_STORAGE_TEST_DIR}/secure/sst_reliability_testsuite.c"
			"${SECURE_STORAGE_TEST_DIR}/secure/sst_policy_testsuite.c"
		)

	list(APPEND ALL_SRC_C_NS "${SECURE_STORAGE_TEST_DIR}/non_secure/sst_ns_interface_testsuite.c")

	if (NOT DEFINED SST_TEST_SERVICES)
		message(FATAL_ERROR "Incomplete build configuration: SST_TEST_SERVICES is undefined.")
	elseif (SST_TEST_SERVICES)
		list(APPEND ALL_SRC_C_NS "${SECURE_STORAGE_TEST_DIR}/non_secure/sst_ns_ref_access_testsuite.c")
	endif()

	#Setting include directories
	embedded_include_directories(PATH ${TFM_ROOT_DIR} ABSOLUTE)
	embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)

	embedded_include_directories(PATH ${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Include ABSOLUTE)
	embedded_include_directories(PATH ${CMSIS_5_DIR}/CMSIS/RTOS2/Include ABSOLUTE)
	embedded_include_directories(PATH ${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Config  ABSOLUTE)
endif()
