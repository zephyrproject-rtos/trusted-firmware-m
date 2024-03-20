#-------------------------------------------------------------------------------
# Copyright (c) 2020-2024, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)
set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

#========================= Platform region defs ===============================#

target_include_directories(platform_region_defs
    INTERFACE
        partition
        cc312
)

target_compile_definitions(platform_region_defs
    INTERFACE
        $<$<BOOL:${RSE_XIP}>:RSE_XIP>
        $<$<BOOL:${RSE_DEBUG_UART}>:RSE_DEBUG_UART>
        $<$<BOOL:${RSE_USE_HOST_UART}>:RSE_USE_HOST_UART>
        $<$<BOOL:${RSE_USE_HOST_FLASH}>:RSE_USE_HOST_FLASH>
        $<$<BOOL:${RSE_GPT_SUPPORT}>:RSE_GPT_SUPPORT>
        $<$<BOOL:${RSE_HAS_EXPANSION_PERIPHERALS}>:RSE_HAS_EXPANSION_PERIPHERALS>
        $<$<BOOL:${PLATFORM_HAS_PS_NV_OTP_COUNTERS}>:PLATFORM_HAS_PS_NV_OTP_COUNTERS>
        $<$<BOOL:${RSE_ENABLE_BRINGUP_HELPERS}>:RSE_ENABLE_BRINGUP_HELPERS>
        $<$<BOOL:${RSE_OTP_TRNG}>:RSE_OTP_TRNG>
        $<$<BOOL:${RSE_LOAD_NS_IMAGE}>:RSE_LOAD_NS_IMAGE>
        $<$<BOOL:${RSE_ENABLE_TRAM}>:RSE_ENABLE_TRAM>
        $<$<BOOL:${RSE_BIT_PROGRAMMABLE_OTP}>:RSE_BIT_PROGRAMMABLE_OTP>
        $<$<BOOL:${RSE_USE_ROM_LIB_FROM_SRAM}>:RSE_USE_ROM_LIB_FROM_SRAM>
        $<$<BOOL:${RSE_SUPPORT_ROM_LIB_RELOCATION}>:RSE_SUPPORT_ROM_LIB_RELOCATION>
        $<$<BOOL:${RSE_BRINGUP_OTP_EMULATION}>:RSE_BRINGUP_OTP_EMULATION>
        RSE_AMOUNT=${RSE_AMOUNT}
        $<$<BOOL:${RSE_HAS_MANUFACTURING_DATA}>:RSE_HAS_MANUFACTURING_DATA>
        $<$<BOOL:${TFM_PARTITION_DELEGATED_ATTESTATION}>:RSE_BOOT_KEYS_CCA>
        $<$<BOOL:${TFM_PARTITION_DPE}>:RSE_BOOT_KEYS_DPE>
        LCM_VERSION=0
        RSE_TP_MODE=LCM_TP_MODE_${RSE_TP_MODE}
        LCM_DCU_PARITY
)

#========================= Platform common defs ===============================#

# Specify the location of platform specific build dependencies.
target_sources(tfm_s
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rse.c
)
target_add_scatter_file(tfm_s
        $<$<C_COMPILER_ID:ARMClang>:${PLATFORM_DIR}/ext/common/armclang/tfm_common_s.sct>
        $<$<C_COMPILER_ID:GNU>:${PLATFORM_DIR}/ext/common/gcc/tfm_common_s.ld>
)

target_sources(bl2
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rse_bl.c
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:${CMAKE_CURRENT_SOURCE_DIR}/bl2/boot_dma.c>
)

target_add_scatter_file(bl2
        $<$<C_COMPILER_ID:ARMClang>:${PLATFORM_DIR}/ext/common/armclang/tfm_common_bl2.sct>
        $<$<C_COMPILER_ID:GNU>:${PLATFORM_DIR}/ext/common/gcc/tfm_common_bl2.ld>
)

target_compile_options(bl2_scatter
    PRIVATE
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:${COMPILER_CMSE_FLAG}>
)

target_compile_definitions(bootutil
    PRIVATE
        MULTIPLE_EXECUTABLE_RAM_REGIONS
)

target_sources(bl1_1
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rse_bl1_1.c
)
target_add_scatter_file(bl1_1
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/rse_bl1_1.sct>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/rse_bl1_1.ld>
)

target_sources(bl1_2
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/device/source/startup_rse_bl.c
)
target_add_scatter_file(bl1_2
        $<$<C_COMPILER_ID:ARMClang>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/armclang/rse_bl1_2.sct>
        $<$<C_COMPILER_ID:GNU>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/gcc/rse_bl1_2.ld>
)


install(FILES       tfm_builtin_key_ids.h
        DESTINATION ${INSTALL_INTERFACE_INC_DIR})

#========================= Platform Secure ====================================#

if (TFM_MULTI_CORE_TOPOLOGY)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/rse_comms rse_comms)
endif()

if (RSE_AMOUNT GREATER 1)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/rse_handshake rse_handshake)
endif()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/provisioning provisioning)

