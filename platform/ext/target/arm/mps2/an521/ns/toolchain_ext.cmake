#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# In the new split build this file defines a platform specific parameters
# like mcpu core, arch etc and to be included by NS toolchain file.
# A platform owner is free to configure toolchain here for building NS side.

set(CMAKE_SYSTEM_PROCESSOR cortex-m33)

if (${CMAKE_C_COMPILER_ID} STREQUAL GNU)

    add_compile_options(-mcpu=cortex-m33+nodsp)
    add_link_options(-mcpu=cortex-m33+nodsp)

elseif (${CMAKE_C_COMPILER_ID} STREQUAL ARMClang)

    add_compile_options(-mcpu=cortex-m33+nodsp)
    add_link_options(--cpu=cortex-m33+nodsp)

elseif (${CMAKE_C_COMPILER_ID} STREQUAL IAR)

    add_compile_options(--cpu cortex-m33)
    add_link_options(--cpu=cortex-m33+nodsp)

endif()

set(CONFIG_TFM_FP_ARCH "fpv5-d16")
set(CONFIG_TFM_FP_ARCH_ASM "FPv5_D16")
