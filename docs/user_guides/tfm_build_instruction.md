# TF-M build instruction

Please make sure you have all required software installed as explained in the
[software requirements](tfm_sw_requirement.md).

## TF-M build steps
TF-M uses [cmake](https://cmake.org/overview/) to provide an out-of-tree build
environment. The instructions are below.

*Note* In the cmake configuration step, to enable debug symbols, the following
option should be added:

`-DCMAKE_BUILD_TYPE=Debug`

### External dependency
* CMSIS_5 is used to import RTX for the example non-secure app
* mbedtls is used as crypto library on the secure side

Both need to be cloned manually in the current release.

### Build steps for the AN521 FPGA:

```
cd <TF-M base folder>
git clone https://git.trustedfirmware.org/trusted-firmware-m.git
git clone https://github.com/ARMmbed/mbedtls.git -b mbedtls-2.5.1
git clone https://github.com/ARM-software/CMSIS_5.git -b 5.2.0
cd trusted-firmware-m
mkdir cmake_build
cd cmake_build
cmake ../ -G"Unix Makefiles" -DTARGET_PLATFORM=AN521 -DCOMPILER=ARMCLANG
make
```

### Concept of build config files
The build configuration for TF-M is provided to the build system by three
different components:

* The way applications are built is specified by providing one of the
`Config<APP_NAME>.cmake` files to the build system. This can be done by adding
the `` -DPROJ_CONFIG=<absolute file path> `` i.e. on Linux:
`` -DPROJ_CONFIG=`readlink -f ../ConfigRegression.cmake` `` parameter to the
cmake command. (See examples below.)
* The target platform can be specified by adding the
`-DTARGET_PLATFORM=<target platform name>` option to the cmake command (See
  examples below.)
* Platforms currently supported
  * Cortex-M33 SSE-200 subsystem for MPS2+ (AN521)
  `-DTARGET_PLATFORM=AN521`
  * Cortex-M23 IoT Kit subsystem for MPS2+ (AN519)
    `-DTARGET_PLATFORM=AN519`
  * Musca-A1 test chip board (Cortex-M33 SSE-200 subsystem)
    `-DTARGET_PLATFORM=MUSCA_A`
* The compiler toolchain to be used for the build must be specified by adding
the `-DCOMPILER=<compiler name>` option to the cmake command (See examples
below.) The possible values are
    - ARMCLANG
    - GNUARM

*Note* For all the applications we build the level 2 bootloader

### Regression Tests for AN521
The default option build doesn't include regression tests. Procedure for
building the regression tests is below. Compiling for other target hardware
is possible by selecting a different build config file.

`It is recommended that tests are built in a different directory.`

*TF-M build regression tests on Linux*

```
cd <TF-M base folder>
cd trusted-firmware-m
mkdir cmake_test
cd cmake_test
cmake -G"Unix Makefiles" -DPROJ_CONFIG=`readlink -f ../ConfigRegression.cmake` -DTARGET_PLATFORM=AN521 -DCOMPILER=ARMCLANG ../
make
```

*TF-M build regression tests on Windows*

```
cd <TF-M base folder>
cd trusted-firmware-m
mkdir cmake_test
cd cmake_test
cmake -G"Unix Makefiles" -DPROJ_CONFIG=`cygpath -m ../ConfigRegression.cmake` -DTARGET_PLATFORM=AN521 -DCOMPILER=ARMCLANG ../
make
```

## Export dependency files for NS applications

An NS application requires a number of files to run with TF-M. The build
system can export these files using "install" target in to a single folder.

*On Windows*

```
cmake -G"Unix Makefiles" -DPROJ_CONFIG=`cygpath -m ../ConfigRegression.cmake` -DTARGET_PLATFORM=AN521 -DCOMPILER=ARMCLANG ../
make install
```

*On Linux*

```
cmake -G"Unix Makefiles" -DPROJ_CONFIG=`readlink -f ../ConfigRegression.cmake` -DTARGET_PLATFORM=AN521 -DCOMPILER=ARMCLANG ../
make install
```

The [integration guide](tfm_integration_guide.md)
explains further details on how to integrate a new NS app with TF-M.

--------------

*Copyright (c) 2017 - 2018, Arm Limited. All rights reserved.*
