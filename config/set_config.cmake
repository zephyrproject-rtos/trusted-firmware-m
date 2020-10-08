#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# The Configuration sequence is captured in the documentation, in
# docs/getting_started/tfm_build_instructions.rst under Cmake Configuration. If
# the sequence is updated here the docs must also be updated.

# First load defaults.
include(${CMAKE_SOURCE_DIR}/config/config_default.cmake)

# Then load the build type config, overriding defaults and command line.
string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWERCASE)
if (EXISTS ${CMAKE_SOURCE_DIR}/config/build_type/${CMAKE_BUILD_TYPE_LOWERCASE}.cmake)
    include(${CMAKE_SOURCE_DIR}/config/build_type/${CMAKE_BUILD_TYPE_LOWERCASE}.cmake)
endif()

# If configured for tests, load config specific to tests overriding defaults.
if (TEST_PSA_API)
    include(${CMAKE_SOURCE_DIR}/config/tests/config_test_psa_api.cmake)
endif()

# Then load the profile, overriding build type config, defaults and command
# line.
if (TFM_PROFILE)
    include(${CMAKE_SOURCE_DIR}/config/profile/${TFM_PROFILE}.cmake)
endif()

# Then load the platform options, overriding profile, build type config,
# defaults and command line.
if (EXISTS ${CMAKE_SOURCE_DIR}/platform/ext/target/${TFM_PLATFORM}/config.cmake)
    include(${CMAKE_SOURCE_DIR}/platform/ext/target/${TFM_PLATFORM}/config.cmake)
endif()

# If CRYPTO_HW_ACCELERATOR is enabled by the platform, then load the
# corresponding config if it exists
if (CRYPTO_HW_ACCELERATOR AND EXISTS ${CMAKE_SOURCE_DIR}/platform/ext/accelerator/${CRYPTO_HW_ACCELERATOR_TYPE}/config.cmake)
    include(${CMAKE_SOURCE_DIR}/platform/ext/accelerator/${CRYPTO_HW_ACCELERATOR_TYPE}/config.cmake)
endif()

# Optionally load extra config, overriding platform options, overriding profile,
# build type config, defaults and command line.
if (TFM_EXTRA_CONFIG_PATH)
    include(${TFM_EXTRA_CONFIG_PATH})
endif()

include(${CMAKE_SOURCE_DIR}/config/check_config.cmake)
