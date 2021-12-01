#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)

add_library(tfm_sprt STATIC)

target_include_directories(tfm_sprt
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${CMAKE_SOURCE_DIR}/secure_fw/include
)

target_sources(tfm_sprt
    PRIVATE
        ./crt_memcmp.c
        ./crt_memcpy.c
        ./crt_memmove.c
        ./crt_memset.c
        ./crt_strnlen.c
        ./service_api.c
        $<$<BOOL:${TFM_SP_META_PTR_ENABLE}>:./sprt_partition_metadata_indicator.c>
)

target_link_libraries(tfm_sprt
    platform_s
    psa_interface
    tfm_boot_status
    tfm_secure_api
    $<$<BOOL:${TFM_SP_LOG_RAW_ENABLED}>:tfm_sp_log_raw>
)

target_compile_definitions(tfm_partition_defs
    INTERFACE
        TFM_PARTITION_LOG_LEVEL=${TFM_PARTITION_LOG_LEVEL}
        $<$<BOOL:${TFM_SP_LOG_RAW_ENABLED}>:TFM_SP_LOG_RAW_ENABLED>
)

# Create a dedicated tfm_sp_log_raw library for some regression test modules
# which don't include tfm_sprt.
# tfm_sp_log_raw shall be controlled by TFM_SP_LOG_RAW_ENABLED.
# Otherwise, build will fail if TFM_SP_LOG_RAW_ENABLED is disabled since some
# definitions are unavailable with TFM_SP_LOG_RAW_ENABLED disabled.
if (TFM_SP_LOG_RAW_ENABLED)
    add_library(tfm_sp_log_raw INTERFACE)

    target_sources(tfm_sp_log_raw
        INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/tfm_sp_log_raw.c
            ${CMAKE_SOURCE_DIR}/platform/ext/common/tfm_hal_sp_logdev_periph.c
    )

    target_include_directories(tfm_sp_log_raw
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

    target_link_libraries(tfm_sp_log_raw
        INTERFACE
            platform_s
            tfm_partition_defs
            tfm_secure_api
    )
endif()
