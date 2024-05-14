#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/version.txt)
    message(WARNING "tf-m-tests version.txt doesn't exist")
    return()
endif()

# Parse the version number after "version="
file(STRINGS ${CMAKE_CURRENT_LIST_DIR}/version.txt VERSION_STRING REGEX "^version=[A-Za-z0-9]*")
if(VERSION_STRING)
    string(REPLACE "version=" "" TFM_TESTS_VERSION ${VERSION_STRING})
endif()
