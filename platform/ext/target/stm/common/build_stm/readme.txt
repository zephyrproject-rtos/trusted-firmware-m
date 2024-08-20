STM_PLATFORM
^^^^^^^^^^^^

Configuration and Build
"""""""""""""""""""""""

GNUARM/ARMCLANG/IARARM compilation is available for this target.
and build the selected configuration as follow.

The build configuration for TF-M is provided to the build system using command
line arguments. Required arguments are noted below.

The following instructions build multi-core TF-M with regression test suites
in Isolation Level 1.


Getting_started
"""""""""""""""

1- Should install TF-M repo and all library in the same directory (trusted-firmware-m, mbedtls, mcuboot, QCBOR, tf-m-tests).
2- Configure the script as you nedd for your project.
3- Launch the script ReBuildTFM_S.bat and ReBuildTFM_NS.bat
4- In iar/build_s/api_ns laucn the scripts (postbuild.sh, regressions.sh, TFM_UPDATE.sh).
5- Reset the board.

Example
"""""""
There's image example to the environment what should you have to use these scripts

-------------

*Copyright (c) 2021, STMicroelectronics. All rights reserved.*
*SPDX-License-Identifier: BSD-3-Clause*
