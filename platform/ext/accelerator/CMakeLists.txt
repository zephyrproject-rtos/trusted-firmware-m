#-------------------------------------------------------------------------------
# Copyright (c) 2020-2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (TFM_PARTITION_CRYPTO)
    add_library(crypto_service_crypto_hw STATIC)
endif()

message(STATUS "[Platform] Crypto HW accelerator type: ${CRYPTO_HW_ACCELERATOR_TYPE}")
add_subdirectory(${CRYPTO_HW_ACCELERATOR_TYPE})

############################ Project Config ####################################

target_compile_definitions(tfm_config
    INTERFACE
        CRYPTO_HW_ACCELERATOR
)

############################ Crypto Service ####################################

if (TFM_PARTITION_CRYPTO)
    target_include_directories(crypto_service_crypto_hw
        PUBLIC
            interface
    )

    target_compile_definitions(crypto_service_crypto_hw
        PUBLIC
            CRYPTO_HW_ACCELERATOR
    )
endif()
