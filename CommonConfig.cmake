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
elseif(NOT DEFINED TFM_LVL)
	message(FATAL_ERROR "ERROR: Incomplete Configuration: TFM_LVL not defined, Include this file from a Config*.cmake")
endif()

if(NOT DEFINED COMPILER)
	message(FATAL_ERROR "ERROR: COMPILER is not set in command line")
elseif((NOT ${COMPILER} STREQUAL "ARMCLANG") AND (NOT ${COMPILER} STREQUAL "GNUARM"))
	message(FATAL_ERROR "ERROR: Compiler \"${COMPILER}\" is not supported.")
endif()

set(BUILD_CMSIS_CORE Off)
set(BUILD_RETARGET Off)
set(BUILD_NATIVE_DRIVERS Off)
set(BUILD_TIME Off)
set(BUILD_STARTUP Off)
set(BUILD_TARGET_CFG Off)
set(BUILD_TARGET_HARDWARE_KEYS Off)
set(BUILD_TARGET_NV_COUNTERS Off)
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

if (NOT DEFINED IMAGE_VERSION)
    set(IMAGE_VERSION 0.0.0+0)
endif()

if(${COMPILER} STREQUAL "ARMCLANG")
	#Use any ARMCLANG version found on PATH. Note: Only versions supported by the
	#build system will work. A file cmake/Common/CompilerArmClangXY.cmake
	#must be present with a matching version.
	include("Common/FindArmClang")
	include("Common/${ARMCLANG_MODULE}")

	set (COMMON_COMPILE_FLAGS -fshort-enums -fshort-wchar -funsigned-char -mfpu=none -mcmse)
	##Shared compiler settings.
	function(config_setting_shared_compiler_flags tgt)
		embedded_set_target_compile_flags(TARGET ${tgt} LANGUAGE C FLAGS -xc -std=c99 ${COMMON_COMPILE_FLAGS} -Wall -Werror)
	endfunction()

	##Shared linker settings.
	function(config_setting_shared_linker_flags tgt)
		embedded_set_target_link_flags(TARGET ${tgt} FLAGS --strict --map --symbols --xref --entry=Reset_Handler --info=summarysizes,sizes,totals,unused,veneers)
	endfunction()
elseif(${COMPILER} STREQUAL "GNUARM")
	#Use any GNUARM version found on PATH. Note: Only versions supported by the
	#build system will work. A file cmake/Common/CompilerGNUARMXY.cmake
	#must be present with a matching version.
	include("Common/FindGNUARM")
	include("Common/${GNUARM_MODULE}")

	set (COMMON_COMPILE_FLAGS -fshort-enums -fshort-wchar -funsigned-char -msoft-float -mcmse  --specs=nano.specs)
	##Shared compiler and linker settings.
	function(config_setting_shared_compiler_flags tgt)
		embedded_set_target_compile_flags(TARGET ${tgt} LANGUAGE C FLAGS -xc -std=c99 ${COMMON_COMPILE_FLAGS} -Wall -Werror -Wno-format -Wno-return-type -Wno-unused-but-set-variable)
	endfunction()

	##Shared linker settings.
	function(config_setting_shared_linker_flags tgt)
		#--no-wchar-size-warning flag is added because TF-M sources are compiled
		#with short wchars, however the standard library is compiled with normal
		#wchar, and this generates linker time warnings. TF-M code does not use
		#wchar, so the warning can be suppressed.
		embedded_set_target_link_flags(TARGET ${tgt} FLAGS -Xlinker -check-sections -Xlinker -fatal-warnings --entry=Reset_Handler -Wl,--no-wchar-size-warning --specs=nano.specs)
	endfunction()
endif()

#Create a string from the compile flags list, so that it can be used later
#in this file to set mbedtls and BL2 flags
list_to_string(COMMON_COMPILE_FLAGS_STR ${COMMON_COMPILE_FLAGS})

#Settings which shall be set for all projects the same way based
# on the variables above.
set (TFM_PARTITION_TEST_CORE OFF)
set (CORE_TEST_POSITIVE OFF)
set (CORE_TEST_INTERACTIVE OFF)
set (TFM_PARTITION_TEST_SST OFF)
set (TEST_FRAMEWORK_S OFF)
set (REFERENCE_PLATFORM OFF)
set (TFM_PARTITION_TEST_SECURE_SERVICES OFF)
set (SERVICES_TEST_ENABLED OFF)

if(${TARGET_PLATFORM} STREQUAL "AN521" OR ${TARGET_PLATFORM} STREQUAL "AN519")
	set (REFERENCE_PLATFORM ON)
elseif(${TARGET_PLATFORM} STREQUAL "MUSCA_A")
	add_definitions(-DTARGET_MUSCA_A)
endif()

# Option to demonstrate usage of secure-only peripheral
set (SECURE_UART1 OFF)

if (REGRESSION)
	set(SERVICES_TEST_ENABLED ON)
endif()

if (SERVICES_TEST_ENABLED)
	set(SERVICE_TEST_S ON)
	set(SERVICE_TEST_NS ON)
	if (REFERENCE_PLATFORM)
		set(CORE_TEST_POSITIVE ON)
	endif()
