#-------------------------------------------------------------------------------
# Copyright (c) 2019-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# This file holds information of the configuration for TF-M Profile Medium

if(NOT DEFINED TFM_ROOT_DIR)
    message(FATAL_ERROR "Variable TFM_ROOT_DIR is not set!")
endif()

#Include board specific config (CPU, etc...), select platform specific build
#system settings file
if(NOT DEFINED TARGET_PLATFORM)
    message(FATAL_ERROR "ERROR: TARGET_PLATFORM is not set in command line")
elseif(${TARGET_PLATFORM} STREQUAL "AN521")
    set(PLATFORM_CMAKE_FILE "${TFM_ROOT_DIR}/platform/ext/Mps2AN521.cmake")
elseif(${TARGET_PLATFORM} STREQUAL "AN519")
    set (PLATFORM_CMAKE_FILE "${TFM_ROOT_DIR}/platform/ext/Mps2AN519.cmake")
elseif(${TARGET_PLATFORM} STREQUAL "MUSCA_B1")
    set(PLATFORM_CMAKE_FILE "${TFM_ROOT_DIR}/platform/ext/musca_b1.cmake")
elseif(${TARGET_PLATFORM} STREQUAL "psoc64")
    set(PLATFORM_CMAKE_FILE "${TFM_ROOT_DIR}/platform/ext/psoc64.cmake")
else()
    message(FATAL_ERROR "ERROR: Target \"${TARGET_PLATFORM}\" is not supported.")
endif()

# These variables select the default setting of this TF-M Profile Medium.
# The settings can be overwritten by the customized platform specific settings

# IPC model
set(CORE_IPC ON)
# TF-M isolation level: 2
set(TFM_LVL 2)

# BL2 settings can be overwritten in the profile extension configuration
# or in platform specific cmake file.
if(NOT DEFINED BL2)
    set(BL2 ON)
endif()

if(BL2)
    # Mulit-image boot
    set(MCUBOOT_IMAGE_NUMBER 2 CACHE STRING "Multi-image boot by default")
endif()

# Enable Internal Trusted Storage service
set(TFM_PARTITION_INTERNAL_TRUSTED_STORAGE ON)
# Set the transient internal buffer size to 32 bytes
set(ITS_BUF_SIZE 32)

# Enable Crypto service
set(TFM_PARTITION_CRYPTO ON)
# Set the dedicated mbed-crypto default config file
set(MBEDTLS_CONFIG_FILE "tfm_profile_m_mbedcrypto_config.h")

# Enable Initial Attestation service
set(TFM_PARTITION_INITIAL_ATTESTATION ON)

# Enable Protected Storage service
set(TFM_PARTITION_PROTECTED_STORAGE ON)
# Set AES-CCM as the AEAD algorithm in SST
set(PS_CRYPTO_AEAD_ALG PSA_ALG_CCM)

# Enable Platform service
set(TFM_PARTITION_PLATFORM ON)

# Disable Audit Logging service
set(TFM_PARTITION_AUDIT_LOG OFF)

# Configure the test cases
set (REGRESSION ON)
set (CORE_TEST ON)
set (IPC_TEST ON)
set (PSA_API_TEST OFF)
# Configure Crypto test cases
set(TFM_CRYPTO_TEST_ALG_CBC OFF)
# Select AES-CCM by default
set(TFM_CRYPTO_TEST_ALG_CCM ON)
set(TFM_CRYPTO_TEST_ALG_CFB OFF)
set(TFM_CRYPTO_TEST_ALG_CTR OFF)
set(TFM_CRYPTO_TEST_ALG_GCM OFF)
set(TFM_CRYPTO_TEST_ALG_SHA_512 OFF)
set(TFM_CRYPTO_TEST_HKDF OFF)

# Include platform specific profile configuration extension file if it is
# provided via argument TFM_PROFILE_CONFIG_EXT in command line.
# The configuration extension file can be specified as an absolute path or a
# relative path to TF-M root folder.
if(TFM_PROFILE_CONFIG_EXT)
    get_filename_component(TFM_PROFILE_CONFIG_EXT ${TFM_PROFILE_CONFIG_EXT}
                           ABSOLUTE BASE_DIR ${TFM_ROOT_DIR})

    if(NOT EXISTS ${TFM_PROFILE_CONFIG_EXT})
        message(FATAL_ERROR "The config extension ${TFM_PROFILE_CONFIG_EXT} is not found")
    endif()

    include(${TFM_PROFILE_CONFIG_EXT})
    message("The config extension ${TFM_PROFILE_CONFIG_EXT} is included")
endif()

include("${TFM_ROOT_DIR}/CommonConfig.cmake")
