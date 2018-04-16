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

### Build steps for the AN521 target platform:

```
cd <TF-M base folder>
git clone https://git.trustedfirmware.org/trusted-firmware-m.git
git clone https://github.com/ARMmbed/mbedtls.git -b mbedtls-2.6.1
git clone https://github.com/ARM-software/CMSIS_5.git -b 5.2.0
cd trusted-firmware-m
mkdir cmake_build
cd cmake_build
cmake ../ -G"Unix Makefiles" -DTARGET_PLATFORM=AN521 -DCOMPILER=ARMCLANG
cmake --build ./ -- install
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
* Supported platforms:
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

### Regression Tests for the AN521 target platform
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
cmake --build ./ -- install
```

*TF-M build regression tests on Windows*

```
cd <TF-M base folder>
cd trusted-firmware-m
mkdir cmake_test
cd cmake_test
cmake -G"Unix Makefiles" -DPROJ_CONFIG=`cygpath -m ../ConfigRegression.cmake` -DTARGET_PLATFORM=AN521 -DCOMPILER=ARMCLANG ../
cmake --build ./ -- install
```

## Location of build artefacts

The build system defines an API which allow easy usage of build artefacts. The
`install` build target copies all files which might be needed as a dependency by
external tools or build systems to the `<build_dir>/install/outputs` directory,
with the following directory hierarchy:
```
<build_dir>/install/outputs/fvp/
<build_dir>/install/outputs/<target_platform>/
```
There is one folder for FVP testing, with more elaborate naming and there is an
other for testing on target hardware platform (AN521, etc.), where naming
convention is aligned with 8.3 format. The dependency tree of `install` build
target ensures a proper update (i.e. build) of all output files before the
actual installation step takes place. As such it is suggested to use this build
target to build TF-M.

## Export dependency files for NS applications

An NS application requires a number of files to interface with TF-M. The build
system exports these files as part of the `install` target and places them in to
a single directory, `<build_dir>/install/export/tfm`. Further details on how to
integrate a new NS app with TF-M are available in the
[integration guide](tfm_integration_guide.md).


--------------

*Copyright (c) 2017 - 2018, Arm Limited. All rights reserved.*
