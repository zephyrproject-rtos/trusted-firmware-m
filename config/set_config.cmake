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

# Load TF-M test suites setting
if (TEST_S OR
    TEST_NS OR
    TEST_NS_ATTESTATION OR
    TEST_NS_T_COSE OR
    TEST_NS_QCBOR OR
    TEST_NS_AUDIT OR
    TEST_NS_CORE OR
    TEST_NS_CRYPTO OR
    TEST_NS_ITS OR
    TEST_NS_PS OR
    TEST_NS_PLATFORM OR
    TEST_NS_FWU OR
    TEST_NS_IPC OR
    TEST_NS_SLIH_IRQ OR
    TEST_NS_FLIH_IRQ OR
    TEST_NS_MULTI_CORE OR
    TEST_S_ATTESTATION OR
    TEST_S_AUDIT OR
    TEST_S_CRYPTO OR
    TEST_S_ITS OR
    TEST_S_PS OR
    TEST_S_PLATFORM OR
    TEST_S_FWU OR
    TEST_S_IPC)

    # TFM_TEST is an internal cmake temporary value to manage tf-m-tests source
    set(TFM_TEST ON)
    include(${TFM_TEST_REPO_PATH}/test/config/set_config.cmake)
endif()
