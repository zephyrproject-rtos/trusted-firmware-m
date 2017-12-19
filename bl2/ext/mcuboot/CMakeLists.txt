#------------------------------------------------------------------------------
# Copyright (c) 2017, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)

#Tell cmake where our modules can be found
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../cmake)

#Include common stuff to control cmake.
include("Common/BuildSys")

#Start an embedded project.
embedded_project_start(CONFIG "${CMAKE_CURRENT_LIST_DIR}/../../../ConfigDefault.cmake")
project(mcuboot LANGUAGES ASM C)
embedded_project_fixup()

#Check input variables
if (NOT DEFINED BL2)
   message(FATAL ERROR "Incomplete build configuration: BL2 is undefined.")
elseif(NOT BL2)
    #If mcuboot is not need to be built then stop further processing.
    return()
endif()

if (NOT DEFINED MBEDTLS_DEBUG)
	message(FATAL_ERROR "Incomplete build configuration: MBEDTLS_DEBUG is undefined.")
endif()

if (NOT DEFINED MBEDTLS_C_FLAGS_BL2)
	message(FATAL_ERROR "Incomplete build configuration: MBEDTLS_C_FLAGS_BL2 is undefined.")
endif()

#Set variables to appropriate path
set(MCUBOOT_DIR ${CMAKE_CURRENT_LIST_DIR})
get_filename_component(TFM_ROOT_DIR "${MCUBOOT_DIR}/../../.." ABSOLUTE)

set(MPS2_SSE200_BUILD_CMSIS_CORE On)
set(MPS2_SSE200_BUILD_RETARGET On)
set(MPS2_SSE200_BUILD_NATIVE_DRIVERS On)
set(MPS2_SSE200_BUILD_STARTUP On)
set(MPS2_SSE200_BUILD_TARGET_CFG Off)
set(MPS2_SSE200_BUILD_TARGET_HARDWARE_KEYS Off)
set(MPS2_SSE200_BUILD_CMSIS_DRIVERS On)
set(MPS2_SSE200_BUILD_MPS2_TIME Off)
set(MPS2_SSE200_BUILD_UART_STDOUT On)
set(MPS2_SSE200_BUILD_MPS2_BOARD_LEDS Off)
set(MPS2_SSE200_BUILD_MPS2_BOARD_TIME On)
set(MPS2_SSE200_BUILD_MPS2_BOARD_FLASH On)
include(${TFM_ROOT_DIR}/platform/ext/Mps2SSE200.cmake)

#Append all our source files to global lists.
list(APPEND ALL_SRC_C "${MCUBOOT_DIR}/bl2_main.c"
		"${MCUBOOT_DIR}/flash_map.c"
		"${MCUBOOT_DIR}/hal_flash.c"
		"${MCUBOOT_DIR}/os.c"
		"${MCUBOOT_DIR}/keys.c"
		"${MCUBOOT_DIR}/bootutil/src/loader.c"
		"${MCUBOOT_DIR}/bootutil/src/bootutil_misc.c"
		"${MCUBOOT_DIR}/bootutil/src/image_validate.c"
		"${MCUBOOT_DIR}/bootutil/src/image_rsa.c"
		"${MCUBOOT_DIR}/bootutil/src/caps.c"
	)

#Define location of mbedtls source, build, and installation directory.
get_filename_component(MBEDTLS_SOURCE_DIR "${TFM_ROOT_DIR}/../mbedtls" ABSOLUTE)
if(NOT EXISTS ${MBEDTLS_SOURCE_DIR})
	message(FATAL_ERROR "Missing mbedtls. Please clone the mbedtls repo to directory \"${MBEDTLS_SOURCE_DIR}\".")
endif()
set (MBEDTLS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/mbedtls")
set (MBEDTLS_INSTALL_DIR ${MBEDTLS_BINARY_DIR}/mbedtls_install)

#Set build type for mbedtls libraries
if (MBEDTLS_DEBUG)
	set(MBEDTLS_BUILD_TYPE "Debug")
else()
	set(MBEDTLS_BUILD_TYPE "Release")
endif()

#Build mbedtls as external project.
#This ensures mbedtls is built with exactly defined settings.
#mbedtls will be used from is't install location
set(MBEDTLS_C_FLAGS ${MBEDTLS_C_FLAGS_BL2})
set(MBEDTLS_TARGET_NAME "mbedtls_mcuboot_lib")
include(${TFM_ROOT_DIR}/BuildMbedtls.cmake)

#Setting include directories
embedded_include_directories(PATH ${TFM_ROOT_DIR} ABSOLUTE APPEND)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/bl2/ext/mcuboot/include ABSOLUTE APPEND)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/bl2/ext/mcuboot/bootutil/include/ ABSOLUTE APPEND)
embedded_include_directories(PATH ${MBEDTLS_INSTALL_DIR}/include ABSOLUTE APPEND)

#Define linker file
embedded_set_target_linker_file(TARGET mcuboot PATH "${TFM_ROOT_DIR}/platform/ext/target/sse_200_mps2/sse_200/armclang/sse_200_bl2.sct")

add_executable(${PROJECT_NAME} ${MCUBOOT_SRC} ${ALL_SRC_ASM_BL2} ${ALL_SRC_C} ${ALL_SRC_CXX})

#Add BL2 define to linker to resolve symbols in region_defs.h
set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS " --predefine=\"-DBL2\"")

#Link mbedcrypto library to project
target_link_libraries(${PROJECT_NAME} "${MBEDTLS_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX_C}mbedcrypto${CMAKE_STATIC_LIBRARY_SUFFIX_C}")
add_dependencies(${PROJECT_NAME} ${MBEDTLS_TARGET_NAME}_install)


#Generate binary file from axf
compiler_generate_binary_output(${PROJECT_NAME})

#Set macro definitions for the project.
target_compile_definitions(${PROJECT_NAME} PRIVATE
							MCUBOOT_SIGN_RSA
							MCUBOOT_VALIDATE_SLOT0
							MCUBOOT_USE_FLASH_AREA_GET_SECTORS
							MBEDTLS_CONFIG_FILE="config-boot.h"
							MCUBOOT_TARGET_CONFIG="flash_layout.h"
						)

#Finally let cmake system apply changes after the whole project is defined.
embedded_project_end(${PROJECT_NAME})
