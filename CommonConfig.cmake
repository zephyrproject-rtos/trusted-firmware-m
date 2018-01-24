#-------------------------------------------------------------------------------
# Copyright (c) 2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if(NOT DEFINED REGRESSION)
	message(FATAL_ERROR "ERROR: Incomplete Configuration: REGRESSION not defined, Include this file from a Config*.cmake")
elseif(NOT DEFINED CORE_TEST)
	message(FATAL_ERROR "ERROR: Incomplete Configuration: CORE_TEST not defined, Include this file from a Config*.cmake")
elseif(NOT DEFINED BL2)
	message(FATAL_ERROR "ERROR: Incomplete Configuration: BL2 not defined, Include this file from a Config*.cmake")
endif()

set(BUILD_CMSIS_CORE Off)
set(BUILD_RETARGET Off)
set(BUILD_NATIVE_DRIVERS Off)
set(BUILD_TIME Off)
set(BUILD_STARTUP Off)
set(BUILD_TARGET_CFG Off)
set(BUILD_TARGET_HARDWARE_KEYS Off)
set(BUILD_CMSIS_DRIVERS Off)
set(BUILD_UART_STDOUT Off)
set(BUILD_FLASH Off)
if(NOT DEFINED PLATFORM_CMAKE_FILE)
	message (FATAL_ERROR "Platform specific CMake is not defined. Please set PLATFORM_CMAKE_FILE.")
elseif(NOT EXISTS ${PLATFORM_CMAKE_FILE})
	message (FATAL_ERROR "Platform specific CMake \"${PLATFORM_CMAKE_FILE}\" file does not exist. Please fix value of PLATFORM_CMAKE_FILE.")
else()
	include(${PLATFORM_CMAKE_FILE})
endif()


#Use any ARMCLANG version found on PATH. Note: Only versions supported by the
#build system will work. A file cmake/Common/CompilerArmClangXY.cmake
#must be present with a matching version.
include("Common/FindArmClang")
include("Common/${ARMCLANG_MODULE}")

##Shared compiler and linker settings.
function(config_setting_shared_flags tgt)
	embedded_set_target_compile_flags(TARGET ${tgt} LANGUAGE C FLAGS -xc -std=c99 -fshort-enums -mfpu=none -fshort-wchar -funsigned-char -mcmse -Wall -Werror)
	embedded_set_target_link_flags(TARGET ${tgt} FLAGS --strict --map --symbols --xref --entry=Reset_Handler --info=summarysizes,sizes,totals,unused,veneers)
endfunction()

#Settings which shall be set for all projects the same way based
# on the variables above.
set (CORE_TEST_SERVICES OFF)
set (CORE_TEST_POSITIVE OFF)
set (CORE_TEST_INTERACTIVE OFF)

if (REGRESSION)
	set(SERVICES_TEST_ENABLED ON)
else()
	set(SERVICES_TEST_ENABLED OFF)
endif()

if (SERVICES_TEST_ENABLED)
	set(SERVICE_TEST_S ON)
	set(SERVICE_TEST_NS ON)
	set(CORE_TEST_POSITIVE ON)
	set(CORE_TEST_INTERACTIVE OFF)
endif()

if (CORE_TEST)
	set(CORE_TEST_POSITIVE ON)
	set(CORE_TEST_INTERACTIVE OFF)
endif()

if(CORE_TEST_INTERACTIVE)
	add_definitions(-DCORE_TEST_INTERACTIVE)
	set(TEST_FRAMEWORK_NS ON)
	set(CORE_TEST_SERVICES ON)
endif()

if(CORE_TEST_POSITIVE)
	add_definitions(-DCORE_TEST_POSITIVE)
	set(TEST_FRAMEWORK_NS ON)
	set(CORE_TEST_SERVICES ON)
endif()

if (SERVICE_TEST_S)
	add_definitions(-DSERVICES_TEST_S)
	set(TEST_FRAMEWORK_S ON)
endif()

if (SERVICE_TEST_NS)
	add_definitions(-DSERVICES_TEST_NS)
	set(TEST_FRAMEWORK_NS ON)
endif()

if(TEST_FRAMEWORK_S)
	add_definitions(-DTEST_FRAMEWORK_S)
endif()

if(TEST_FRAMEWORK_NS)
	add_definitions(-DTEST_FRAMEWORK_NS)
endif()

if (CORE_TEST_SERVICES)
	add_definitions(-DCORE_TEST_SERVICES)
endif()

if (BL2)
	add_definitions(-DBL2)
endif()

##Secure side
config_setting_shared_flags(tfm_s)

##Non secure side
config_setting_shared_flags(tfm_ns)

##TF-M storage
config_setting_shared_flags(tfm_storage)
set(MBEDTLS_C_FLAGS "-D__ARM_FEATURE_CMSE=3 -D__thumb2__ -fshort-enums -mfpu=none -fshort-wchar -funsigned-char -mcmse  -DMBEDTLS_CONFIG_FILE=\\\\\\\"mbedtls_config.h\\\\\\\" -I${CMAKE_CURRENT_LIST_DIR}/platform/ext/common")

set (SST_ENCRYPTION ON)
set (SST_RAM_FS ON)
set (SST_VALIDATE_METADATA_FROM_FLASH ON)
set (ENABLE_SECURE_STORAGE ON)
set (MBEDTLS_DEBUG ON)

##Tests
config_setting_shared_flags(tfm_secure_tests)
config_setting_shared_flags(tfm_non_secure_tests)

##BL2
config_setting_shared_flags(mcuboot)
set(MBEDTLS_C_FLAGS_BL2 "-D__ARM_FEATURE_CMSE=3 -D__thumb2__ -fshort-enums -mfpu=none -fshort-wchar -funsigned-char -mcmse  -DMBEDTLS_CONFIG_FILE=\\\\\\\"config-boot.h\\\\\\\" -I${CMAKE_CURRENT_LIST_DIR}/bl2/ext/mcuboot/include")
