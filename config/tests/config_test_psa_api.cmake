#------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#------------------------------------------------------------------------------

############ Override defaults for PSA API tests ##############################

# When building for the PSA Crypto API tests, ensure the ITS max asset size is
# set to at least the size of the largest asset created by the tests
if (("${TEST_PSA_API}" STREQUAL "CRYPTO") AND NOT (ITS_MAX_ASSET_SIZE GREATER 1229))
    set(ITS_MAX_ASSET_SIZE                  "1229"      CACHE STRING    "The maximum asset size to be stored in the Internal Trusted Storage area")
endif()

if ("${TEST_PSA_API}" STREQUAL "IPC")
    # PSA Arch test partitions only support IPC model so far
    set(CONFIG_TFM_SPM_BACKEND      "IPC"       CACHE STRING    "The SPM backend [IPC, SFN]")
    set(TFM_PARTITION_FF_TEST   ON)
else()
    set(TFM_PARTITION_FF_TEST   OFF)
endif()

if ("${TEST_PSA_API}" STREQUAL "INITIAL_ATTESTATION")
    set(TFM_PARTITION_INITIAL_ATTESTATION      ON       CACHE BOOL      "Enable Initial Attestation partition")
    set(TFM_PARTITION_CRYPTO                   ON       CACHE BOOL      "Enable Crypto partition")
    set(TFM_PARTITION_INTERNAL_TRUSTED_STORAGE ON       CACHE BOOL      "Enable Internal Trusted Storage partition")
    set(TFM_PARTITION_PLATFORM                 ON       CACHE BOOL      "Enable Platform partition")
endif()

if ("${TEST_PSA_API}" STREQUAL "CRYPTO")
    set(TFM_PARTITION_CRYPTO                   ON       CACHE BOOL      "Enable Crypto partition")
    set(TFM_PARTITION_INTERNAL_TRUSTED_STORAGE ON       CACHE BOOL      "Enable Internal Trusted Storage partition")
    set(TFM_PARTITION_PLATFORM                 ON       CACHE BOOL      "Enable Platform partition")
endif()

if ("${TEST_PSA_API}" STREQUAL "STORAGE")
    set(TFM_PARTITION_INTERNAL_TRUSTED_STORAGE ON       CACHE BOOL      "Enable Internal Trusted Storage partition")
    set(TFM_PARTITION_PROTECTED_STORAGE        ON       CACHE BOOL      "Enable Protected Storage partition")
    set(TFM_PARTITION_CRYPTO                   ON       CACHE BOOL      "Enable Crypto partition")
    set(TFM_PARTITION_PLATFORM                 ON       CACHE BOOL      "Enable Platform partition")
endif()

set(PROJECT_CONFIG_HEADER_FILE  "${CMAKE_SOURCE_DIR}/config/tests/config_test_psa_api.h" CACHE FILEPATH "User defined header file for TF-M config")
