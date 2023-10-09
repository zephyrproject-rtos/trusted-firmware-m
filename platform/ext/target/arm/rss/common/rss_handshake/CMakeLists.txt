#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

target_include_directories(platform_bl1_2
    PUBLIC
        .
)

target_sources(platform_bl1_2
    PRIVATE
        rss_handshake.c
)

