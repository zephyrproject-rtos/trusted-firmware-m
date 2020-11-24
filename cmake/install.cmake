#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Generate correct filename
string(TOUPPER ${TFM_PLATFORM} TFM_PLATFORM_UPPERCASE)
string(REGEX REPLACE "-" "_" TFM_PLATFORM_UPPERCASE_UNDERSCORE ${TFM_PLATFORM_UPPERCASE})

install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/
        DESTINATION ${CMAKE_BINARY_DIR}/install/outputs/${TFM_PLATFORM_UPPERCASE_UNDERSCORE}
)

set(INTERFACE_INC_DIR ${CMAKE_SOURCE_DIR}/interface/include)
set(INTERFACE_SRC_DIR ${CMAKE_SOURCE_DIR}/interface/src)
set(EXPORT_INC_DIR    ${CMAKE_BINARY_DIR}/install/export/tfm/include)
set(EXPORT_SRC_DIR    ${CMAKE_BINARY_DIR}/install/export/tfm/src)

# export NS static lib
install(TARGETS psa_api_ns
        ARCHIVE
        DESTINATION ${CMAKE_BINARY_DIR}/install/export/tfm/lib
)

# export veneer lib
if (NOT TFM_MULTI_CORE_TOPOLOGY)
    install(FILES       ${CMAKE_BINARY_DIR}/secure_fw/s_veneers.o
            DESTINATION ${CMAKE_BINARY_DIR}/install/export/tfm/lib)
endif()

####################### export headers #########################################

install(FILES       ${INTERFACE_INC_DIR}/psa/client.h
                    ${INTERFACE_INC_DIR}/psa/error.h
        DESTINATION ${EXPORT_INC_DIR}/psa)

install(FILES       ${INTERFACE_INC_DIR}/os_wrapper/common.h
                    ${INTERFACE_INC_DIR}/os_wrapper/mutex.h
        DESTINATION ${EXPORT_INC_DIR}/os_wrapper)

install(FILES       ${CMAKE_BINARY_DIR}/generated/interface/include/psa_manifest/sid.h
        DESTINATION ${EXPORT_INC_DIR}/psa_manifest)

install(FILES       ${INTERFACE_INC_DIR}/tfm_api.h
                    ${INTERFACE_INC_DIR}/tfm_ns_interface.h
                    ${INTERFACE_INC_DIR}/tfm_ns_svc.h
        DESTINATION ${EXPORT_INC_DIR})

install(FILES       ${INTERFACE_INC_DIR}/ext/tz_context.h
        DESTINATION ${EXPORT_INC_DIR}/ext)

if (TFM_MULTI_CORE_TOPOLOGY)
    install(FILES       ${INTERFACE_INC_DIR}/tfm_multi_core_api.h
                        ${INTERFACE_INC_DIR}/tfm_ns_mailbox.h
                        ${INTERFACE_INC_DIR}/tfm_mailbox.h
            DESTINATION ${EXPORT_INC_DIR})
else()
    install(FILES       ${CMAKE_BINARY_DIR}/generated/interface/include/tfm_veneers.h
            DESTINATION ${EXPORT_INC_DIR}/tfm/veneers)
endif()

if (TFM_NS_CLIENT_IDENTIFICATION)
    install(FILES       ${INTERFACE_INC_DIR}/tfm_nspm_api.h
                        ${INTERFACE_INC_DIR}/tfm_nspm_svc_handler.h
            DESTINATION ${EXPORT_INC_DIR})
endif()

if (TFM_PARTITION_PROTECTED_STORAGE)
    install(FILES       ${INTERFACE_INC_DIR}/psa/protected_storage.h
            DESTINATION ${EXPORT_INC_DIR}/psa)
endif()

if (TFM_PARTITION_INTERNAL_TRUSTED_STORAGE)
    install(FILES       ${INTERFACE_INC_DIR}/psa/internal_trusted_storage.h
                        ${INTERFACE_INC_DIR}/psa/storage_common.h
            DESTINATION ${EXPORT_INC_DIR}/psa)
endif()

if (TFM_PARTITION_CRYPTO)
    install(FILES       ${INTERFACE_INC_DIR}/psa/crypto_extra.h
                        ${INTERFACE_INC_DIR}/psa/crypto_compat.h
                        ${INTERFACE_INC_DIR}/psa/crypto.h
                        ${INTERFACE_INC_DIR}/psa/crypto_client_struct.h
                        ${INTERFACE_INC_DIR}/psa/crypto_sizes.h
                        ${INTERFACE_INC_DIR}/psa/crypto_struct.h
                        ${INTERFACE_INC_DIR}/psa/crypto_types.h
                        ${INTERFACE_INC_DIR}/psa/crypto_values.h
            DESTINATION ${EXPORT_INC_DIR}/psa)
    install(FILES       ${INTERFACE_INC_DIR}/tfm_crypto_defs.h
            DESTINATION ${EXPORT_INC_DIR})
