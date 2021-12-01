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
if (TFM_LIB_MODEL)
    include(config/tfm_library_config_default.cmake)
else()
    include(config/tfm_ipc_config_default.cmake)
endif()

# Load MCUboot specific default.cmake
# Set BL2 to ON by default, OFF if the platform specifically defines this property
set(BL2 ON CACHE BOOL "Whether to build BL2")
if (BL2)
    include(${CMAKE_SOURCE_DIR}/bl2/ext/mcuboot/mcuboot_default_config.cmake)
endif()

# Include coprocessor configs
include(config/cp_config_default.cmake)

# Load defaults, setting options not already set
include(config/config_default.cmake)

# Fetch tf-m-tests repo during config, if NS or regression test is required.
# Therefore tf-m-tests configs can be set with TF-M configs since their configs
# are coupled.
include(lib/ext/tf-m-tests/tf-m-tests.cmake)

# Load TF-M regression test suites setting
if (TFM_NS_REG_TEST OR TFM_S_REG_TEST)
    include(${TFM_TEST_PATH}/config/set_config.cmake)
endif()

# Set secure log configs
# It also depends on regression test config.
include(config/tfm_secure_log.cmake)
