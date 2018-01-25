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
embedded_set_target_linker_file(TARGET tfm_s PATH "${CMAKE_CURRENT_LIST_DIR}/platform/ext/target/mps2/an521/armclang/mps2_an521_s.sct")

##Non secure side
config_setting_shared_flags(tfm_ns)
embedded_set_target_linker_file(TARGET tfm_ns PATH  "${CMAKE_CURRENT_LIST_DIR}/platform/ext/target/mps2/an521/armclang/mps2_an521_ns.sct")

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