endif()

if (TFM_PARTITION_INITIAL_ATTESTATION)
    install(FILES       ${INTERFACE_INC_DIR}/psa/initial_attestation.h
            DESTINATION ${EXPORT_INC_DIR}/psa)
endif()

if(TFM_PARTITION_AUDIT_LOG)
    install(FILES       ${INTERFACE_INC_DIR}/psa_audit_api.h
                        ${INTERFACE_INC_DIR}/psa_audit_defs.h
            DESTINATION ${EXPORT_INC_DIR})
endif()

if(TFM_PARTITION_PLATFORM)
    install(FILES       ${INTERFACE_INC_DIR}/tfm_platform_api.h
            DESTINATION ${EXPORT_INC_DIR})
endif()

####################### export sources #########################################

if (TFM_MULTI_CORE_TOPOLOGY)
    install(FILES       ${INTERFACE_SRC_DIR}/tfm_ns_mailbox.c
                        ${INTERFACE_SRC_DIR}/tfm_multi_core_api.c
                        ${INTERFACE_SRC_DIR}/tfm_multi_core_psa_ns_api.c
            DESTINATION ${EXPORT_SRC_DIR})
else()
    install(FILES       ${INTERFACE_SRC_DIR}/tfm_ns_interface.c
            DESTINATION ${EXPORT_SRC_DIR})

    if(TFM_PSA_API)
        install(FILES       ${INTERFACE_SRC_DIR}/tfm_psa_ns_api.c
                DESTINATION ${EXPORT_SRC_DIR})
    endif()
endif()

if (TFM_NS_CLIENT_IDENTIFICATION)
    install(FILES       ${INTERFACE_SRC_DIR}/tfm_nspm_api.c
                        ${INTERFACE_SRC_DIR}/tfm_nspm_svc_handler.c
            DESTINATION ${EXPORT_SRC_DIR})
endif()

if (TFM_PARTITION_PROTECTED_STORAGE)
    if (TFM_PSA_API)
        install(FILES       ${INTERFACE_SRC_DIR}/tfm_ps_ipc_api.c
                DESTINATION ${EXPORT_SRC_DIR})
    else()
        install(FILES       ${INTERFACE_SRC_DIR}/tfm_ps_func_api.c
                DESTINATION ${EXPORT_SRC_DIR})
    endif()
endif()

if (TFM_PARTITION_INTERNAL_TRUSTED_STORAGE)
    if (TFM_PSA_API)
        install(FILES       ${INTERFACE_SRC_DIR}/tfm_its_ipc_api.c
                DESTINATION ${EXPORT_SRC_DIR})
    else()
        install(FILES       ${INTERFACE_SRC_DIR}/tfm_its_func_api.c
                DESTINATION ${EXPORT_SRC_DIR})
    endif()
endif()

if (TFM_PARTITION_CRYPTO)
    if (TFM_PSA_API)
        install(FILES       ${INTERFACE_SRC_DIR}/tfm_crypto_ipc_api.c
                DESTINATION ${EXPORT_SRC_DIR})
    else()
        install(FILES       ${INTERFACE_SRC_DIR}/tfm_crypto_func_api.c
                DESTINATION ${EXPORT_SRC_DIR})
    endif()
endif()

if (TFM_PARTITION_INITIAL_ATTESTATION)
    if (TFM_PSA_API)
        install(FILES       ${INTERFACE_SRC_DIR}/tfm_initial_attestation_ipc_api.c
                DESTINATION ${EXPORT_SRC_DIR})
    else()
        install(FILES       ${INTERFACE_SRC_DIR}/tfm_initial_attestation_func_api.c
                DESTINATION ${EXPORT_SRC_DIR})
    endif()
endif()

if(TFM_PARTITION_AUDIT_LOG)
    install(FILES       ${INTERFACE_SRC_DIR}/tfm_audit_func_api.c
            DESTINATION ${EXPORT_SRC_DIR})
endif()

if(TFM_PARTITION_PLATFORM)
    if(TFM_PSA_API)
        install(FILES       ${INTERFACE_SRC_DIR}/tfm_platform_ipc_api.c
                DESTINATION ${EXPORT_SRC_DIR})
    else()
        install(FILES       ${INTERFACE_SRC_DIR}/tfm_platform_func_api.c
                DESTINATION ${EXPORT_SRC_DIR})
    endif()
endif()