# Set which PLATFORM depedency must be used by the CC3XX driver
set(CC3XX_PLATFORM_INTERFACE platform_bl1_1_interface)
add_subdirectory(${PLATFORM_DIR}/ext/accelerator/cc312/cc312-rom cc312-rom)

target_include_directories(platform_s
    PUBLIC
        .
        ../common
        cmsis_drivers
        cmsis_drivers/config
        device
        device/config
        device/include
        device/source/armclang
        dpe
        native_drivers
        libraries
        partition
        services/src
        ${PLATFORM_DIR}/..
        ${PLATFORM_DIR}/ext/target/arm/drivers/counter/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpc_sie
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpu/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/sam
        ${PLATFORM_DIR}/ext/target/arm/drivers/timer/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350
        ${PLATFORM_DIR}/ext/target/arm/drivers/kmu
        ${PLATFORM_DIR}/ext/target/arm/drivers/lcm
        ${PLATFORM_DIR}/ext/target/arm/drivers/gpio/pl061
        ./bringup_helpers
)

target_sources(platform_s
    PRIVATE
        cmsis_drivers/Driver_MPC.c
        cmsis_drivers/Driver_PPC.c
        device/source/device_definition.c
        device/source/system_core_init.c
        $<$<BOOL:${TFM_PARTITION_DPE}>:${CMAKE_CURRENT_SOURCE_DIR}/dpe/dpe_plat.c>
        ${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350/dma350_lib.c
        $<$<BOOL:${RSE_USE_SDS_LIB}>:${CMAKE_CURRENT_SOURCE_DIR}/libraries/sds.c>
        $<$<BOOL:${RSE_USE_SDS_LIB}>:${CMAKE_CURRENT_SOURCE_DIR}/libraries/sds_svc.c>
        native_drivers/atu_rse_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350/dma350_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350/dma350_ch_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/kmu/kmu_drv.c
        native_drivers/sic_drv.c
        $<$<EQUAL:${PLAT_MHU_VERSION},2>:${CMAKE_CURRENT_SOURCE_DIR}/native_drivers/mhu_v2_x.c>
        $<$<EQUAL:${PLAT_MHU_VERSION},2>:${CMAKE_CURRENT_SOURCE_DIR}/native_drivers/mhu_wrapper_v2_x.c>
        $<$<EQUAL:${PLAT_MHU_VERSION},3>:${CMAKE_CURRENT_SOURCE_DIR}/native_drivers/mhu_v3_x.c>
        $<$<EQUAL:${PLAT_MHU_VERSION},3>:${CMAKE_CURRENT_SOURCE_DIR}/native_drivers/mhu_wrapper_v3_x.c>
        native_drivers/ppc_rse_drv.c
        native_drivers/integrity_checker_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/counter/armv8m/syscounter_armv8-m_cntrl_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpc_sie/mpc_sie_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpu/armv8m/mpu_armv8m_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/sam/sam_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/timer/armv8m/systimer_armv8-m_drv.c
        $<$<BOOL:${RSE_DEBUG_UART}>:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_drivers/Driver_USART_cmsdk.c>
        $<$<BOOL:${RSE_DEBUG_UART}>:${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk/uart_cmsdk_drv.c>
        tfm_peripherals_def.c
        $<$<OR:$<BOOL:${TFM_PARTITION_SLIH_TEST}>,$<BOOL:${TFM_PARTITION_FLIH_TEST}>>:${CMAKE_CURRENT_SOURCE_DIR}/plat_test.c>
        $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${CMAKE_CURRENT_SOURCE_DIR}/services/src/tfm_platform_system.c>
        ${PLATFORM_DIR}/ext/target/arm/drivers/lcm/lcm_drv.c
        otp_lcm.c
        nv_counters.c
        attest_hal.c
        sam_interrupts.c
        tfm_hal_platform_reset.c
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${CMAKE_CURRENT_SOURCE_DIR}/platform_svc_handler.c>
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350/dma350_checker_layer.c>
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/dma350_checker_device_defs.c>
        spm_dma_copy.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/gpio/pl061/gpio_pl061_drv.c
        ./platform_fatal_error.c
)

target_link_libraries(platform_s
    PRIVATE
        tfm_sprt # For tfm_strnlen in attest HAL
)

target_sources(tfm_sprt
    PRIVATE
        # SLIH test Partition and FLIH test Partition access the timer as ARoT Partitions.
        # Put the driver to SPRT so that both SLIH and FLIH tests can access it.
        $<$<OR:$<BOOL:${TFM_PARTITION_SLIH_TEST}>,$<BOOL:${TFM_PARTITION_FLIH_TEST}>>:${PLATFORM_DIR}/ext/target/arm/drivers/timer/armv8m/systimer_armv8-m_drv.c>
)

target_compile_definitions(platform_s
    INTERFACE
        ATTEST_KEY_BITS=${ATTEST_KEY_BITS}
        spm_memcpy=spm_dma_memcpy
    PRIVATE
        # Needed for DMA-350 library
        CMSIS_device_header="rse.h"
        $<$<BOOL:${PLATFORM_SVC_HANDLERS}>:PLATFORM_SVC_HANDLERS>
        $<$<BOOL:${RSE_USE_SDS_LIB}>:RSE_USE_SDS_LIB>
)

target_compile_options(platform_s
    PUBLIC
        ${COMPILER_CMSE_FLAG}
)

#========================= Platform Crypto Keys ===============================#

if (TFM_PARTITION_CRYPTO)
    target_sources(platform_crypto_keys
        PRIVATE
            crypto_keys.c
    )

    target_include_directories(platform_crypto_keys
        PUBLIC
            .
            ./native_drivers
            ./device/include
            ./device/config
    )
endif()

#========================= Platform BL2 =======================================#

target_sources(platform_bl2
    PRIVATE
        tfm_hal_platform_reset.c
        device/source/device_definition.c
        device/source/system_core_init.c
        $<$<BOOL:${PLATFORM_HAS_NI_TOWER}>:${CMAKE_CURRENT_SOURCE_DIR}/host_drivers/ni_tower/apu/ni_tower_apu_drv.c>
        $<$<BOOL:${PLATFORM_HAS_NI_TOWER}>:${CMAKE_CURRENT_SOURCE_DIR}/host_drivers/ni_tower/discovery/ni_tower_discovery_drv.c>
        $<$<BOOL:${PLATFORM_HAS_NI_TOWER}>:${CMAKE_CURRENT_SOURCE_DIR}/host_drivers/ni_tower/psam/ni_tower_psam_drv.c>
        $<$<BOOL:${PLATFORM_HAS_NI_TOWER}>:${CMAKE_CURRENT_SOURCE_DIR}/host_drivers/ni_tower/util/ni_tower_util.c>
        $<$<BOOL:${PLATFORM_HAS_NI_TOWER}>:${CMAKE_CURRENT_SOURCE_DIR}/host_drivers/ni_tower/ni_tower_rse_drv.c>
        $<$<BOOL:${PLATFORM_HAS_SMMU_V3}>:${CMAKE_CURRENT_SOURCE_DIR}/host_drivers/smmu_v3/smmu_v3_drv.c>
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350/dma350_lib.c>
        native_drivers/atu_rse_drv.c
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350/dma350_ch_drv.c>
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350/dma350_drv.c>
        ${PLATFORM_DIR}/ext/target/arm/drivers/kmu/kmu_drv.c
        native_drivers/sic_drv.c
        $<$<EQUAL:${PLAT_MHU_VERSION},2>:${CMAKE_CURRENT_SOURCE_DIR}/native_drivers/mhu_v2_x.c>
        $<$<EQUAL:${PLAT_MHU_VERSION},2>:${CMAKE_CURRENT_SOURCE_DIR}/native_drivers/mhu_wrapper_v2_x.c>
        $<$<EQUAL:${PLAT_MHU_VERSION},3>:${CMAKE_CURRENT_SOURCE_DIR}/native_drivers/mhu_v3_x.c>
        $<$<EQUAL:${PLAT_MHU_VERSION},3>:${CMAKE_CURRENT_SOURCE_DIR}/native_drivers/mhu_wrapper_v3_x.c>
        $<$<BOOL:${RSE_DEBUG_UART}>:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_drivers/Driver_USART_cmsdk.c>
        $<$<BOOL:${RSE_DEBUG_UART}>:${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk/uart_cmsdk_drv.c>
        ${PLATFORM_DIR}/ext/target/arm/drivers/lcm/lcm_drv.c
        native_drivers/integrity_checker_drv.c
        otp_lcm.c
        nv_counters.c
        $<$<BOOL:${RSE_XIP}>:${CMAKE_CURRENT_SOURCE_DIR}/bl2/sic_boot.c>
        ${PLATFORM_DIR}/ext/target/arm/drivers/gpio/pl061/gpio_pl061_drv.c
        ./platform_fatal_error.c
)

target_include_directories(platform_bl2
    PUBLIC
        .
        bl2
        cmsis_drivers
        cmsis_drivers/config
        device
        device/config
        device/include
        device/source/armclang
        $<$<BOOL:${PLATFORM_HAS_NI_TOWER}>:${CMAKE_CURRENT_SOURCE_DIR}/host_drivers/ni_tower>
        $<$<BOOL:${PLATFORM_HAS_SMMU_V3}>:${CMAKE_CURRENT_SOURCE_DIR}/host_drivers/smmu_v3>
        native_drivers
        partition
        services/src
        libraries
        ${PLATFORM_DIR}/ext/target/arm/drivers/counter/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpc_sie
        ${PLATFORM_DIR}/ext/target/arm/drivers/sam
        ${PLATFORM_DIR}/ext/target/arm/drivers/timer/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350
        ${PLATFORM_DIR}/ext/target/arm/drivers/kmu
        ${PLATFORM_DIR}/ext/target/arm/drivers/lcm
        ${PLATFORM_DIR}/ext/target/arm/drivers/gpio/pl061
        ./bringup_helpers
    PRIVATE
        ${PLATFORM_DIR}/..
)

target_compile_definitions(platform_bl2
    PRIVATE
        $<$<BOOL:${TFM_PARTITION_DELEGATED_ATTESTATION}>:RSE_BOOT_KEYS_CCA>
        $<$<BOOL:${TFM_PARTITION_DPE}>:RSE_BOOT_KEYS_DPE>
        # Needed for DMA-350 library
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:PLATFORM_HAS_BOOT_DMA>
        $<$<BOOL:${PLATFORM_BOOT_DMA_MIN_SIZE_REQ}>:BOOT_DMA_MIN_SIZE_REQ=${PLATFORM_BOOT_DMA_MIN_SIZE_REQ}>
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:CMSIS_device_header="rse.h">
        $<$<BOOL:${PLATFORM_HAS_NI_TOWER}>:PLATFORM_HAS_NI_TOWER>
        $<$<BOOL:${PLATFORM_HAS_SMMU_V3}>:PLATFORM_HAS_SMMU_V3>
)

target_compile_options(platform_bl2
    PUBLIC
        $<$<BOOL:${PLATFORM_HAS_BOOT_DMA}>:${COMPILER_CMSE_FLAG}>
)

#========================= Platform bl1_1 =======================================#

target_sources(platform_bl1_1
    PRIVATE
        ./bl1/boot_hal_bl1_1.c
        ./device/source/device_definition.c
        ./device/source/system_core_init.c
        ./native_drivers/atu_rse_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/kmu/kmu_drv.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/lcm/lcm_drv.c
        $<$<EQUAL:${PLAT_MHU_VERSION},2>:${CMAKE_CURRENT_SOURCE_DIR}/native_drivers/mhu_v2_x.c>
        $<$<EQUAL:${PLAT_MHU_VERSION},2>:${CMAKE_CURRENT_SOURCE_DIR}/native_drivers/mhu_wrapper_v2_x.c>
        $<$<EQUAL:${PLAT_MHU_VERSION},3>:${CMAKE_CURRENT_SOURCE_DIR}/native_drivers/mhu_v3_x.c>
        $<$<EQUAL:${PLAT_MHU_VERSION},3>:${CMAKE_CURRENT_SOURCE_DIR}/native_drivers/mhu_wrapper_v3_x.c>
        ./native_drivers/tram_drv.c
        ./native_drivers/integrity_checker_drv.c
        ./nv_counters.c
        ./otp_lcm.c
        ./tfm_hal_platform_reset.c
        $<$<BOOL:${RSE_DEBUG_UART}>:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_drivers/Driver_USART_cmsdk.c>
        $<$<BOOL:${RSE_DEBUG_UART}>:${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk/uart_cmsdk_drv.c>
        ./dpa_hardened_word_copy.c
        ./cc312/cc3xx_aes_external_key_loader.c
        $<$<BOOL:${RSE_ENABLE_BRINGUP_HELPERS}>:${CMAKE_CURRENT_SOURCE_DIR}/bringup_helpers/rse_bringup_helpers.c>
        $<$<BOOL:${RSE_BRINGUP_OTP_EMULATION}>:${CMAKE_CURRENT_SOURCE_DIR}/bringup_helpers/rse_bl1_2_image_otp_or_flash.c>
        ./rse_key_derivation.c
        $<$<BOOL:${RSE_SUPPORT_ROM_LIB_RELOCATION}>:${CMAKE_CURRENT_SOURCE_DIR}/bl1/bl1_patch_stubs.c>
        ${PLATFORM_DIR}/ext/target/arm/drivers/gpio/pl061/gpio_pl061_drv.c
        ./platform_fatal_error.c
)

target_include_directories(platform_bl1_1_interface
    INTERFACE
        .
        ./cmsis_drivers
        ./cmsis_drivers/config
        ./device/config
        ./device/include
        ./native_drivers
        ${PLATFORM_DIR}/ext/target/arm/drivers/counter/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpc_sie
        ${PLATFORM_DIR}/ext/target/arm/drivers/sam
        ${PLATFORM_DIR}/ext/target/arm/drivers/timer/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk
        ${PLATFORM_DIR}/ext/target/arm/drivers/usart/common
        ${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350
        ${PLATFORM_DIR}/ext/target/arm/drivers/kmu
        ${PLATFORM_DIR}/ext/target/arm/drivers/lcm
        ${PLATFORM_DIR}/ext/accelerator/cc312/cc312-rom
        ${PLATFORM_DIR}/ext/target/arm/drivers/gpio/pl061
        ./bringup_helpers
)

target_compile_definitions(platform_bl1_1_interface
    INTERFACE
        $<$<BOOL:${TFM_PARTITION_DELEGATED_ATTESTATION}>:RSE_BOOT_KEYS_CCA>
        $<$<BOOL:${TFM_PARTITION_DPE}>:RSE_BOOT_KEYS_DPE>
        KMU_CONFIG_EXTERNAL_SECURE_WORD_COPY
        $<$<BOOL:${RSE_ENCRYPTED_OTP_KEYS}>:RSE_ENCRYPTED_OTP_KEYS>
)

target_link_libraries(platform_bl1_1
    PRIVATE
        cc312_rom
        bl1_1_shared_lib_interface
)

if(RSE_SUPPORT_ROM_LIB_RELOCATION)
    target_compile_options(platform_bl1_1
        PUBLIC
            # Enable position independent code
            -fpic
            # Tell the compiler that it can't assume the offset between the data
            # and code sections doesn't change
            -mno-pic-data-is-text-relative
            # Set r9 as the register to use for the GOT. This register will not
            # be used for any other purpose. If this is changed, some of the ASM
            # setup functions will need to be changed.
            -mpic-register=r9
            # Tell the compiler it can assume that r9 will always contain the
            # address of the GOT, and that it doesn't need to try to set r9
            # itself (we do it in the initial setup).
            -msingle-pic-base
            # Insert stub function calls into every function which can then be
            # replaced with calls to patched versions of the function.
            -finstrument-functions
            # Don't insert stub function into the stub functions, or the early
            # startup files where we are still setting up the GOT
            -finstrument-functions-exclude-file-list=bl1_patch_stubs,startup_rse_bl1_1
            # Don't insert stub function into functions where the stack isn't
            # valid.
            -finstrument-functions-exclude-function-list=boot_platform_quit,boot_jump_to_next_image
    )
endif()

if (RSE_USE_ROM_LIB_FROM_SRAM)
    add_custom_target(bl1_1_shared_symbols_in_sram
        DEPENDS bl1_1
        SOURCES ${CMAKE_BINARY_DIR}/bin/bl1_1${CODE_SHARING_INPUT_FILE_SUFFIX}
    )

    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/bin/bl1_1${CODE_SHARING_INPUT_FILE_SUFFIX}
        DEPENDS ${CMAKE_BINARY_DIR}/bin/bl1_1${CODE_SHARING_OUTPUT_FILE_SUFFIX}
        COMMAND ${CMAKE_OBJCOPY} ${CMAKE_BINARY_DIR}/bin/bl1_1${CODE_SHARING_OUTPUT_FILE_SUFFIX}
                --change-section-address .text\*+0x20010000
                ${CMAKE_BINARY_DIR}/bin/bl1_1${CODE_SHARING_INPUT_FILE_SUFFIX}
    )

    add_dependencies(bl1_2
        bl1_1_shared_symbols_in_sram
    )
endif()

target_sources(bl1_1_shared_lib
    PRIVATE
        $<$<NOT:$<BOOL:${TFM_BL1_SOFTWARE_CRYPTO}>>:${CMAKE_CURRENT_SOURCE_DIR}/bl1/cc312_rom_crypto.c>
        $<$<AND:$<NOT:$<BOOL:${RSE_OTP_TRNG}>>,$<NOT:$<BOOL:${TFM_BL1_DUMMY_TRNG}>>>:${CMAKE_CURRENT_SOURCE_DIR}/bl1/cc312_rom_trng.c>
        $<$<AND:$<BOOL:${RSE_OTP_TRNG}>,$<NOT:$<BOOL:${TFM_BL1_DUMMY_TRNG}>>>:${CMAKE_CURRENT_SOURCE_DIR}/rse_trng.c>
)

target_link_libraries(cc312_rom
    PRIVATE
        platform_bl1_1
)

#========================= Platform BL1_2 =====================================#

target_sources(platform_bl1_2
    PRIVATE
        device/source/system_core_init.c
        ./bl1/boot_hal_bl1_2.c
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpu/armv8m/mpu_armv8m_drv.c
        $<$<BOOL:${PLATFORM_PSA_ADAC_SECURE_DEBUG}>:${PLATFORM_DIR}/ext/target/arm/drivers/lcm/lcm_drv.c>
        $<$<BOOL:${PLATFORM_PSA_ADAC_SECURE_DEBUG}>:${CMAKE_CURRENT_SOURCE_DIR}/device/source/device_definition.c>
        $<$<BOOL:${PLATFORM_PSA_ADAC_SECURE_DEBUG}>:${CMAKE_CURRENT_SOURCE_DIR}/platform_dcu.c>
)

target_include_directories(platform_bl1_2
    PRIVATE
        .
        ${PLATFORM_DIR}/ext/target/arm/drivers/mpu/armv8m
        ${PLATFORM_DIR}/ext/target/arm/drivers/dma/dma350
        ${PLATFORM_DIR}/ext/target/arm/drivers/kmu
        ${PLATFORM_DIR}/ext/target/arm/drivers/lcm
)

target_compile_definitions(platform_bl1_2
    PUBLIC
        $<$<BOOL:${TFM_PARTITION_DELEGATED_ATTESTATION}>:RSE_BOOT_KEYS_CCA>
        $<$<BOOL:${TFM_PARTITION_DPE}>:RSE_BOOT_KEYS_DPE>
        $<$<BOOL:${PLATFORM_PSA_ADAC_SECURE_DEBUG}>:PLATFORM_PSA_ADAC_SECURE_DEBUG>
)

if(RSE_USE_HOST_FLASH)
    add_subdirectory(./fip_parser)
endif()

if(RSE_SUPPORT_ROM_LIB_RELOCATION)
    target_compile_options(platform_bl1_2
        PUBLIC
            # Prevent BL1_2 from using r9
            -ffixed-reg=r9
            # Inlining causes gcc to generate reads and writes to r9. This is
            # probably a bug, but work around it by not inlining functions.
            -fno-inline
    )
endif()

#========================= ns_agent_mailbox ===================================#

if(TFM_MULTI_CORE_TOPOLOGY)
    target_sources(tfm_psa_rot_partition_ns_agent_mailbox
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/tfm_hal_multi_core.c
    )
endif()

#========================= tfm_spm ============================================#

target_sources(tfm_spm
    PRIVATE
        target_cfg.c
        ${PLATFORM_DIR}/ext/common/tfm_hal_isolation_v8m.c
        tfm_hal_platform.c
        tfm_interrupts.c
        faults.c
)

#=========================== CPAK generator tool ===============================

find_package(Python3)

add_custom_target(cpak_generator
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cpak_generator/cpak_generator.c
)

add_dependencies(cpak_generator
    bl1_2_padded_bin
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_generator
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cpak_generator/cpak_generator.c
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator
    COMMAND ${CMAKE_COMMAND} -S ${CMAKE_CURRENT_SOURCE_DIR}/cpak_generator
                             -B ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator
                             -G ${CMAKE_GENERATOR}
                             -DMBEDCRYPTO_PATH=${MBEDCRYPTO_PATH}
                             -DBL1_2_HASH_PATH=\\\"$<TARGET_FILE_DIR:bl1_2>/bl1_2_padded_hash.bin\\\"
                             -DGUK_PATH=\\\"${TFM_GUK_PATH}\\\"
                             -DCPAK_OUTPUT_PATH=\\\"${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_public.bin\\\"
                             -DRSE_TP_MODE=${RSE_TP_MODE}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator
)

add_custom_target(cpak_public ALL
    DEPENDS ${TFM_GUK_PATH}
    DEPENDS cpak_generator
    DEPENDS ${CMAKE_BINARY_DIR}/bin/bl1_2_padded_hash.bin
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_generator
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cpak_generator/bin_to_pem.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_generator
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/cpak_generator/bin_to_pem.py
                    --input_key_file ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_public.bin
                    --output_key_file ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_public.pem
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_public.bin $<TARGET_FILE_DIR:bl1_1>
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/cpak_generator/cpak_public.pem $<TARGET_FILE_DIR:bl1_1>
)

############################### IMAGE SIGNING ##################################

if (RSE_XIP)
    find_package(Python3)

    add_custom_target(public_key ALL
        SOURCES
            image_s_signing_public_key.pem
            image_ns_signing_public_key.pem
    )
    add_custom_command(OUTPUT image_s_signing_public_key.pem
        # If MCUBOOT_GENERATE_SIGNING_KEYPAIR=ON then the command should depend on that
        # to make sure that the key is generated and copied to the MCUBOOT_KEY_S path.
        DEPENDS $<IF:$<BOOL:${MCUBOOT_GENERATE_SIGNING_KEYPAIR}>,generated_private_key,>
        COMMAND ${Python3_EXECUTABLE} ${MCUBOOT_PATH}/scripts/imgtool.py
            getpub
            -k ${MCUBOOT_KEY_S}
            -e pem
            -o image_s_signing_public_key.pem
        COMMAND ${CMAKE_COMMAND} -E copy image_s_signing_public_key.pem $<TARGET_FILE_DIR:bl2>
    )
    add_custom_command(OUTPUT image_ns_signing_public_key.pem
        # If MCUBOOT_GENERATE_SIGNING_KEYPAIR=ON then the command should depend on that
        # to make sure that the key is generated and copied to the MCUBOOT_KEY_NS path.
        DEPENDS $<IF:$<BOOL:${MCUBOOT_GENERATE_SIGNING_KEYPAIR}>,generated_private_key,>
        COMMAND ${Python3_EXECUTABLE} ${MCUBOOT_PATH}/scripts/imgtool.py
            getpub
            -k ${MCUBOOT_KEY_NS}
            -e pem
            -o image_ns_signing_public_key.pem
        COMMAND ${CMAKE_COMMAND} -E copy image_ns_signing_public_key.pem $<TARGET_FILE_DIR:bl2>
    )

    add_custom_target(tfm_s_sic_tables
        SOURCES bin/tfm_s_sic_tables.bin
    )

    add_custom_command(OUTPUT bin/tfm_s_sic_tables.bin
        DEPENDS $<TARGET_FILE_DIR:tfm_s>/tfm_s.bin
        DEPENDS tfm_s_bin
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bl2/create_xip_tables.py
            --input_image $<TARGET_FILE_DIR:tfm_s>/tfm_s.bin
            --table_output_file tfm_s_sic_tables.bin
            --encrypted_image_output_file tfm_s_encrypted.bin
            --image_version ${MCUBOOT_SECURITY_COUNTER_S}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tfm_s_sic_tables.bin $<TARGET_FILE_DIR:tfm_s>
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tfm_s_encrypted.bin $<TARGET_FILE_DIR:tfm_s>
    )

    set(IMAGE_TYPE "S_IMAGE")
    set(FLASH_AREA_NUM ${MCUBOOT_S_IMAGE_FLASH_AREA_NUM})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bl2/signing_layout_sic_tables.c.in signing_layout_s.c @ONLY)
    add_library(signing_layout_s OBJECT ${CMAKE_CURRENT_BINARY_DIR}/signing_layout_s.c)

    target_compile_options(signing_layout_s
        PRIVATE
            $<$<C_COMPILER_ID:GNU>:-E\;-xc>
            $<$<C_COMPILER_ID:ARMClang>:-E\;-xc>
            $<$<C_COMPILER_ID:IAR>:--preprocess=ns\;$<TARGET_OBJECTS:signing_layout_s>>
    )
    target_compile_definitions(signing_layout_s
        PRIVATE
            $<$<BOOL:${BL2}>:BL2>
            $<$<BOOL:${MCUBOOT_IMAGE_NUMBER}>:MCUBOOT_IMAGE_NUMBER=${MCUBOOT_IMAGE_NUMBER}>
            $<$<STREQUAL:${MCUBOOT_UPGRADE_STRATEGY},DIRECT_XIP>:IMAGE_ROM_FIXED>
    )
    target_link_libraries(signing_layout_s
        PRIVATE
            platform_bl2
    )

    add_custom_target(tfm_s_sic_tables_signed_bin
        SOURCES bin/tfm_s_sic_tables_signed.bin
    )
    add_custom_command(OUTPUT bin/tfm_s_sic_tables_signed.bin
        DEPENDS tfm_s_sic_tables
        DEPENDS tfm_s_bin signing_layout_s
        WORKING_DIRECTORY ${MCUBOOT_PATH}/scripts

        #Sign secure binary image with provided secret key
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/bl2/ext/mcuboot/scripts/wrapper/wrapper.py
            -v ${MCUBOOT_IMAGE_VERSION_S}
            --layout $<TARGET_OBJECTS:signing_layout_s>
            -k ${MCUBOOT_KEY_S}
            --public-key-format $<IF:$<BOOL:${MCUBOOT_HW_KEY}>,full,hash>
            --align ${MCUBOOT_ALIGN_VAL}
            --pad
            --pad-header
            -H ${BL2_HEADER_SIZE}
            -s ${MCUBOOT_SECURITY_COUNTER_S}
            -L ${MCUBOOT_ENC_KEY_LEN}
            -d \"\(1,${MCUBOOT_NS_IMAGE_MIN_VER}\)\"
            $<$<STREQUAL:${MCUBOOT_UPGRADE_STRATEGY},OVERWRITE_ONLY>:--overwrite-only>
            $<$<BOOL:${MCUBOOT_ENC_IMAGES}>:-E${MCUBOOT_KEY_ENC}>
            $<$<BOOL:${MCUBOOT_MEASURED_BOOT}>:--measured-boot-record>
            $<TARGET_FILE_DIR:tfm_s>/tfm_s_sic_tables.bin
            ${CMAKE_CURRENT_BINARY_DIR}/tfm_s_sic_tables_signed.bin
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tfm_s_sic_tables_signed.bin $<TARGET_FILE_DIR:bl2>
    )

    add_custom_target(signed_images
        ALL
        DEPENDS bin/tfm_s_sic_tables_signed.bin
    )

    set(IMAGE_TYPE "NS_IMAGE")
    set(FLASH_AREA_NUM ${MCUBOOT_NS_IMAGE_FLASH_AREA_NUM})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bl2/signing_layout_sic_tables.c.in
                   ${CMAKE_CURRENT_BINARY_DIR}/signing_layout_sic_tables_ns.c @ONLY)

    install(FILES       ${CMAKE_CURRENT_BINARY_DIR}/signing_layout_sic_tables_ns.c
            DESTINATION ${INSTALL_IMAGE_SIGNING_DIR}/layout_files)

    install(FILES       bl2/create_xip_tables.py
            DESTINATION ${INSTALL_IMAGE_SIGNING_DIR}/scripts)

    install(DIRECTORY ${CMAKE_SOURCE_DIR}/bl2/ext/mcuboot/scripts
            DESTINATION ${INSTALL_IMAGE_SIGNING_DIR}
            PATTERN "scripts/*.py"
            PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
            GROUP_EXECUTE GROUP_READ
            PATTERN "scripts/wrapper/*.py"
            PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                        GROUP_EXECUTE GROUP_READ)

    install(DIRECTORY ${MCUBOOT_PATH}/scripts/imgtool
            DESTINATION ${INSTALL_IMAGE_SIGNING_DIR}/scripts)

    install(FILES ${MCUBOOT_KEY_NS}
            RENAME image_ns_signing_private_key.pem
            DESTINATION ${INSTALL_IMAGE_SIGNING_DIR}/keys/)
    install(FILES $<TARGET_FILE_DIR:bl2>/image_ns_signing_public_key.pem
            DESTINATION ${INSTALL_IMAGE_SIGNING_DIR}/keys)

    if (MCUBOOT_ENC_IMAGES)
        install(FILES ${MCUBOOT_KEY_ENC}
                RENAME image_enc_key.pem
                DESTINATION ${INSTALL_IMAGE_SIGNING_DIR}/keys)
    endif()
endif()

############################## BL1 DMA ###################################
set(bin_dir ${CMAKE_BINARY_DIR}/bin)

add_custom_target(dma_icses
    ALL
    SOURCES rom_dma_ics.bin ${bin_dir}/rom_dma_ics.bin
            rom_dma_ics.hex ${bin_dir}/rom_dma_ics.hex
            otp_dma_ics.bin ${bin_dir}/otp_dma_ics.bin
            otp_dma_ics.hex ${bin_dir}/otp_dma_ics.hex
)

add_custom_command(
    OUTPUT rom_dma_ics.bin
    OUTPUT otp_dma_ics.bin
    OUTPUT rom_dma_ics.hex
    OUTPUT otp_dma_ics.hex
    OUTPUT ${bin_dir}/rom_dma_ics.bin
    OUTPUT ${bin_dir}/otp_dma_ics.bin
    OUTPUT ${bin_dir}/rom_dma_ics.hex
    OUTPUT ${bin_dir}/otp_dma_ics.hex
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/bl1/scripts/create_bl1_1_dma_bin.py
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bl1/scripts/create_bl1_1_dma_bin.py
                    --input_file ${CMAKE_CURRENT_SOURCE_DIR}/bl1/scripts/dma_config.yaml
                    --output_dir ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/rom_dma_ics.bin $<TARGET_FILE_DIR:bl1_1>
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/otp_dma_ics.bin $<TARGET_FILE_DIR:bl1_1>
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/rom_dma_ics.hex $<TARGET_FILE_DIR:bl1_1>
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/otp_dma_ics.hex $<TARGET_FILE_DIR:bl1_1>
)

#========================= Files for building NS side platform ================#

install(FILES       ${PLATFORM_DIR}/ext/driver/Driver_Common.h
                    ${PLATFORM_DIR}/ext/driver/Driver_USART.h
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/ext/driver)

install(FILES       device/source/device_definition.c
                    device/source/startup_rse.c
                    device/source/system_core_init.c
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/device/source
)

install(FILES       ${PLATFORM_DIR}/ext/common/uart_stdout.c
                    $<$<BOOL:${RSE_DEBUG_UART}>:${PLATFORM_DIR}/ext/target/arm/drivers/usart/cmsdk/uart_cmsdk_drv.c>
                    $<$<BOOL:${RSE_DEBUG_UART}>:${CMAKE_CURRENT_SOURCE_DIR}/cmsis_drivers/Driver_USART_cmsdk.c>
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}
)

install(DIRECTORY native_drivers
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}
)

