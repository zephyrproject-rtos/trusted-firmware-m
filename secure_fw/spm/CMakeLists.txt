#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)

add_library(tfm_spm STATIC)
add_library(tfm_boot_status INTERFACE)
add_library(tfm_arch INTERFACE)
add_library(tfm_utilities INTERFACE)

set(TFM_PARTITION_PLATFORM ON CACHE BOOL "Enable the TF-M Platform partition")

# Generate TF-M version
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/tfm_version.h.in
               ${CMAKE_BINARY_DIR}/generated/secure_fw/spm/include/tfm_version.h)

target_include_directories(tfm_spm
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        $<$<BOOL:${TFM_PSA_API}>:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_psa>
        $<$<BOOL:${TFM_PSA_API}>:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_psa/include>
        $<$<NOT:$<BOOL:${TFM_PSA_API}>>:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_func>
        $<$<NOT:$<BOOL:${TFM_PSA_API}>>:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_func/include>
    PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/secure_fw/include
        ${CMAKE_BINARY_DIR}/generated
        ${CMAKE_BINARY_DIR}/generated/secure_fw/spm/include
        $<$<BOOL:${TFM_PSA_API}>:${CMAKE_BINARY_DIR}/generated/secure_fw/spm/cmsis_psa>
        $<$<NOT:$<BOOL:${TFM_PSA_API}>>:${CMAKE_BINARY_DIR}/generated/secure_fw/spm/cmsis_func>
)

target_sources(tfm_spm
    PRIVATE
        ffm/tfm_boot_data.c
        ffm/tfm_core_utils.c
        ffm/utilities.c
        $<$<BOOL:${TFM_EXCEPTION_INFO_DUMP}>:cmsis_psa/exception_info.c>
        $<$<NOT:$<STREQUAL:${TFM_SPM_LOG_LEVEL},TFM_SPM_LOG_LEVEL_SILENCE>>:ffm/spm_log.c>
        $<$<BOOL:${TFM_MULTI_CORE_TOPOLOGY}>:cmsis_psa/tfm_multi_core.c>
        $<$<BOOL:${TFM_MULTI_CORE_TOPOLOGY}>:cmsis_psa/tfm_multi_core_mem_check.c>
        $<$<BOOL:${TFM_MULTI_CORE_TOPOLOGY}>:cmsis_psa/tfm_rpc.c>
        $<$<AND:$<BOOL:${TFM_MULTI_CORE_TOPOLOGY}>,$<NOT:$<BOOL:${TFM_PLAT_SPECIFIC_MULTI_CORE_COMM}>>>:cmsis_psa/tfm_spe_mailbox.c>
        $<$<NOT:$<BOOL:${TFM_PSA_API}>>:ffm/tfm_core_mem_check.c>
        $<$<BOOL:${TFM_PSA_API}>:cmsis_psa/arch/tfm_arch.c>
        $<$<BOOL:${TFM_PSA_API}>:cmsis_psa/main.c>
        $<$<BOOL:${TFM_PSA_API}>:cmsis_psa/spm_ipc.c>
        $<$<BOOL:${TFM_PSA_API}>:cmsis_psa/spm_thread_call.c>
        $<$<BOOL:${TFM_PSA_API}>:cmsis_psa/static_load.c>
        $<$<BOOL:${TFM_PSA_API}>:ffm/psa_api.c>
        $<$<BOOL:${TFM_PSA_API}>:ffm/backend.c>
        $<$<BOOL:${TFM_PSA_API}>:cmsis_psa/tfm_core_svcalls_ipc.c>
        $<$<AND:$<BOOL:${TFM_PSA_API}>,$<NOT:$<BOOL:${TFM_MULTI_CORE_TOPOLOGY}>>>:cmsis_psa/tfm_nspm_ipc.c>
        $<$<BOOL:${TFM_PSA_API}>:cmsis_psa/tfm_pools.c>
        $<$<BOOL:${TFM_PSA_API}>:cmsis_psa/thread.c>
        $<$<NOT:$<BOOL:${TFM_PSA_API}>>:cmsis_func/main.c>
        $<$<NOT:$<BOOL:${TFM_PSA_API}>>:cmsis_func/arch.c>
        $<$<NOT:$<BOOL:${TFM_PSA_API}>>:cmsis_func/spm_func.c>
        $<$<NOT:$<BOOL:${TFM_PSA_API}>>:cmsis_func/tfm_core_svcalls_func.c>
        $<$<NOT:$<BOOL:${TFM_PSA_API}>>:cmsis_func/tfm_nspm_func.c>
        $<$<AND:$<NOT:$<BOOL:${TFM_MULTI_CORE_TOPOLOGY}>>,$<BOOL:${TFM_NS_MANAGE_NSID}>>:ns_client_ext/tfm_ns_ctx.c>
        $<$<NOT:$<BOOL:${TFM_MULTI_CORE_TOPOLOGY}>>:ns_client_ext/tfm_spm_ns_ctx.c>
        $<$<NOT:$<BOOL:${TFM_PSA_API}>>:cmsis_func/tfm_secure_api.c>
        #TODO add other arches
        $<$<AND:$<BOOL:${TFM_PSA_API}>,$<STREQUAL:${TFM_SYSTEM_ARCHITECTURE},armv8.1-m.main>>:cmsis_psa/arch/tfm_arch_v8m_main.c>
        $<$<AND:$<BOOL:${TFM_PSA_API}>,$<STREQUAL:${TFM_SYSTEM_ARCHITECTURE},armv8-m.base>>:cmsis_psa/arch/tfm_arch_v8m_base.c>
        $<$<AND:$<BOOL:${TFM_PSA_API}>,$<STREQUAL:${TFM_SYSTEM_ARCHITECTURE},armv8-m.main>>:cmsis_psa/arch/tfm_arch_v8m_main.c>
        $<$<AND:$<BOOL:${TFM_PSA_API}>,$<STREQUAL:${TFM_SYSTEM_ARCHITECTURE},armv6-m>>:cmsis_psa/arch/tfm_arch_v6m_v7m.c>
        $<$<AND:$<BOOL:${TFM_PSA_API}>,$<STREQUAL:${TFM_SYSTEM_ARCHITECTURE},armv7-m>>:cmsis_psa/arch/tfm_arch_v6m_v7m.c>
)

