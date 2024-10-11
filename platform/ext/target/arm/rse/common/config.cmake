#-------------------------------------------------------------------------------
# SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(RSE_XIP                             OFF        CACHE BOOL     "Whether to run runtime firmware XIP via the SIC")

if (RSE_XIP)
    set(PLATFORM_DEFAULT_IMAGE_SIGNING      OFF        CACHE BOOL     "Use default image signing implementation")

    set(MCUBOOT_S_IMAGE_FLASH_AREA_NUM      10         CACHE STRING   "ID of the flash area containing the primary Secure image")
    set(MCUBOOT_NS_IMAGE_FLASH_AREA_NUM     11         CACHE STRING   "ID of the flash area containing the primary Non-Secure image")
endif()

set(RSE_USE_HOST_UART                   ON         CACHE BOOL     "Whether RSE should setup to use the UART from the host system")
set(RSE_USE_LOCAL_UART                  OFF        CACHE BOOL     "Whether RSE should setup to use the UART local to the RSE subsystem")
set(RSE_HAS_EXPANSION_PERIPHERALS       OFF        CACHE BOOL     "Whether RSE has sub-platform specific peripherals in the expansion layer")

set(CRYPTO_HW_ACCELERATOR               ON         CACHE BOOL     "Whether to enable the crypto hardware accelerator on supported platforms")
set(CC312_LEGACY_DRIVER_API_ENABLED     OFF        CACHE BOOL     "Whether the legacy mbedtls accelerator API is used")
set(CC3XX_RUNTIME_ENABLED               ON         CACHE BOOL     "Whether the CC3XX driver is used at runtime")

set(PLATFORM_DEFAULT_OTP                OFF        CACHE BOOL     "Use trusted on-chip flash to implement OTP memory")
set(PLATFORM_DEFAULT_CRYPTO_KEYS        OFF        CACHE BOOL     "Use default crypto keys implementation.")
set(PLATFORM_DEFAULT_ROTPK              OFF        CACHE BOOL      "Use default root of trust public key.")
set(PLATFORM_DEFAULT_PROVISIONING       OFF        CACHE BOOL     "Use default provisioning implementation")
set(TFM_BL1_DEFAULT_PROVISIONING        OFF        CACHE BOOL     "Whether BL1_1 will use default provisioning")
set(TFM_BL1_DEFAULT_OTP                 OFF        CACHE BOOL     "Whether BL1_1 will use default OTP memory")
set(PLATFORM_DEFAULT_NV_SEED            OFF        CACHE BOOL     "Use default NV seed implementation.")
set(PLATFORM_DEFAULT_NV_COUNTERS        OFF        CACHE BOOL     "Use default nv counter implementation.")
set(PLATFORM_DEFAULT_ATTEST_HAL         OFF        CACHE BOOL     "Use default attest hal implementation.")
set(PLATFORM_DEFAULT_SYSTEM_RESET_HALT  OFF        CACHE BOOL     "Use default system reset/halt implementation")
set(PLATFORM_HAS_BOOT_DMA               ON         CACHE BOOL     "Enable dma support for memory transactions for bootloader")
set(PLATFORM_BOOT_DMA_MIN_SIZE_REQ      0x40       CACHE STRING   "Minimum transaction size (in bytes) required to enable dma support for bootloader")
set(PLATFORM_SVC_HANDLERS               ON         CACHE BOOL     "Platform supports custom SVC handlers")
set(PLATFORM_ERROR_CODES                ON         CACHE BOOL     "Whether to use platform-specific error codes.")

set(BL1                                 ON         CACHE BOOL     "Whether to build BL1")
set(PLATFORM_DEFAULT_BL1                ON         CACHE STRING   "Whether to use default BL1 or platform-specific one")
set(TFM_BL1_SOFTWARE_CRYPTO             OFF        CACHE BOOL     "Whether BL1_1 will use software crypto")
set(TFM_BL1_MEMORY_MAPPED_FLASH         ON         CACHE BOOL     "Whether BL1 can directly access flash content")
set(TFM_BL1_PQ_CRYPTO                   ON         CACHE BOOL     "Enable LMS PQ crypto for BL2 verification. This is experimental and should not yet be used in production")
set(TFM_BL1_DUMMY_TRNG                  OFF        CACHE BOOL     "Whether BL1_1 will use dummy TRNG")
if (RSE_BRINGUP_OTP_EMULATION)
    set(TFM_BL1_2_IN_OTP                FALSE      CACHE BOOL      "Whether BL1_2 is stored in OTP")
