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

if (NOT DEFINED ENABLE_SECURE_STORAGE_SERVICE_TESTS)
	message(FATAL_ERROR "Incomplete build configuration: ENABLE_SECURE_STORAGE_SERVICE_TESTS is undefined. ")
elseif (ENABLE_SECURE_STORAGE_SERVICE_TESTS)
	list(APPEND ALL_SRC_C_S "${SECURE_STORAGE_TEST_DIR}/secure/s_test_helpers.c"
			"${SECURE_STORAGE_TEST_DIR}/secure/sst_sec_interface_testsuite.c"
			"${SECURE_STORAGE_TEST_DIR}/secure/sst_reliability_testsuite.c"
		)
	if (SST_ROLLBACK_PROTECTION AND SST_ENCRYPTION AND TFM_LVL EQUAL 1)
		list(APPEND ALL_SRC_C_S "${SECURE_STORAGE_TEST_DIR}/secure/sst_rollback_protection_testsuite.c"
					"${SECURE_STORAGE_TEST_DIR}/secure/nv_counters/test_sst_nv_counters.c")
	endif()

	list(APPEND ALL_SRC_C_NS
			"${SECURE_STORAGE_TEST_DIR}/non_secure/ns_test_helpers.c"
			"${SECURE_STORAGE_TEST_DIR}/non_secure/sst_ns_interface_testsuite.c"
	)

	if (NOT DEFINED TFM_NS_CLIENT_IDENTIFICATION)
		message(FATAL_ERROR "Incomplete build configuration: TFM_NS_CLIENT_IDENTIFICATION is undefined.")
	elseif (TFM_NS_CLIENT_IDENTIFICATION)
		list(APPEND ALL_SRC_C_NS "${SECURE_STORAGE_TEST_DIR}/non_secure/sst_policy_testsuite.c")
		set_property(SOURCE ${ALL_SRC_C_NS} APPEND PROPERTY COMPILE_DEFINITIONS TFM_NS_CLIENT_IDENTIFICATION)
	endif()

	if (NOT DEFINED TFM_PARTITION_TEST_SST)
		message(FATAL_ERROR "Incomplete build configuration: TFM_PARTITION_TEST_SST is undefined.")
	elseif (TFM_PARTITION_TEST_SST AND TFM_NS_CLIENT_IDENTIFICATION)
		list(APPEND ALL_SRC_C_NS "${SECURE_STORAGE_TEST_DIR}/non_secure/sst_ns_ref_access_testsuite.c")
	endif()

	if (SST_ENABLE_PARTIAL_ASSET_RW)
		set_property(SOURCE ${ALL_SRC_C_S} APPEND PROPERTY COMPILE_DEFINITIONS SST_ENABLE_PARTIAL_ASSET_RW)
		set_property(SOURCE ${ALL_SRC_C_NS} APPEND PROPERTY COMPILE_DEFINITIONS SST_ENABLE_PARTIAL_ASSET_RW)
	endif()

	if (NOT SST_RAM_FS AND NOT REFERENCE_PLATFORM)
		# Show flash warning message only when the RAM FS is not in use or the tests are compiled to
		# be executed in the reference plaforms (AN519 and AN521). The reference platforms use RAM
		# memory to emulated a flash device as they do not have one.
		set_property(SOURCE ${ALL_SRC_C_S} APPEND PROPERTY COMPILE_DEFINITIONS SST_SHOW_FLASH_WARNING)
		set_property(SOURCE ${ALL_SRC_C_NS} APPEND PROPERTY COMPILE_DEFINITIONS SST_SHOW_FLASH_WARNING)
	endif()

	#Setting include directories
	embedded_include_directories(PATH ${TFM_ROOT_DIR} ABSOLUTE)
	embedded_include_directories(PATH ${TFM_ROOT_DIR}/test/interface/include ABSOLUTE)
	embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)
endif()
