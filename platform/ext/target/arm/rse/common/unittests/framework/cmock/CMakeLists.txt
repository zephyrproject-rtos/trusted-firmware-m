#-------------------------------------------------------------------------------
# Copyright (c) 2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(FETCHCONTENT_QUIET FALSE)

set(CMOCK_PATH      "DOWNLOAD"  CACHE PATH          "Path to CMock (or DOWNLOAD to fetch automatically)")
set(CMOCK_TAG       "v2.5.3"    CACHE STRING        "CMock version to use")

fetch_remote_library(
    LIB_NAME                    cmock
    LIB_SOURCE_PATH_VAR         CMOCK_PATH
    LIB_PATCH_DIR               ${CMAKE_CURRENT_LIST_DIR}
    LIB_BASE_DIR                ${CMAKE_BINARY_DIR}/unittests
    FETCH_CONTENT_ARGS
        GIT_REPOSITORY          https://github.com/ThrowTheSwitch/CMock
        GIT_TAG                 ${CMOCK_TAG}
        GIT_SHALLOW             TRUE
        GIT_PROGRESS            FALSE
        GIT_SUBMODULES_RECURSE  TRUE
)

add_library(cmock STATIC ${CMOCK_PATH}/src/cmock.c)

target_link_libraries(cmock unity)

target_include_directories(cmock PUBLIC ${CMOCK_PATH}/src)