endif()

set(BL2                                 ON         CACHE BOOL     "Whether to build BL2")
set(MCUBOOT_UPGRADE_STRATEGY            "RAM_LOAD" CACHE STRING   "Upgrade strategy when multiple boot images are loaded [OVERWRITE_ONLY, SWAP, DIRECT_XIP, RAM_LOAD]")
set(MCUBOOT_IMAGE_NUMBER                4          CACHE STRING   "Number of images supported by MCUBoot")
set(DEFAULT_MCUBOOT_FLASH_MAP           OFF        CACHE BOOL     "Whether to use the default flash map defined by TF-M project")
set(MCUBOOT_S_IMAGE_FLASH_AREA_NUM      2          CACHE STRING   "ID of the flash area containing the primary Secure image")
set(MCUBOOT_NS_IMAGE_FLASH_AREA_NUM     3          CACHE STRING   "ID of the flash area containing the primary Non-Secure image")
set(RSE_USE_HOST_FLASH                  ON         CACHE BOOL     "Enable RSE using the host flash.")
set(RSE_LOAD_NS_IMAGE                   ON         CACHE BOOL     "Whether to load an RSE NSPE image")
set(RSE_BL2_ENABLE_IMAGE_STAGING        OFF        CACHE BOOL     "Whether to enable staging of the images to be loaded by BL2")

set(TFM_PARTITION_CRYPTO                ON         CACHE BOOL     "Enable Crypto partition")
set(TFM_PARTITION_PROTECTED_STORAGE     OFF        CACHE BOOL     "Enable Protected Storage partition")
set(TFM_PARTITION_INTERNAL_TRUSTED_STORAGE OFF     CACHE BOOL     "Enable Internal Trusted Storage partition")
set(TFM_PARTITION_PLATFORM              ON         CACHE BOOL     "Enable Platform partition")
set(TFM_PARTITION_SCMI_COMMS            OFF        CACHE BOOL     "Enable SCMI Comms partition")

set(MEASURED_BOOT_HASH_ALG              PSA_ALG_SHA_256 CACHE STRING "Hash algorithm used by Measured boot services")
set(TFM_MBEDCRYPTO_PLATFORM_EXTRA_CONFIG_PATH ${CMAKE_CURRENT_LIST_DIR}/mbedtls_extra_config.h CACHE PATH "Config to append to standard Mbed Crypto config, used by platforms to configure feature support")

set(TFM_ATTESTATION_SCHEME              "PSA"      CACHE STRING   "Attestation scheme to use [OFF, PSA, CCA, DPE]")

set(TFM_EXTRAS_REPO_PATH                "DOWNLOAD"  CACHE PATH    "Path to tf-m-extras repo (or DOWNLOAD to fetch automatically")
set(TFM_EXTRAS_REPO_VERSION             "9076ca6"   CACHE STRING  "The version of tf-m-extras to use")
set(TFM_EXTRAS_REPO_EXTRA_PARTITIONS    "measured_boot;delegated_attestation;dice_protection_environment;scmi;adac" CACHE STRING "List of extra secure partition directory name(s)")
# Below TFM_EXTRAS_REPO_EXTRA_MANIFEST_LIST path is relative to tf-m-extras repo
set(TFM_EXTRAS_REPO_EXTRA_MANIFEST_LIST "partitions/measured_boot/measured_boot_manifest_list.yaml;partitions/delegated_attestation/delegated_attestation_manifest_list.yaml;partitions/dice_protection_environment/dpe_manifest_list.yaml;partitions/adac/adac_manifest_list.yaml;" CACHE STRING "List of extra secure partition manifests")

if (TEST_S_SCMI_COMMS)
    list(APPEND TFM_EXTRAS_REPO_EXTRA_MANIFEST_LIST "partitions/scmi/test/secure/scmi_comms_manifest_list.yaml")
