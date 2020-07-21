#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.13)

project("Trusted Firmware M" VERSION 1.1.0 LANGUAGES C ASM)
set(TFM_VERSION ${PROJECT_VERSION})

# Some compiler flags depend on the CPU / platform config. This include should
# be run before anything else so the compiler can be configured properly.
include(platform/ext/target/${TFM_PLATFORM}/preload.cmake)

# The default build type is release. If debug symbols are needed then
# -DCMAKE_BUILD_TYPE=debug should be used (likewise with other build types)
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type: [Debug, Release, RelWithDebInfo, MinSizeRel]" FORCE)
endif()

############################ CONFIGURATION #####################################

# First load defaults.
include(config/config_default.cmake)

# Then load the build type config, overriding defaults and command line.
string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWERCASE)
if (EXISTS ${CMAKE_SOURCE_DIR}/config/build_type/${CMAKE_BUILD_TYPE_LOWERCASE}.cmake)
    include(${CMAKE_SOURCE_DIR}/config/build_type/${CMAKE_BUILD_TYPE_LOWERCASE}.cmake)
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

# Optionally load extra config, overriding platform options, overriding profile,
# build type config, defaults and command line.
if (TFM_EXTRA_CONFIG_PATH)
    include(${TFM_EXTRA_CONFIG_PATH})
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

################################################################################

add_subdirectory(lib/ext)
add_subdirectory(tools)
add_subdirectory(docs)
if(NS)
    # Set to ${TFM_TEST_REPO_PATH}/app by default
    add_subdirectory(${TFM_APP_PATH} ${CMAKE_CURRENT_BINARY_DIR}/app)
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
