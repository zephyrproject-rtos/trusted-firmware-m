#-------------------------------------------------------------------------------
# Copyright (c) 2017, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file holds information of a specific build configuration of this project.
#Process this file only once.
#if(DEFINED EMBEDDED_PROJ_CONFIG_LOADED)
# 	return()
#endif()
#set(EMBEDDED_PROJ_CONFIG_LOADED True)

#Include board specific config (CPU, etc...)
include("Common/BoardSSE200")

#Use any ARMCLANG version found on PATH. Note: Only versions supported by the
#build system will work. A file cmake/Common/CompilerArmClangXY.cmake
#must be present with a matching version.
include("Common/FindArmClang")
include("Common/${ARMCLANG_MODULE}")

##These variables select how the projects are built. Each project will set
#various project specific settings (e.g. what files to build, macro
#definitions) based on these.
set (REGRESSION True)
set (CORE_TEST False)
set (BL2 True)

##Shared compiler and linker settings.
function(config_setting_shared_flags tgt)
	embedded_set_target_compile_flags(TARGET ${tgt} LANGUAGE C FLAGS -xc -std=c99 -fshort-enums -mfpu=none -fshort-wchar -funsigned-char -mcmse -Wall -Werror)
	embedded_set_target_link_flags(TARGET ${tgt} FLAGS --strict --map --symbols --xref --entry=Reset_Handler --info=summarysizes,sizes,totals,unused,veneers)
endfunction()

#Settings which shall be set for all projects the same way based
# on the variables above.
set (CORE_TEST_SERVICES False)
set (CORE_TEST_POSITIVE False)
set (CORE_TEST_INTERACTIVE False)
if (REGRESSION)
	set(SERVICES_TEST_ENABLED True)
else()
	set(SERVICES_TEST_ENABLED False)
endif()

if (SERVICES_TEST_ENABLED)
	set(SERVICE_TEST_S on)
	set(SERVICE_TEST_NS on)
	set(CORE_TEST_POSITIVE on)
	set(CORE_TEST_INTERACTIVE Off)
endif()

if (CORE_TEST)
	set(CORE_TEST_POSITIVE on)
	set(CORE_TEST_INTERACTIVE OFF)
endif()

if(CORE_TEST_INTERACTIVE)
	add_definitions(-DCORE_TEST_INTERACTIVE)
	set(TEST_FRAMEWORK_NS On)
	set(CORE_TEST_SERVICES On)
endif()

if(CORE_TEST_POSITIVE)
	add_definitions(-DCORE_TEST_POSITIVE)
	set(TEST_FRAMEWORK_NS On)
	set(CORE_TEST_SERVICES On)
endif()

if (SERVICE_TEST_S)
	add_definitions(-DSERVICES_TEST_S)
	set(TEST_FRAMEWORK_S On)
endif()

if (SERVICE_TEST_NS)
	add_definitions(-DSERVICES_TEST_NS)
	set(TEST_FRAMEWORK_NS On)
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
embedded_set_target_linker_file(TARGET tfm_s PATH "${CMAKE_CURRENT_LIST_DIR}/platform/ext/target/sse_200_mps2/sse_200/armclang/sse_200_s.sct")

#Non secure side
config_setting_shared_flags(tfm_ns)
embedded_set_target_linker_file(TARGET tfm_ns PATH  "${CMAKE_CURRENT_LIST_DIR}/platform/ext/target/sse_200_mps2/sse_200/armclang/sse_200_ns.sct")

##TF-M storage
config_setting_shared_flags(tfm_storage)
set(MBEDTLS_C_FLAGS "-D__ARM_FEATURE_CMSE=3 -D__thumb2__ -fshort-enums -mfpu=none -fshort-wchar -funsigned-char -mcmse  -DMBEDTLS_CONFIG_FILE=\\\\\\\"mbedtls_config.h\\\\\\\" -I${CMAKE_CURRENT_LIST_DIR}/platform/ext/target/common")

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
