#!/bin/bash
# Copyright (c) 2020, Linaro. All rights reserved.
# Copyright (c) 2020, Arm Limited. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause

# Exit on error
set -e

# Cleanup previous build artifacts
rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake install bl2 secure_fw app unit_test test

# Set the readlink binary name:
if [ "$(uname)" == "Darwin" ]; then
    # For OS X this should be be 'greadlink' (brew install coreutils)
    readlink=greadlink
else
    # For Linux this should be 'readlink'
    readlink=readlink
fi

# Generate the S and NS makefiles
cmake -DTFM_PLATFORM=nxp/lpcxpresso55s69 \
      -DCMAKE_TOOLCHAIN_FILE=../toolchain_GNUARM.cmake \
      -DCMAKE_BUILD_TYPE=Debug \
      ../

# Build the binaries
make install

# Convert S and NS binaries to .hex file
arm-none-eabi-objcopy -S --gap-fill 0xff -O ihex \
        install/outputs/LPC55S69/tfm_s.axf tfm_s.hex
arm-none-eabi-objcopy -S --gap-fill 0xff -O ihex \
        install/outputs/LPC55S69/tfm_ns.axf tfm_ns.hex