else()
    list(APPEND TFM_EXTRAS_REPO_EXTRA_MANIFEST_LIST "partitions/scmi/scmi_comms_manifest_list.yaml")
endif()

set(ATTEST_KEY_BITS                     384        CACHE STRING   "The size of the initial attestation key in bits")
set(PSA_INITIAL_ATTEST_MAX_TOKEN_SIZE   0x800      CACHE STRING    "The maximum possible size of a token")

set(TFM_NS_NV_COUNTER_AMOUNT            3          CACHE STRING   "How many NS NV counters are enabled")

set(TFM_PLAT_SPECIFIC_MULTI_CORE_COMM   ON         CACHE BOOL     "Whether to use a platform specific inter-core communication instead of mailbox in dual-cpu topology")

set(PLATFORM_HAS_ISOLATION_L3_SUPPORT   ON)
set(TFM_ISOLATION_LEVEL                 2          CACHE STRING   "Isolation level")
set(TFM_PXN_ENABLE                      ON         CACHE BOOL     "Use Privileged execute never (PXN)")

set(TFM_MANIFEST_LIST                   "${CMAKE_CURRENT_LIST_DIR}/manifest/tfm_manifest_list.yaml" CACHE PATH "Platform specific Secure Partition manifests file")

if (PLATFORM_PSA_ADAC_SECURE_DEBUG)
    set(TFM_PARTITION_ADAC                          ON      CACHE BOOL  "Enable Authenticatd Debug partition")
endif()

# Platform-specific configurations
if (RSE_LOAD_NS_IMAGE)
    set(CONFIG_TFM_USE_TRUSTZONE            ON)
else()
    set(CONFIG_TFM_USE_TRUSTZONE            OFF)
endif()

if (TFM_ATTESTATION_SCHEME      STREQUAL "PSA")
    set(TFM_PARTITION_INITIAL_ATTESTATION   ON      CACHE BOOL  "Enable Initial Attestation partition")
    set(TFM_PARTITION_DELEGATED_ATTESTATION OFF     CACHE BOOL  "Enable Delegated Attestation partition")
    set(TFM_PARTITION_DPE                   OFF     CACHE BOOL  "Enable DICE Protection Environment partition")
    set(TFM_PARTITION_MEASURED_BOOT         OFF     CACHE BOOL  "Enable Measured Boot partition")
elseif (TFM_ATTESTATION_SCHEME  STREQUAL "CCA")
    set(TFM_PARTITION_INITIAL_ATTESTATION   ON      CACHE BOOL  "Enable Initial Attestation partition")
    set(TFM_PARTITION_DELEGATED_ATTESTATION ON      CACHE BOOL  "Enable Delegated Attestation partition")
    set(TFM_PARTITION_DPE                   OFF     CACHE BOOL  "Enable DICE Protection Environment partition")
    set(TFM_PARTITION_MEASURED_BOOT         ON      CACHE BOOL  "Enable Measured Boot partition")
elseif (TFM_ATTESTATION_SCHEME  STREQUAL "DPE")
    set(TFM_PARTITION_INITIAL_ATTESTATION   OFF     CACHE BOOL  "Enable Initial Attestation partition")
    set(TFM_PARTITION_DELEGATED_ATTESTATION OFF     CACHE BOOL  "Enable Delegated Attestation partition")
    set(TFM_PARTITION_DPE                   ON      CACHE BOOL  "Enable DICE Protection Environment partition")
    set(TFM_PARTITION_MEASURED_BOOT         OFF     CACHE BOOL  "Enable Measured Boot partition")
    set(RSE_USE_SDS_LIB                     ON)
    # Temporarily only 256-bit IAK is allowed when DPE is enabled
    set(ATTEST_KEY_BITS                     256     CACHE STRING    "The size of the initial attestation key in bits" FORCE)