target_include_directories(tfm_partitions INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/cmsis_psa)

#workaround for arch-test
if (TEST_PSA_API STREQUAL IPC)
    target_include_directories(tfm_partitions INTERFACE ${CMAKE_BINARY_DIR}/generated/api-tests/platform/manifests)
    target_sources(tfm_psa_rot_partition_driver_partition PRIVATE
        ${CMAKE_BINARY_DIR}/generated/api-tests/platform/manifests/auto_generated/intermedia_driver_partition_psa.c)
    target_compile_definitions(tfm_psa_rot_partition_driver_partition PRIVATE CONFIG_TFM_BUILDING_SPE=1 TFM_LVL=${TFM_ISOLATION_LEVEL})
    target_sources(tfm_app_rot_partition_client_partition PRIVATE
        ${CMAKE_BINARY_DIR}/generated/api-tests/platform/manifests/auto_generated/intermedia_client_partition_psa.c)
    target_compile_definitions(tfm_app_rot_partition_client_partition PRIVATE CONFIG_TFM_BUILDING_SPE=1 TFM_LVL=${TFM_ISOLATION_LEVEL})
    target_sources(tfm_app_rot_partition_server_partition PRIVATE
        ${CMAKE_BINARY_DIR}/generated/api-tests/platform/manifests/auto_generated/intermedia_server_partition_psa.c)
    target_compile_definitions(tfm_app_rot_partition_server_partition PRIVATE CONFIG_TFM_BUILDING_SPE=1 TFM_LVL=${TFM_ISOLATION_LEVEL})
    target_sources(tfm_partitions INTERFACE
        ${CMAKE_BINARY_DIR}/generated/api-tests/platform/manifests/auto_generated/load_info_driver_partition_psa.c
        ${CMAKE_BINARY_DIR}/generated/api-tests/platform/manifests/auto_generated/load_info_client_partition_psa.c
        ${CMAKE_BINARY_DIR}/generated/api-tests/platform/manifests/auto_generated/load_info_server_partition_psa.c
    )
