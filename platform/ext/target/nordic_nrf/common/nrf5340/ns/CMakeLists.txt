#
# Copyright (c) 2023, Nordic Semiconductor ASA.
#
# SPDX-License-Identifier: BSD-3-Clause
#

target_sources(tfm_ns
     PRIVATE
        gcc/startup_nrf5340.c
)

target_sources(platform_ns
    PRIVATE
        ${HAL_NORDIC_PATH}/nrfx/mdk/system_nrf5340_application.c
)

target_include_directories(platform_ns
    PUBLIC
        partition
)
