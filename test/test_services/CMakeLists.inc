#-------------------------------------------------------------------------------
# Copyright (c) 2017-2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definitions to compile the "secure services" module.
#This file assumes it will be included from a project specific cmakefile, and
#will not create a library or executable.
#Inputs:
#	TFM_ROOT_DIR - root directory of the TF-M repository.
#
#Outputs:
#	Will modify include directories to make the source compile.
#	ALL_SRC_C: C source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	ALL_SRC_CXX: C++ source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	ALL_SRC_ASM: assembly source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	Include directories will be modified by using the include_directories() commands as needed.

#Get the current directory where this file is located.
set(CORE_TEST_DIR ${CMAKE_CURRENT_LIST_DIR})

#Check input variables
if (NOT DEFINED TFM_ROOT_DIR)
	message(FATAL_ERROR "Please set TFM_ROOT_DIR before including this file.")
endif()

if (NOT DEFINED ENABLE_CORE_TESTS)
	message(FATAL_ERROR "Incomplete build configuration: ENABLE_CORE_TESTS is undefined. ")
elseif(ENABLE_CORE_TESTS)
	list(APPEND ALL_SRC_C_S "${CORE_TEST_DIR}/tfm_core_test/tfm_ss_core_test.c"
		"${CORE_TEST_DIR}/tfm_core_test/tfm_ss_core_test_veneers.c"
		)
endif()

if (NOT DEFINED ENABLE_CORE_TESTS_2)
	message(FATAL_ERROR "Incomplete build configuration: ENABLE_CORE_TESTS_2 is undefined. ")
elseif(ENABLE_CORE_TESTS_2)
	list(APPEND ALL_SRC_C_S "${CORE_TEST_DIR}/tfm_core_test_2/tfm_ss_core_test_2_veneers.c"
		"${CORE_TEST_DIR}/tfm_core_test_2/tfm_ss_core_test_2.c"
		)
endif()

embedded_include_directories(PATH ${TFM_ROOT_DIR} ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)

set(PLATFORM_DIR ${TFM_ROOT_DIR}/platform/ext)
embedded_include_directories(PATH "${PLATFORM_DIR}/cmsis" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse_200_mps2/cmsis_core" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse_200_mps2/sse_200/retarget" ABSOLUTE)
embedded_include_directories(PATH "${PLATFORM_DIR}/target/sse_200_mps2/mps2/mps2_board" ABSOLUTE)