endif()

if (CORE_TEST)
	set(CORE_TEST_POSITIVE ON)
	set(CORE_TEST_INTERACTIVE OFF)
endif()

if (CORE_TEST_INTERACTIVE)
	add_definitions(-DCORE_TEST_INTERACTIVE)
	set(TEST_FRAMEWORK_NS ON)
	set(TFM_PARTITION_TEST_CORE ON)
endif()

if (CORE_TEST_POSITIVE)
	add_definitions(-DCORE_TEST_POSITIVE)
	set(TEST_FRAMEWORK_NS ON)
	set(TFM_PARTITION_TEST_CORE ON)
endif()

if (SERVICE_TEST_S)
	add_definitions(-DSERVICES_TEST_S)
	add_definitions(-DTFM_PARTITION_TEST_SECURE_SERVICES)
	set(TEST_FRAMEWORK_S ON)
	set(TFM_PARTITION_TEST_SECURE_SERVICES ON)
	set(TFM_PARTITION_TEST_SST ON)
endif()

if (SERVICE_TEST_NS)
	add_definitions(-DSERVICES_TEST_NS)
	set(TEST_FRAMEWORK_NS ON)
endif()

if (TEST_FRAMEWORK_S)
	add_definitions(-DTEST_FRAMEWORK_S)
endif()

if (TEST_FRAMEWORK_NS)
	add_definitions(-DTEST_FRAMEWORK_NS)
endif()

if (TFM_PARTITION_TEST_CORE)
	add_definitions(-DTFM_PARTITION_TEST_CORE)
endif()

if (TFM_PARTITION_TEST_SST)
	add_definitions(-DTFM_PARTITION_TEST_SST)
endif()

if (BL2)
	add_definitions(-DBL2)
	if (MCUBOOT_NO_SWAP)
		set(LINK_TO_BOTH_MEMORY_REGION ON)
	endif()
	if (MCUBOOT_NO_SWAP AND MCUBOOT_RAM_LOADING)
		message (FATAL_ERROR "Bootloader: MCUBOOT_RAM_LOADING and MCUBOOT_NO_SWAP are not supported together")
	endif()
else()
	if (MCUBOOT_NO_SWAP)
		message (FATAL_ERROR "Bootloader build is turned off, not possible to specify bootloader behavior")
	endif()
endif()

##Set mbedTLS compiler flags and variables for secure storage and audit log
set(MBEDTLS_C_FLAGS_SST_LOG "-D__ARM_FEATURE_CMSE=3 -D__thumb2__ ${COMMON_COMPILE_FLAGS_STR} -DMBEDTLS_CONFIG_FILE=\\\\\\\"tfm_mbedtls_config.h\\\\\\\" -I${CMAKE_CURRENT_LIST_DIR}/platform/ext/common")

#Default TF-M secure storage flags.
#These flags values can be overwritten by setting them in platform/ext/<TARGET_NAME>.cmake
#Documentation about these flags can be found in docs/user_guides/services/tfm_sst_integration_guide.md
if (NOT DEFINED ENABLE_SECURE_STORAGE)
	set (ENABLE_SECURE_STORAGE ON)

	if (NOT DEFINED SST_ENCRYPTION)
		set (SST_ENCRYPTION ON)
	endif()

	if (NOT DEFINED SST_ROLLBACK_PROTECTION)
		set (SST_ROLLBACK_PROTECTION OFF)
	endif()

	if (NOT DEFINED SST_CREATE_FLASH_LAYOUT)
		set (SST_CREATE_FLASH_LAYOUT OFF)
	endif()

	if (NOT DEFINED SST_VALIDATE_METADATA_FROM_FLASH)
		set (SST_VALIDATE_METADATA_FROM_FLASH ON)
	endif()

	if (NOT DEFINED SST_ENABLE_PARTIAL_ASSET_RW)
		set (SST_ENABLE_PARTIAL_ASSET_RW ON)
	endif()

	if (NOT DEFINED SST_RAM_FS)
		if (REGRESSION)
			set (SST_RAM_FS ON)
		else()
			set (SST_RAM_FS OFF)
		endif()
	endif()
endif()

if (NOT DEFINED MBEDTLS_DEBUG)
    if (${COMPILER} STREQUAL "GNUARM" AND ${TARGET_PLATFORM} STREQUAL "MUSCA_A" AND BL2)
        #The size of the MCUboot binary compiled with GCC exceeds the size limit on
        #Musca A. By turning off the mbed TLS debug build is a good way to go below
        #that limit, while it is still possible to debug TFM/bootloader code.
        set (MBEDTLS_DEBUG OFF)
    else ()
        set (MBEDTLS_DEBUG ON)
    endif ()
endif()

##Set mbedTLS compiler flags for BL2 bootloader
set(MBEDTLS_C_FLAGS_BL2 "-D__ARM_FEATURE_CMSE=3 -D__thumb2__ ${COMMON_COMPILE_FLAGS_STR} -DMBEDTLS_CONFIG_FILE=\\\\\\\"config-boot.h\\\\\\\" -I${CMAKE_CURRENT_LIST_DIR}/bl2/ext/mcuboot/include")
