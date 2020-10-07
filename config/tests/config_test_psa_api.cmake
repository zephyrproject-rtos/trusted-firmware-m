#------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#------------------------------------------------------------------------------

############ Override defaults for PSA API tests ##############################

# CRYPTO_ENGINE_BUF_SIZE needs to be much larger for PSA API tests.
if(CRYPTO_ENGINE_BUF_SIZE LESS 0x5000)
    set(CRYPTO_ENGINE_BUF_SIZE              0x5000      CACHE STRING    "Heap size for the crypto backend" FORCE)
endif()