else()
    # Disable attestation
    set(TFM_PARTITION_INITIAL_ATTESTATION   OFF     CACHE BOOL  "Enable Initial Attestation partition")
    set(TFM_PARTITION_DELEGATED_ATTESTATION OFF     CACHE BOOL  "Enable Delegated Attestation partition")
    set(TFM_PARTITION_DPE                   OFF     CACHE BOOL  "Enable DICE Protection Environment partition")
    set(TFM_PARTITION_MEASURED_BOOT         OFF     CACHE BOOL  "Enable Measured Boot partition")
    set(CONFIG_TFM_BOOT_STORE_MEASUREMENTS  OFF     CACHE BOOL  "Store measurement values from all the boot stages. Used for initial attestation token.")
    set(CONFIG_TFM_BOOT_STORE_ENCODED_MEASUREMENTS  OFF CACHE BOOL  "Enable storing of encoded measurements in boot.")
endif()

if (TFM_PARTITION_MEASURED_BOOT OR TFM_PARTITION_DPE)
    set(CONFIG_TFM_BOOT_STORE_ENCODED_MEASUREMENTS  OFF CACHE BOOL  "Enable storing of encoded measurements in boot.")
    set(MCUBOOT_DATA_SHARING                        ON)
endif()

set(TFM_MULTI_CORE_TOPOLOGY             ON)

set(PLAT_MHU_VERSION                    2          CACHE STRING  "Supported MHU version by platform")

set(RSE_AMOUNT                          1          CACHE STRING  "Amount of RSEes in the system")

set(RSE_TP_MODE                         TCI        CACHE STRING "Whether system is in Test or Production mode")

if (RSE_TP_MODE STREQUAL "TCI")
    set(RSE_RTL_KEY_PATH                ${CMAKE_CURRENT_LIST_DIR}/provisioning/tci_krtl.bin CACHE FILEPATH "Path to binary RTL key for encrypting provisioning bundles")
elseif(RSE_TP_MODE STREQUAL "PCI")
    set(RSE_RTL_KEY_PATH                ${CMAKE_CURRENT_LIST_DIR}/provisioning/pci_krtl_dummy.bin CACHE FILEPATH "Path to binary RTL key for encrypting provisioning bundles")
else()
    message(FATAL_ERROR "Invalid TP mode ${RSE_TP_MODE}")
endif()

# FIXME Enable this once the FVP is fixed
set(RSE_ENCRYPTED_OTP_KEYS              OFF        CACHE BOOL "Whether keys in OTP are encrypted")
set(RSE_ENABLE_TRAM                     OFF        CACHE BOOL "Whether TRAM encryption is enabled")

set(RSE_BIT_PROGRAMMABLE_OTP            ON         CACHE BOOL "Whether RSE OTP words can be programmed bit by bit, or whole words must be programmed at once")

set(RSE_SUPPORT_ROM_LIB_RELOCATION      OFF        CACHE BOOL "Whether shared ROM code supports being copied to SRAM and then executed")
set(RSE_USE_ROM_LIB_FROM_SRAM           OFF        CACHE BOOL "Whether shared ROM code will be used XIP from ROM or copied to SRAM and then executed")

if (RSE_USE_ROM_LIB_FROM_SRAM)
    set(CODE_SHARING_OUTPUT_FILE_SUFFIX     "_shared_symbols.axf" CACHE STRING "Suffix to use for code-sharing output files")
    set(CODE_SHARING_INPUT_FILE_SUFFIX      "_shared_symbols_in_sram.axf" CACHE STRING "Suffix to use for code-sharing input files")
    if (RSE_XIP)
        set(RSE_ROM_LIB_RELOCATION_OFFSET   +0x20010000           CACHE STRING "Relocation offset to be used to change section address w.r.t. ROM base address")
    else()
        set(RSE_ROM_LIB_RELOCATION_OFFSET   +0x20080000           CACHE STRING "Relocation offset to be used to change section address w.r.t. ROM base address")
    endif()
endif()

set(RSE_HAS_MANUFACTURING_DATA          OFF        CACHE BOOL "Whether manufacturing data is provisioned into RSE OTP")

set(RSE_DEFAULT_CLOCK_CONFIG            ON         CACHE BOOL "Use default RSE clock config implementation")

if (TEST_BL1_1 OR TEST_BL1_2)
    set(RSE_BL1_TEST_BINARY                 ON         CACHE BOOL "Create and run a separate BL1 test binary")
    set(RSE_TEST_BINARY_IN_ROM              ON         CACHE BOOL "Whether the RSE BL1 test binary is stored in ROM")
