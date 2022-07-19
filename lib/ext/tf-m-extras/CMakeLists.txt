#-------------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

# If tf-m-extras partitions are required, please set TFM_EXTRAS_REPO_PATH to
# DOWNLOAD and TFM_EXTRAS_REPO_VERSION to appropriate commit-id in the platform
# specific config. By default, they are not fetched.
set(TFM_EXTRAS_REPO_PATH                ""         CACHE PATH    "Path to tf-m-extras repo (or DOWNLOAD to fetch automatically")
set(TFM_EXTRAS_REPO_VERSION             ""         CACHE STRING  "The version of tf-m-extras to use")

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0")
    cmake_policy(SET CMP0097 NEW)
endif()

if ("${TFM_EXTRAS_REPO_PATH}" STREQUAL "DOWNLOAD")
    FetchContent_Declare(tf-m-extras
        GIT_REPOSITORY https://git.trustedfirmware.org/TF-M/tf-m-extras.git
        GIT_TAG ${TFM_EXTRAS_REPO_VERSION}
        GIT_SHALLOW FALSE
        GIT_PROGRESS TRUE
    )

    FetchContent_GetProperties(tf-m-extras)
    if(NOT tfm_extras_repo_POPULATED)
        FetchContent_Populate(tf-m-extras)
        set(TFM_EXTRAS_REPO_PATH "${CMAKE_CURRENT_BINARY_DIR}-src" CACHE PATH "Path to TFM-EXTRAS repo (or DOWNLOAD to fetch automatically" FORCE)
    endif()
endif()
