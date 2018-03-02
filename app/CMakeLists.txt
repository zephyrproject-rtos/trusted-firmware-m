#-------------------------------------------------------------------------------
# Copyright (c) 2017-2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)

#Tell cmake where our modules can be found
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../cmake)

#Include common stuff to control cmake.
include("Common/BuildSys")

#Start an embedded project.
embedded_project_start(CONFIG "${CMAKE_CURRENT_LIST_DIR}/../ConfigDefault.cmake")
project(tfm_ns LANGUAGES ASM C)
embedded_project_fixup()

set(APP_DIR ${CMAKE_CURRENT_LIST_DIR})
get_filename_component(TFM_ROOT_DIR ${APP_DIR}/.. ABSOLUTE)
set(INTERFACE_DIR ${TFM_ROOT_DIR}/interface)

#Set variables
get_filename_component(CMSIS_5_DIR ${TFM_ROOT_DIR}/../CMSIS_5 ABSOLUTE)

if(NOT EXISTS ${CMSIS_5_DIR})
	message(FATAL_ERROR "Missing CMSIS_5. Please clone the CMSIS_5 repo to directory \"${CMSIS_5_DIR}\".")
endif()

set(NS_APP_SRC "${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Config/RTX_Config.c"
	"${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Source/rtx_lib.c"
	"${APP_DIR}/main_ns.c"
	"${APP_DIR}/ext/tz_context.c"
	"${APP_DIR}/tfm_integ_test.c"
	"${APP_DIR}/os_wrapper_rtx.c"
	"${INTERFACE_DIR}/src/tfm_sst_api.c"
	"${INTERFACE_DIR}/src/tfm_sst_svc_handler.c"
	"${INTERFACE_DIR}/src/tfm_id_mngr_dummy.c"
	"${INTERFACE_DIR}/src/tfm_ns_lock_rtx.c"
	)

set(BUILD_CMSIS_CORE On)
set(BUILD_RETARGET On)
set(BUILD_NATIVE_DRIVERS On)
set(BUILD_TIME Off)
set(BUILD_STARTUP On)
set(BUILD_TARGET_CFG Off)
set(BUILD_TARGET_HARDWARE_KEYS Off)
set(BUILD_CMSIS_DRIVERS On)
set(BUILD_UART_STDOUT Off)
set(BUILD_FLASH Off)
if(NOT DEFINED PLATFORM_CMAKE_FILE)
	message (FATAL_ERROR "Platform specific CMake is not defined. Please set PLATFORM_CMAKE_FILE.")
elseif(NOT EXISTS ${PLATFORM_CMAKE_FILE})
	message (FATAL_ERROR "Platform specific CMake \"${PLATFORM_CMAKE_FILE}\" file does not exist. Please fix value of PLATFORM_CMAKE_FILE.")
else()
	include(${PLATFORM_CMAKE_FILE})
endif()

if(NOT DEFINED NS_SCATTER_FILE_NAME)
	message(FATAL_ERROR "ERROR: Incomplete Configuration: NS_SCATTER_FILE_NAME not defined, Include this file from a Config*.cmake")
endif()
embedded_set_target_linker_file(TARGET tfm_ns PATH  "${NS_SCATTER_FILE_NAME}")

#Set include directories.
embedded_target_include_directories(TARGET ${PROJECT_NAME} PATH ${TFM_ROOT_DIR}/common/sct ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_NAME} PATH ${INTERFACE_DIR}/include ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_NAME} PATH ${TFM_ROOT_DIR} ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_NAME} PATH ${TFM_ROOT_DIR}/secure_fw/spm ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_NAME} PATH ${TFM_ROOT_DIR}/secure_fw/core ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_NAME} PATH ${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Include ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_NAME} PATH ${CMSIS_5_DIR}/CMSIS/RTOS2/Include ABSOLUTE APPEND)
embedded_target_include_directories(TARGET ${PROJECT_NAME} PATH ${CMSIS_5_DIR}/CMSIS/RTOS2/RTX/Config  ABSOLUTE APPEND)

#Create an executable
add_executable(${PROJECT_NAME} ${ALL_SRC_C} ${ALL_SRC_C_NS} ${ALL_SRC_ASM_NS} ${NS_APP_SRC})
#Add the RTX library
if(NOT DEFINED RTX_LIB_PATH)
	message(FATAL_ERROR "ERROR: Incomplete Configuration: RTX_LIB_PATH is not defined.")
endif()
target_link_libraries(${PROJECT_NAME} "${RTX_LIB_PATH}")
#Set macro definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE __thumb2__ __DOMAIN_NS=1 __ARM_FEATURE_CMSE=3 LOG_MSG_HANDLER_MODE_PRINTF_ENABLED)

