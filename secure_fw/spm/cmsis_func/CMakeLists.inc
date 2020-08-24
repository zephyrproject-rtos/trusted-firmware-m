#-------------------------------------------------------------------------------
# Copyright (c) 2017-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file assumes it will be included from a project specific cmakefile.
#
#Inputs:
#	TFM_ROOT_DIR - directory where secure FW sourec is located.
#
#Outputs:
#	Will modify include directories to make the source compile.
#	ALL_SRC_C: C source files to be compiled will be added to this list.
#	  This shall be added to your add_executable or add_library command.
#	ALL_SRC_CXX: C++ source files to be compiled will be added to this list.
#	  This shall be added to your add_executable or add_library command.
#	ALL_SRC_ASM: assembly source files to be compiled will be added to this
#	  list. This shall be added to your add_executable or add_library
#	  command.
#	Include directories will be modified by using the include_directories()
#	  commands as needed.

if(NOT DEFINED TFM_ROOT_DIR)
	message(FATAL_ERROR
		"Please set TFM_ROOT_DIR before including this file.")
endif()

#Get the current directory where this file is located.
set(SFW_FUNC_SPM_DIR ${CMAKE_CURRENT_LIST_DIR})
set(SFW_SPM_DIR "${SFW_FUNC_SPM_DIR}/..")
set(SFW_SPM_COMMON_DIR "${SFW_SPM_DIR}/common")

set (SFW_FUNC_SPM_SRC
		"${SFW_SPM_COMMON_DIR}/tfm_boot_data.c"
		"${SFW_SPM_COMMON_DIR}/tfm_core_mem_check.c"
		"${SFW_SPM_COMMON_DIR}/tfm_core_utils.c"
		"${SFW_SPM_COMMON_DIR}/utilities.c"
		"${SFW_FUNC_SPM_DIR}/main.c"
		"${SFW_FUNC_SPM_DIR}/tfm_core_svcalls_func.c"
		"${SFW_FUNC_SPM_DIR}/tfm_secure_api.c"
		"${SFW_FUNC_SPM_DIR}/spm_func.c"
		"${SFW_FUNC_SPM_DIR}/tfm_nspm_func.c"
		"${SFW_FUNC_SPM_DIR}/tfm_veneers.c"
		"${SFW_FUNC_SPM_DIR}/arch.c"
	)

#Append all our source files to global lists.
list(APPEND ALL_SRC_C ${SFW_FUNC_SPM_SRC})
unset(SFW_FUNC_SPM_SRC)

#Setting include directories
embedded_include_directories(PATH ${SFW_SPM_DIR} ABSOLUTE)
embedded_include_directories(PATH ${SFW_FUNC_SPM_DIR} ABSOLUTE)
embedded_include_directories(PATH ${SFW_FUNC_SPM_DIR}/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/spm/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/bl2/include ABSOLUTE)