install(FILES       ${PLATFORM_DIR}/ext/target/arm/drivers/counter/armv8m/syscounter_armv8-m_cntrl_drv.c
                    ${PLATFORM_DIR}/ext/target/arm/drivers/counter/armv8m/syscounter_armv8-m_cntrl_drv.h
                    ${PLATFORM_DIR}/ext/target/arm/drivers/mpc_sie/mpc_sie_drv.c
                    ${PLATFORM_DIR}/ext/target/arm/drivers/mpu/armv8m/mpu_armv8m_drv.c
                    ${PLATFORM_DIR}/ext/target/arm/drivers/timer/armv8m/systimer_armv8-m_drv.c
                    ${PLATFORM_DIR}/ext/target/arm/drivers/gpio/pl061/gpio_pl061_drv.c
                    ${PLATFORM_DIR}/ext/target/arm/drivers/mpc_sie/mpc_sie_drv.h
                    ${PLATFORM_DIR}/ext/target/arm/drivers/mpu/armv8m/mpu_armv8m_drv.h
                    ${PLATFORM_DIR}/ext/target/arm/drivers/timer/armv8m/systimer_armv8-m_drv.h
                    ${PLATFORM_DIR}/ext/target/arm/drivers/gpio/pl061/gpio_pl061_drv.h
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/native_drivers
)

