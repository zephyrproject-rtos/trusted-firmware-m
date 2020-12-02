#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0076 NEW)
cmake_policy(SET CMP0079 NEW)


if (TFM_MULTI_CORE_TOPOLOGY)
    include(${CMAKE_SOURCE_DIR}/platform/ext/target/${TFM_PLATFORM}/preload_ns.cmake)
    tfm_toolchain_reload_compiler()
    # The platform target is created in this directory/file so that it has the
    # same settings as the main ns target.
    add_library(platform_ns STATIC)
endif()

###################### PSA interface (header only) #############################

add_library(psa_interface INTERFACE)

target_include_directories(psa_interface
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/generated/interface/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/os_wrapper
)

# PSA interface files are generated from a template
add_dependencies(psa_interface
    tfm_generated_files
)

target_link_libraries(psa_interface
    INTERFACE
        tfm_partition_defs
)

target_compile_definitions(psa_interface
    INTERFACE
        $<$<BOOL:${TFM_PSA_API}>:TFM_PSA_API>
        $<$<BOOL:${TFM_NS_CLIENT_IDENTIFICATION}>:TFM_NS_CLIENT_IDENTIFICATION>
        $<$<BOOL:${CONFIG_TFM_ENABLE_CTX_MGMT}>:CONFIG_TFM_ENABLE_CTX_MGMT>
        $<$<BOOL:${TFM_ISOLATION_LEVEL}>:TFM_LVL=${TFM_ISOLATION_LEVEL}>
        $<$<OR:$<VERSION_GREATER:${TFM_ISOLATION_LEVEL},1>,$<STREQUAL:"${TEST_PSA_API}","IPC">>:CONFIG_TFM_ENABLE_MEMORY_PROTECT>
        $<$<BOOL:${TFM_MULTI_CORE_TOPOLOGY}>:TFM_MULTI_CORE_TOPOLOGY>
        $<$<BOOL:${TFM_MULTI_CORE_MULTI_CLIENT_CALL}>:TFM_MULTI_CORE_MULTI_CLIENT_CALL>
)

###################### PSA api (S lib) #########################################

target_sources(tfm_secure_api
    INTERFACE
        $<$<BOOL:${TFM_PSA_API}>:${CMAKE_CURRENT_SOURCE_DIR}/src/psa/psa_client.c>
        $<$<BOOL:${TFM_PSA_API}>:${CMAKE_CURRENT_SOURCE_DIR}/src/psa/psa_service.c>
        $<$<BOOL:${TFM_PSA_API}>:${CMAKE_CURRENT_SOURCE_DIR}/src/psa/psa_lifecycle.c>
        ${CMAKE_CURRENT_SOURCE_DIR}/src/log/tfm_log_raw.c
)

###################### PSA api (NS lib) ########################################

add_library(psa_api_ns STATIC)

target_sources(psa_api_ns
    PRIVATE
        $<$<BOOL:${TFM_NS_CLIENT_IDENTIFICATION}>:src/tfm_nspm_svc_handler.c>
        $<$<BOOL:${TFM_NS_CLIENT_IDENTIFICATION}>:src/tfm_nspm_api.c>
)

if (${TFM_PSA_API})
    target_sources(psa_api_ns PRIVATE
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/src/tfm_platform_ipc_api.c>
        $<$<BOOL:${TFM_PARTITION_PROTECTED_STORAGE}>:${CMAKE_CURRENT_SOURCE_DIR}/src/tfm_ps_ipc_api.c>
        $<$<BOOL:${TFM_PARTITION_INTERNAL_TRUSTED_STORAGE}>:${CMAKE_CURRENT_SOURCE_DIR}/src/tfm_its_ipc_api.c>
        $<$<BOOL:${TFM_PARTITION_CRYPTO}>:${CMAKE_CURRENT_SOURCE_DIR}/src/tfm_crypto_ipc_api.c>
        $<$<BOOL:${TFM_PARTITION_INITIAL_ATTESTATION}>:${CMAKE_CURRENT_SOURCE_DIR}/src/tfm_initial_attestation_ipc_api.c>
    )

    if (TFM_MULTI_CORE_TOPOLOGY)
        target_sources(psa_api_ns PRIVATE
            src/tfm_ns_mailbox.c
            src/tfm_multi_core_api.c
            src/tfm_multi_core_psa_ns_api.c
        )
    else()
        target_sources(psa_api_ns PRIVATE
           src/tfm_ns_interface.c
           src/tfm_psa_ns_api.c
        )
    endif()
else()
    target_sources(psa_api_ns PRIVATE
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/src/tfm_platform_func_api.c>
        $<$<BOOL:${TFM_PARTITION_AUDIT_LOG}>:${CMAKE_CURRENT_SOURCE_DIR}/src/tfm_audit_func_api.c>
        $<$<BOOL:${TFM_PARTITION_PROTECTED_STORAGE}>:${CMAKE_CURRENT_SOURCE_DIR}/src/tfm_ps_func_api.c>
        $<$<BOOL:${TFM_PARTITION_INTERNAL_TRUSTED_STORAGE}>:${CMAKE_CURRENT_SOURCE_DIR}/src/tfm_its_func_api.c>
        $<$<BOOL:${TFM_PARTITION_CRYPTO}>:${CMAKE_CURRENT_SOURCE_DIR}/src/tfm_crypto_func_api.c>
        $<$<BOOL:${TFM_PARTITION_INITIAL_ATTESTATION}>:${CMAKE_CURRENT_SOURCE_DIR}/src/tfm_initial_attestation_func_api.c>
        src/tfm_psa_ns_api.c
    )
    if (NOT TFM_MULTI_CORE_TOPOLOGY)
        target_sources(psa_api_ns PRIVATE
           src/tfm_ns_interface.c
        )
    endif()

endif()

target_link_libraries(psa_api_ns
    PUBLIC
        psa_interface
    PRIVATE
        platform_ns
        # CMSIS is currently only required to provide the NS client IDs. In
        # future, this should probably be made more OS agnostic
        $<$<BOOL:${TFM_NS_CLIENT_IDENTIFICATION}>:CMSIS_5_tfm_ns>
)
