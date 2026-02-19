#-------------------------------------------------------------------------------
# Copyright (c) 2024-2025 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

fetch_remote_library(
    LIB_NAME                ifx_mbedtls_acceleration
    LIB_SOURCE_PATH_VAR     IFX_MBEDTLS_ACCELERATION_LIB_PATH
    # By using ${IFX_LIB_BASE_DIR} as LIB_BASE_DIR repo will only be downloaded once
    # and then reused by subsequent builds. This speeds up the build.
    LIB_BASE_DIR            "${IFX_LIB_BASE_DIR}"
    LIB_PATCH_DIR           ${IFX_MBEDTLS_ACCELERATION_PATCH_DIR}
    LIB_FORCE_PATCH         ON
    FETCH_CONTENT_ARGS
        GIT_REPOSITORY      ${IFX_MBEDTLS_ACCELERATION_LIB_GIT_REMOTE}
        GIT_TAG             ${IFX_MBEDTLS_ACCELERATION_LIB_VERSION}
        GIT_SHALLOW         TRUE
        GIT_PROGRESS        TRUE
        GIT_SUBMODULES      ""
)

#-------------------------------------------------------------------------------

add_subdirectory(spe)