#Generate binary file from axf
compiler_generate_binary_output(${PROJECT_NAME})

#Generate MCUBoot compatible payload
if (BL2)
	#Find Python3.x interpreter
	find_package(PythonInterp 3)

	if (NOT PYTHONINTERP_FOUND)
		message(FATAL_ERROR "Failed to find Python3.x interpreter. Pyhton3 must be installed an available on the PATH.")
	endif()

	set(MCUBOOT_DIR ${TFM_ROOT_DIR}/bl2/ext/mcuboot)

if(NOT DEFINED FLASH_LAYOUT)
	message(FATAL_ERROR "ERROR: Incomplete Configuration: FLASH_LAYOUT is not defined.")
endif()

	add_custom_command(TARGET ${PROJECT_NAME}
						POST_BUILD

						#Create concatenated binary image from tfm_ns.bin and tfm_s.bin
						COMMAND ${PYTHON_EXECUTABLE} ${MCUBOOT_DIR}/scripts/assemble.py
						ARGS -l ${FLASH_LAYOUT}
							 -s $<TARGET_FILE_DIR:tfm_s>/tfm_s.bin
							 -n $<TARGET_FILE_DIR:tfm_ns>/tfm_ns.bin
							 -o ${CMAKE_BINARY_DIR}/tfm_full.bin

						#Sign concatenated binary image with default public key in mcuboot folder
						COMMAND ${PYTHON_EXECUTABLE} ${MCUBOOT_DIR}/scripts/imgtool.py
						ARGS sign
							 -k ${MCUBOOT_DIR}/root-rsa-2048.pem
							 --align 1
							 -v 1.0
							 -H 0x400
							 --pad 0x100000
							 ${CMAKE_BINARY_DIR}/tfm_full.bin
							 ${CMAKE_BINARY_DIR}/tfm_sign.bin
				)
endif()

if (NOT DEFINED CORE_TEST)
	message(FATAL_ERROR "Incomplete build configuration: CORE_TEST is undefined. ")
elseif(CORE_TEST)
	target_compile_definitions(${PROJECT_NAME} PRIVATE CORE_TEST)
endif()

if (NOT DEFINED CORE_TEST_SERVICES)
	message(FATAL_ERROR "Incomplete build configuration: CORE_TEST_SERVICES is undefined. ")
elseif (CORE_TEST_SERVICES)
	set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS " --predefine=\"-DCORE_TEST_SERVICES\"")
endif()

if (NOT DEFINED SST_TEST_SERVICES)
	message(FATAL_ERROR "Incomplete build configuration: SST_TEST_SERVICES is undefined. ")
elseif (SST_TEST_SERVICES)
	set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS " --predefine=\"-DSST_TEST_SERVICES\"")
endif()

#Set BL2 specific settings.
if (NOT DEFINED BL2)
	message(FATAL_ERROR "Incomplete build configuration: BL2 is undefined. ")
elseif (BL2)
	set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS " --predefine=\"-DBL2\"")
endif()


if(NOT TARGET tfm_s)
	#We need access to the secure veneers. If the location is not already
	#specified, then set it.
	if (NOT DEFINED S_VENEER_FILE)
		set(S_VENEER_FILE "${CMAKE_CURRENT_BINARY_DIR}/s_veneers.o")
	endif()
	add_subdirectory(../secure_fw ${CMAKE_CURRENT_BINARY_DIR}/secure_fw)
endif()

#We depend on the non secure tests. See if the library target is available.
if(TARGET tfm_non_secure_tests)
	#If yes, then use the library.
	target_link_libraries(${PROJECT_NAME} tfm_non_secure_tests)
	#Ensure library is built first.
	#add_dependencies(${PROJECT_NAME} tfm_non_secure_tests)
else()
	#If not, add the test source to the build.
	#As of today since secufre_fw is built as a sub-project this code will never
	#execute.
	option(ENABLE_INVERT_SERVICE_TESTS "" TRUE)
	option(ENABLE_SECURE_STORAGE_SERVICE_TESTS "" TRUE)
	include(../test/CMakeLists.inc)
	target_sources(${PROJECT_NAME} PUBLIC ${ALL_SRC_C} ${ALL_SRC_C_NS})
endif()

#Ensure secure_fw is built before our executable.
add_dependencies(${PROJECT_NAME} tfm_s)

#Add the veneers to the executable.
set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_LIBRARIES ${S_VENEER_FILE})

#Finally let cmake system apply changes after the whole project is defined.
embedded_project_end(${PROJECT_NAME})