endif()

if (RSE_BL1_TEST_BINARY)
    set(PLATFORM_DEFAULT_BL1_1_TESTS    OFF        CACHE BOOL "Whether to use platform-specific BL1_1 testsuite")
    set(PLATFORM_DEFAULT_BL1_2_TESTS    OFF        CACHE BOOL "Whether to use platform-specific BL1_2 testsuite")
    set(BL1_1_PLATFORM_SPECIFIC_LINK_LIBRARIES rse_bl1_tests_platform)
    set(BL1_2_PLATFORM_SPECIFIC_LINK_LIBRARIES rse_bl1_tests_platform)
endif()

set(TEMP_BL1_1_SHARED_SYMBOLS_PATH             ${CMAKE_CURRENT_LIST_DIR}/bl1/bl1_1_shared_symbols.txt)
if (TEST_BL1_1 AND RSE_BL1_TEST_BINARY)
    list(APPEND TEMP_BL1_1_SHARED_SYMBOLS_PATH ${CMAKE_CURRENT_LIST_DIR}/tests/rse_test_executable/bl1_1_tests_shared_symbols.txt)
endif()

set(BL1_1_SHARED_SYMBOLS_PATH             ${TEMP_BL1_1_SHARED_SYMBOLS_PATH} CACHE FILEPATH "Path to list of symbols that BL1_1 that can be referenced from BL1_2")

if (TEST_BL1_2 AND RSE_BL1_TEST_BINARY)
    set(BL1_2_SHARED_SYMBOLS_PATH ${CMAKE_CURRENT_LIST_DIR}/tests/rse_test_executable/bl1_2_tests_shared_symbols.txt CACHE FILEPATH "Path to list of symbols that BL1_2 that can be referenced from tests")
endif()

set(EXTRA_BL1_1_TEST_SUITE_PATH         "${CMAKE_CURRENT_LIST_DIR}/tests/bl1_1" CACHE STRING "path to extra BL1_1 testsuite")
# TODO uncomment this once we have some extra BL1_2 tests, else this causes an error
# set(EXTRA_BL1_2_TEST_SUITE_PATH         "${CMAKE_CURRENT_LIST_DIR}/tests/bl1_2" CACHE STRING "path to extra BL1_2 testsuite")
# TODO remove this conditional once we have some extra secure, else this causes an error
if (${TEST_CC3XX})
    set(EXTRA_S_TEST_SUITE_PATH         "${CMAKE_CURRENT_LIST_DIR}/tests/secure" CACHE STRING "path to extra secure testsuite")
endif()

set(MCUBOOT_USE_PSA_CRYPTO            ON               CACHE BOOL      "Enable the cryptographic abstraction layer to use PSA Crypto APIs")
set(MCUBOOT_SIGNATURE_TYPE            "EC-P256"        CACHE STRING    "Algorithm to use for signature validation [RSA-2048, RSA-3072, EC-P256, EC-P384]")
set(MCUBOOT_HW_KEY                    ON               CACHE BOOL      "Whether to embed the entire public key in the image metadata instead of the hash only")
set(MCUBOOT_BUILTIN_KEY               OFF              CACHE BOOL      "Use builtin key(s) for validation, no public key data is embedded into the image metadata")

set(RSE_SUBPLATFORM_PAL_DEFAULT_NV_COUNTERS ON             CACHE BOOL      "Use default subplatform PAL nv counters mapping")
set(RSE_SUBPLATFORM_PAL_DEFAULT_ROTPK       ON             CACHE BOOL      "Use default subplatform PAL rotpk mapping")
set(RSE_SUBPLATFORM_PAL_CONFIG_DIR "${CMAKE_CURRENT_LIST_DIR}/subplatform_pal_default_config" CACHE PATH "path where subplatform PAL config headers can be found")

################################################################################

# Specifying the accepted values for certain configuration options to facilitate
# their later validation.

########################## Attestation #########################################

set_property(CACHE TFM_ATTESTATION_SCHEME PROPERTY STRINGS "OFF;PSA;CCA;DPE")
