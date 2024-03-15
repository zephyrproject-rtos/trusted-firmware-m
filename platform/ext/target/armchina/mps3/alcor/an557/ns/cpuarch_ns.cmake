#-------------------------------------------------------------------------------
# Copyright (c) 2020-2023, Arm Limited. All rights reserved.
# Copyright (c) 2024 ArmChina. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# cpuarch.cmake is used to set things that related to the platform that are both
# immutable and global, which is to say they should apply to any kind of project
# that uses this platform. In practise this is normally compiler definitions and
# variables related to hardware.

add_definitions(
    -DALCOR_AN557
)

set(ALCOR_COMMON_DIR "${CMAKE_CURRENT_LIST_DIR}/common")
set(TARGET_PLATFORM_PATH    ${CMAKE_CURRENT_LIST_DIR})

include(${ALCOR_COMMON_DIR}/cpuarch.cmake)
