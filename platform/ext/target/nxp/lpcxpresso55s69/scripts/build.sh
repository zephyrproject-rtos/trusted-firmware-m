#!/bin/bash
# Copyright (c) 2020, Linaro. All rights reserved.
# Copyright (c) 2020, Arm Limited. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause

# Exit on error
set -e

# Cleanup previous build artifacts
rm -rf app bin bl2 CMakeFiles generated install interface platform secure_fw tools cmake_install.cmake CMakeCache.txt

# Generate the S and NS makefiles
cmake -DTFM_PLATFORM=nxp/lpcxpresso55s69 \
      -DTFM_TOOLCHAIN_FILE=../toolchain_GNUARM.cmake \
      -DCMAKE_BUILD_TYPE=Relwithdebinfo \
      -DTFM_PROFILE=profile_medium ../

# Build the binaries
make install
