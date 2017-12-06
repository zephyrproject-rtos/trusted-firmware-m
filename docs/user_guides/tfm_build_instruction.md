# TF-M build instruction

Please make sure you have all required software installed as explained in the
[software requirements](tfm_sw_requirement.md).

## TF-M build steps
TF-M uses [cmake](https://cmake.org/overview/) to provide an out-of-tree build
environment. The instructions are below.

### External dependency
* CMSIS_5 is used to import RTX for tests and apps
* mbedtls is used as crypto library on the secure side

Both need to be cloned manually in the current release.

### Build steps:

```
cd <TF-M base folder>
git clone https://github.com/ARM-software/trusted-firmware-m.git
git clone https://github.com/ARMmbed/mbedtls.git -b mbedtls-2.5.1
git clone https://github.com/ARM-software/CMSIS_5.git
cd trusted-firmware-m
mkdir cmake_build
cd cmake_build
cmake ../ -G"Unix Makefiles"
make
```

### Regression Tests
The default option build doesn't include regression tests. Procedure for
building the regression tests is below.

`It is recommended that tests are built in a different directory.`

*TF-M build regression tests on Linux*

```
cd <TF-M base folder>
cd trusted-firmware-m
mkdir cmake_test
cd cmake_test
cmake -G"Unix Makefiles" -DPROJ_CONFIG=`readlink -f ../ConfigRegression.cmake` ../
make
```

*TF-M build regression tests on Windows*

```
cd <TF-M base folder>
cd trusted-firmware-m
mkdir cmake_test
cd cmake_test
cmake -G"Unix Makefiles" -DPROJ_CONFIG=`cygpath -m ../ConfigRegression.cmake` ../
make
```

## Export dependency files for NS applications

An NS application requires a number of files to run with TF-M. The build
system can export these files using "install" target in to a single folder.

*On Windows*

```
cmake -G"Unix Makefiles" -DPROJ_CONFIG=`cygpath -m ../ConfigRegression.cmake` ../
make install
```

*On Linux*

```
cmake -G"Unix Makefiles" -DPROJ_CONFIG=`readlink -f ../ConfigRegression.cmake` ../
make install
```

The [integration guide](user_guides/tfm_integration_guide.md)
explains further details of creating a new NS app.

--------------

*Copyright (c) 2017, Arm Limited. All rights reserved.*
