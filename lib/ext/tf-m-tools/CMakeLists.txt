#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(TFM_TOOLS_PATH          "DOWNLOAD"     CACHE PATH   "Path to tf-m-tools (or DOWNLOAD to fetch automatically)")
set(TFM_TOOLS_VERSION       "0b49b00"      CACHE STRING "The version of tf-m-tools to use")

fetch_remote_library(
    LIB_NAME                tfm_tools_repo
    LIB_SOURCE_PATH_VAR     TFM_TOOLS_PATH
    LIB_BASE_DIR            "${CMAKE_BINARY_DIR}/lib/ext"
    FETCH_CONTENT_ARGS
        GIT_REPOSITORY      https://git.trustedfirmware.org/TF-M/tf-m-tools.git
        GIT_TAG             ${TFM_TOOLS_VERSION}
        GIT_PROGRESS        TRUE
)
