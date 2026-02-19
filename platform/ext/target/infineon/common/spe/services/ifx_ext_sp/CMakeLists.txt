#-------------------------------------------------------------------------------
# Copyright (c) 2025 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# This partition is currently located within the tf-m repository to facilitate
# upstreaming while the Infineon tools are being enhanced to support fetching
# partitions from the tf-m-extras repository.
# NOTE: This is a temporary arrangement and should be updated to use the
# tf-m-extras repository once the fetching mechanism is fully implemented.
# Tracked in infineon internal ticket TFM-4601.

if(NOT IFX_EXT_SP)
    return()
endif()

################################ Configuration #################################

set(IFX_EXT_SP_STACK_SIZE   "0x600"   CACHE STRING  "IFX Extensions Partition stack size")

# When enabled, IFX Extensions Partition will make a call to register the user
# SRF module during its initialization. User module implementation can be
# provided in ifx_user_srf_module.c in the same directory as this
# CMakeLists.txt. Alternatively, user module implementation can be integrated
# through other means, such as modifying the partition sources directly or
# including the file in the build system elsewhere.
set(IFX_EXT_SP_REGISTER_USER_SRF_MODULE OFF CACHE BOOL "Whether to register the user SRF module in the IFX Extensions Partition.")

# Registering the user SRF module requires MTB SRF to be enabled
tfm_invalid_config(IFX_EXT_SP_REGISTER_USER_SRF_MODULE AND NOT IFX_MTB_SRF)

################################## Partition ###################################

add_library(tfm_psa_rot_partition_ifx_ext_sp STATIC EXCLUDE_FROM_ALL)

add_dependencies(tfm_psa_rot_partition_ifx_ext_sp manifest_tool)

target_sources(tfm_psa_rot_partition_ifx_ext_sp
    PRIVATE
        ifx_ext_sp_mngr.c

        # The generated sources
        ${IFX_GENERATED_DIR}/secure_fw/partitions/ifx_ext_sp/auto_generated/intermedia_ifx_ext_sp.c
)

if(IFX_EXT_SP_REGISTER_USER_SRF_MODULE AND (EXISTS ${CMAKE_CURRENT_LIST_DIR}/ifx_user_srf_module.c))
    target_sources(tfm_psa_rot_partition_ifx_ext_sp
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/ifx_user_srf_module.c
    )
endif()

target_include_directories(tfm_psa_rot_partition_ifx_ext_sp
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    PRIVATE
        ${IFX_GENERATED_DIR}/secure_fw/partitions/ifx_ext_sp
)

target_link_libraries(tfm_psa_rot_partition_ifx_ext_sp
    PRIVATE
        tfm_sprt
)

################################ TFM partitions ################################

target_sources(tfm_partitions
    INTERFACE
        ${IFX_GENERATED_DIR}/secure_fw/partitions/ifx_ext_sp/auto_generated/load_info_ifx_ext_sp.c
)

target_include_directories(tfm_partitions
    INTERFACE
        ${IFX_GENERATED_DIR}/secure_fw/partitions/ifx_ext_sp
)

target_link_libraries(tfm_partitions
    INTERFACE
        tfm_psa_rot_partition_ifx_ext_sp
)

################################## secure API ##################################

target_sources(tfm_sprt
    PRIVATE
        ifx_ext_sp_api.c
)

target_include_directories(tfm_sprt
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

#################################### config ####################################

target_compile_definitions(tfm_config
    INTERFACE
        TFM_PARTITION_IFX_EXT_SP
        IFX_EXT_SP_STACK_SIZE=${IFX_EXT_SP_STACK_SIZE}
        $<$<BOOL:${IFX_EXT_SP_REGISTER_USER_SRF_MODULE}>:IFX_EXT_SP_REGISTER_USER_SRF_MODULE=1>
)

#################################### install ###################################

install(FILES       ${CMAKE_CURRENT_LIST_DIR}/ifx_ext_sp_defs.h
                    ${CMAKE_CURRENT_LIST_DIR}/ifx_ext_sp_api.h
                    $<$<BOOL:${IFX_MTB_SRF}>:${IFX_COMMON_SOURCE_DIR}/interface/include/mtb_srf_ipc_custom_packet.h>
        DESTINATION ${INSTALL_INTERFACE_INC_DIR})

install(FILES       ${CMAKE_CURRENT_LIST_DIR}/ifx_ext_sp_api.c
                    $<$<BOOL:${IFX_MTB_SRF}>:${IFX_COMMON_SOURCE_DIR}/interface/src/ifx_mtb_srf.c>
                    $<$<BOOL:${IFX_MTB_MAILBOX}>:${IFX_COMMON_SOURCE_DIR}/interface/src/ifx_mtb_srf_relay.c>
        DESTINATION ${INSTALL_INTERFACE_SRC_DIR})