endif()

target_link_libraries(tfm_spm
    PUBLIC
        tfm_arch
    PRIVATE
        psa_interface
        platform_s
        tfm_boot_status
        tfm_secure_api
        tfm_partitions
        $<$<STREQUAL:${TEST_PSA_API},IPC>:tfm_psa_rot_partition_driver_partition>
        $<$<STREQUAL:${TEST_PSA_API},IPC>:tfm_app_rot_partition_client_partition>
        $<$<STREQUAL:${TEST_PSA_API},IPC>:tfm_app_rot_partition_server_partition>
        tfm_fih
        tfm_sprt
)

target_compile_definitions(tfm_spm
    PRIVATE
        $<$<CONFIG:Debug>:TFM_CORE_DEBUG>
        $<$<AND:$<BOOL:${BL2}>,$<BOOL:${MCUBOOT_MEASURED_BOOT}>>:BOOT_DATA_AVAILABLE>
        $<$<BOOL:${TFM_EXCEPTION_INFO_DUMP}>:TFM_EXCEPTION_INFO_DUMP>
        $<$<BOOL:${TFM_NS_MANAGE_NSID}>:TFM_NS_MANAGE_NSID>
        CONFIG_TFM_SPE_FP=${CONFIG_TFM_SPE_FP}
)

target_compile_options(tfm_spm
    PUBLIC
        ${COMPILER_CP_FLAG}
)

# The veneers give warnings about not being properly declared so they get hidden
# to not overshadow _real_ warnings.
set_source_files_properties(tfm_secure_api.c
    PROPERTIES
        COMPILE_FLAGS
            $<$<C_COMPILER_ID:ARMClang>:-Wno-implicit-function-declaration>
            $<$<C_COMPILER_ID:GNU>:-Wno-implicit-function-declaration>
            $<$<C_COMPILER_ID:IAR>:>
)

############################ Partition Defs ####################################

target_compile_definitions(tfm_partition_defs
    INTERFACE
        $<$<STREQUAL:${TEST_PSA_API},IPC>:PSA_API_TEST_IPC>
)

############################ TFM arch ##########################################

target_include_directories(tfm_arch
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_psa/arch>
)

############################ Boot Status #######################################

target_include_directories(tfm_boot_status
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

############################ TFM utilities #####################################

target_include_directories(tfm_utilities
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

############################ Secure API ########################################

target_link_libraries(tfm_secure_api
    INTERFACE
        tfm_partitions
)

target_include_directories(tfm_secure_api
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/interface>
        $<$<NOT:$<BOOL:${TFM_PSA_API}>>:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_func/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/arch/include>
)

############################# Secure veneers ###################################

if(NOT TFM_MULTI_CORE_TOPOLOGY)
    # If this is added to the spm, it is discarded as it is not used. Since the
    # spm is a static library it can't generate veneers under all compilers so
    # instead this single file is added to the tfm_s target.
    target_sources(tfm_s
        PRIVATE
            $<$<NOT:$<BOOL:${TFM_PSA_API}>>:${CMAKE_BINARY_DIR}/generated/secure_fw/spm/cmsis_func/tfm_veneers.c>
            $<$<BOOL:${TFM_NS_MANAGE_NSID}>:${CMAKE_CURRENT_SOURCE_DIR}/ns_client_ext/tfm_ns_client_ext.c>
    )
endif()
