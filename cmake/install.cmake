#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Generate correct filename
string(TOUPPER ${TFM_PLATFORM} TFM_PLATFORM_UPPERCASE)
string(REGEX REPLACE "-" "_" TFM_PLATFORM_UPPERCASE_UNDERSCORE ${TFM_PLATFORM_UPPERCASE})

install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/
        DESTINATION ${CMAKE_BINARY_DIR}/install/outputs/${TFM_PLATFORM_UPPERCASE_UNDERSCORE}
    )
