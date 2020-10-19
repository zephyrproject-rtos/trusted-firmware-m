#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)

set(TFM_VERSION 1.1.0)

############################ CONFIGURATION #####################################

# First load defaults.
include(config/config_default.cmake)

# Then load the build type config, overriding defaults and command line.
string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWERCASE)
if (EXISTS ${CMAKE_SOURCE_DIR}/config/build_type/${CMAKE_BUILD_TYPE_LOWERCASE}.cmake)
    include(${CMAKE_SOURCE_DIR}/config/build_type/${CMAKE_BUILD_TYPE_LOWERCASE}.cmake)
endif()

# If configured for tests, load config specific to tests overriding defaults.
if (TEST_PSA_API)
    include(config/tests/config_test_psa_api.cmake)
endif()

# Then load the profile, overriding build type config, defaults and command
# line.
if (TFM_PROFILE)
    include(config/profile/${TFM_PROFILE}.cmake)
endif()

# Then load the platform options, overriding profile, build type config,
# defaults and command line.
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/platform/ext/target/${TFM_PLATFORM}/config.cmake)
    include(platform/ext/target/${TFM_PLATFORM}/config.cmake)
endif()

# If CRYPTO_HW_ACCELERATOR is enabled by the platform, then load the
# corresponding config if it exists
if (CRYPTO_HW_ACCELERATOR)
    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/platform/ext/accelerator/${CRYPTO_HW_ACCELERATOR_TYPE}/config.cmake)
        include(platform/ext/accelerator/${CRYPTO_HW_ACCELERATOR_TYPE}/config.cmake)
    endif()
endif()

# Optionally load extra config, overriding platform options, overriding profile,
# build type config, defaults and command line.
if (TFM_EXTRA_CONFIG_PATH)
    include(${TFM_EXTRA_CONFIG_PATH})
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(NOT ${CMAKE_GENERATOR} STREQUAL "Unix Makefiles" AND
   NOT ${CMAKE_GENERATOR} STREQUAL "Ninja")
    Message(FATAL_ERROR "Unsupported generator ${CMAKE_GENERATOR}. Hint: Try -G\"Unix Makefiles\"")
endif()

# The default build type is release. If debug symbols are needed then
# -DCMAKE_BUILD_TYPE=debug should be used (likewise with other build types)
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type: [Debug, Release, RelWithDebInfo, MinSizeRel]" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

############################### Compiler configuration #########################

# Some compiler flags depend on the CPU / platform config. This include should
# be run before the toolchain file so the compiler can be configured properly.
if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/platform/ext/target/${TFM_PLATFORM}/preload.cmake)
    Message(FATAL_ERROR "Unsupported TFM_PLATFORM ${TFM_PLATFORM}")
else()
    include(platform/ext/target/${TFM_PLATFORM}/preload.cmake)
endif()

#Legacy compat option - load CMAKE_TOOLCHAIN_FILE as a TFM_TOOLCHAIN_FILE
if (CMAKE_TOOLCHAIN_FILE)
    message(DEPRECATION "SETTING CMAKE_TOOLCHAIN_FILE is deprecated. It has been replaced with TFM_TOOLCHAIN_FILE.")
    message(DEPRECATION "INTERPRETING -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} as -DTFM_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
    set(TFM_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE})
    unset(CMAKE_TOOLCHAIN_FILE)
endif()

include(${TFM_TOOLCHAIN_FILE})
set(CMAKE_PROJECT_INCLUDE_BEFORE ${CMAKE_SOURCE_DIR}/cmake/disable_compiler_detection.cmake)

project("Trusted Firmware M" VERSION ${TFM_VERSION} LANGUAGES C ASM)
tfm_toolchain_reload_compiler()

################################################################################

add_subdirectory(lib/ext)
add_subdirectory(tools)
add_subdirectory(docs)
if(NS)
    # Set to ${TFM_TEST_REPO_PATH}/app by default
    add_subdirectory(${TFM_APP_PATH} ${CMAKE_CURRENT_BINARY_DIR}/app)
    add_subdirectory(${TFM_NS_LOG_PATH} ${CMAKE_CURRENT_BINARY_DIR}/ns_log)
endif()
add_subdirectory(secure_fw)
add_subdirectory(interface)
if(BL2)
    add_subdirectory(bl2)
endif()
add_subdirectory(platform)

if(NS AND (TEST_S OR TEST_NS))
    # Set to ${TFM_TEST_REPO_PATH}/test by default
    add_subdirectory(${TFM_TEST_PATH} ${CMAKE_CURRENT_BINARY_DIR}/test)
endif()

include(cmake/install.cmake)

if(CRYPTO_HW_ACCELERATOR)
    add_subdirectory(platform/ext/accelerator)
endif()
