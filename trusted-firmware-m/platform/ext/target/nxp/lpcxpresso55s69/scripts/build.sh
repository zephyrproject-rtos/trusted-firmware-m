#!/bin/bash
# Copyright (c) 2020, Linaro. All rights reserved.
# Copyright (c) 2020, Arm Limited. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause

# Exit on error
set -e

# Cleanup previous build artifacts
find . \! -path './lib*' \! -name '*.sh' -delete

# Generate the S and NS makefiles
cmake -DTFM_PLATFORM=nxp/lpcxpresso55s69 \
      -DTFM_TOOLCHAIN_FILE=../toolchain_GNUARM.cmake \
      -DCMAKE_BUILD_TYPE=Relwithdebinfo \
      -DTFM_PSA_API=ON -DTFM_ISOLATION_LEVEL=1 ../

# Build the binaries
make install