install(FILES       ${PLATFORM_DIR}/ext/accelerator/cc312/crypto_accelerator_config.h
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/include
)

install(FILES       device/config/device_cfg_ns.h
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/device/config
        RENAME      device_cfg.h)

install(DIRECTORY   device/include
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/device
)

install(DIRECTORY   cmsis_drivers
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}
)

install(DIRECTORY   partition
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}
)

install(FILES       target_cfg.h
                    tfm_peripherals_def.h
                    ${PLATFORM_DIR}/include/tfm_plat_defs.h
                    ${PLATFORM_DIR}/ext/common/uart_stdout.h
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/include)

install(DIRECTORY   ns/
        DESTINATION ${INSTALL_PLATFORM_NS_DIR})

install(FILES       cpuarch.cmake
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/config/common)

install(FILES       tests/tfm_tests_config.cmake
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/tests/common)

install(FILES       check_config.cmake
        DESTINATION ${INSTALL_PLATFORM_NS_DIR})

# Install linker scripts
install(FILES       ${PLATFORM_DIR}/ext/common/gcc/tfm_common_ns.ld
                    ${PLATFORM_DIR}/ext/common/armclang/tfm_common_ns.sct
                    ${PLATFORM_DIR}/ext/common/iar/tfm_common_ns.icf
        DESTINATION ${INSTALL_PLATFORM_NS_DIR}/linker_scripts)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ns/config.cmake.in
               ${INSTALL_PLATFORM_NS_DIR}/config.cmake @ONLY)
