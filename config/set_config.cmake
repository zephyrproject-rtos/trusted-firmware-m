#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# The Configuration sequence is captured in the documentation, in
# docs/getting_started/tfm_build_instructions.rst under Cmake Configuration. If
# the sequence is updated here the docs must also be updated.

# Load extra config
if (TFM_EXTRA_CONFIG_PATH)
    include(${TFM_EXTRA_CONFIG_PATH})
endif()

# Load PSA config, setting options not already set
if (TEST_PSA_API)
    include(config/tests/config_test_psa_api.cmake)
endif()

# Load build type config, setting options not already set
string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWERCASE)
if (EXISTS ${CMAKE_SOURCE_DIR}/config/build_type/${CMAKE_BUILD_TYPE_LOWERCASE}.cmake)
    include(${CMAKE_SOURCE_DIR}/config/build_type/${CMAKE_BUILD_TYPE_LOWERCASE}.cmake)
endif()

# Load platform config, setting options not already set
if (EXISTS ${CMAKE_SOURCE_DIR}/platform/ext/target/${TFM_PLATFORM}/config.cmake)
    include(platform/ext/target/${TFM_PLATFORM}/config.cmake)
endif()

# Load accelerator config, setting options not already set
if (CRYPTO_HW_ACCELERATOR)
    if (EXISTS ${CMAKE_SOURCE_DIR}/platform/ext/accelerator/${CRYPTO_HW_ACCELERATOR_TYPE}/config.cmake)
        include(${CMAKE_SOURCE_DIR}/platform/ext/accelerator/${CRYPTO_HW_ACCELERATOR_TYPE}/config.cmake)
    endif()
endif()

# Load profile config, setting options not already set
if (TFM_PROFILE)
    include(config/profile/${TFM_PROFILE}.cmake)
endif()

# Load TF-M model specific default config
if (TFM_PSA_API)
    include(config/tfm_ipc_config_default.cmake)
endif()

# Load defaults, setting options not already set
include(config/config_default.cmake)

# Load TF-M regression test suites setting

get_cmake_property(CACHE_VARS CACHE_VARIABLES)
# By default all non-secure regression tests are disabled.
# If TEST_NS or TEST_NS_XXX flag is passed via command line and set to ON,
# selected corresponding features to support non-secure regression tests.
foreach(CACHE_VAR ${CACHE_VARS})
    string(REGEX MATCH "^TEST_NS.*" _NS_TEST_FOUND "${CACHE_VAR}")
    if (_NS_TEST_FOUND AND "${${CACHE_VAR}}")
        # TFM_NS_REG_TEST is a TF-M internal cmake flag to manage building
        # tf-m-tests non-secure regression tests related source
        set(TFM_NS_REG_TEST ON)
        break()
    endif()
endforeach()

# By default all secure regression tests are disabled.
# If TEST_S or TEST_S_XXX flag is passed via command line and set to ON,
# selected corresponding features to support secure regression tests.
foreach(CACHE_VAR ${CACHE_VARS})
    string(REGEX MATCH "^TEST_S.*" _S_TEST_FOUND "${CACHE_VAR}")
    if (_S_TEST_FOUND AND "${${CACHE_VAR}}")
        # TFM_S_REG_TEST is a TF-M internal cmake flag to manage building
        # tf-m-tests secure regression tests related source
        set(TFM_S_REG_TEST ON)
        break()
    endif()
endforeach()

if (TFM_NS_REG_TEST OR TFM_S_REG_TEST)
    include(config/tests/set_config.cmake)
endif()
