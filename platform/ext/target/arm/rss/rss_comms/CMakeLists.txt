#-------------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

target_include_directories(platform_s
    PUBLIC
        .
)

target_sources(platform_s
    PRIVATE
        rss_comms.c
        rss_comms_hal.c
        rss_comms_queue.c
        rss_comms_protocol.c
        rss_comms_protocol_embed.c
)

target_compile_definitions(platform_s
    PRIVATE
        RSS_COMMS_MAX_CONCURRENT_REQ=2
        RSS_COMMS_PROTOCOL_EMBED_ENABLED
)

# For spm_log_msgval
target_link_libraries(platform_s
    PRIVATE
        tfm_spm
)

