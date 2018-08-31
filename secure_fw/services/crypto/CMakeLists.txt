#-------------------------------------------------------------------------------
# Copyright (c) 2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)

#Tell cmake where our modules can be found
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../cmake)

#Include common stuff to control cmake.
include("Common/BuildSys")

#Start an embedded project.
embedded_project_start(CONFIG "${CMAKE_CURRENT_LIST_DIR}/../../../ConfigDefault.cmake")
project(tfm_crypto LANGUAGES ASM C)
embedded_project_fixup()

###Some project global settings
set (CRYPTO_DIR "${CMAKE_CURRENT_LIST_DIR}")
get_filename_component(TFM_ROOT_DIR "${CRYPTO_DIR}/../../.." ABSOLUTE)
#Define location of mbed TLS source, build, and installation directory.
get_filename_component(MBEDTLS_SOURCE_DIR "${TFM_ROOT_DIR}/../mbedtls" ABSOLUTE)
set (MBEDTLS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/mbedtls")
set (MBEDTLS_INSTALL_DIR ${MBEDTLS_BINARY_DIR}/mbedtls_install)
set (MBEDTLS_TARGET_NAME "mbedtls_crypto_lib")

#Set mbed TLS compiler flags
set(MBEDTLS_C_FLAGS ${MBEDTLS_C_FLAGS_SERVICES})

###Get the definition of what files we need to build
set (ENABLE_CRYPTO ON)
include(CMakeLists.inc)

###Configure how we build our target
if(DEFINED CORE_TEST)
  set (TFM_LVL 3)
else()
  set (TFM_LVL 1)
endif()

#Set build type for mbed TLS libraries
if (MBEDTLS_DEBUG)
  set(MBEDTLS_BUILD_TYPE "Debug")
else()
  set(MBEDTLS_BUILD_TYPE "Release")
endif()

#Set preinclude header options for mbed TLS
set(MBEDTLS_PREINCLUDE_PREFIX __tfm_crypto__)
set(MBEDTLS_PREINCLUDE_HEADER ${CRYPTO_DIR}/mbedtls_global_symbols.h)

#Build mbed TLS as external project.
#This ensures mbed TLS is built with exactly defined settings.
#mbed TLS will be used from its install location
include(${TFM_ROOT_DIR}/BuildMbedtls.cmake)

#Specify what we build (for the crypto service, build as a static library)
add_library(tfm_crypto STATIC ${ALL_SRC_ASM} ${ALL_SRC_C})
embedded_set_target_compile_defines(TARGET tfm_crypto LANGUAGE C DEFINES __ARM_FEATURE_CMSE=3 __thumb2__ TFM_LVL=${TFM_LVL} MBEDTLS_CONFIG_FILE="platform/ext/common/tfm_mbedtls_config.h")
#Add a dependency on the mbed_tls_lib_install target.
add_dependencies(tfm_crypto ${MBEDTLS_TARGET_NAME}_install)
#Ask the compiler to merge the mbed TLS and the secure storage libraries.
compiler_merge_library(DEST tfm_crypto LIBS "${MBEDTLS_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX_C}mbedcrypto${CMAKE_STATIC_LIBRARY_SUFFIX_C}")

#Set common compiler and linker flags
config_setting_shared_compiler_flags(tfm_crypto)
config_setting_shared_linker_flags(tfm_crypto)

embedded_project_end(tfm_crypto)
