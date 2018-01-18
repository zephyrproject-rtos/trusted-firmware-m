# TF-M Software requirements
To compile TF-M code, you need ARM Compiler v6.7.1. The ARM
compiler is provided via Keil uVision v5.24.1 or DS-5 v5.27.1.

*Note* In this version, you also need manually clone CMSIS_5 and mbedtls
repositories at the same location where tfm repo is located.

*Note* ARM compiler specific environment variable may need updating based
on specific products and licenses as explained in
[product-and-toolkit-configuration](https://developer.arm.com/products/software-development-tools/license-management/resources/product-and-toolkit-configuration).


## Linux
- DS-5 v5.27.1.
- Git tools v2.10.0
- Cmake v3.7.0
  https://cmake.org/download/
- GNU Make v3.81
- Python3, with the following libraries:
  - pycrypto
  - pyasn1
  - sudo apt-get install python3-crypto python3-pyasn1

### Setup a shell to enable ARM Compiler v6.7.1 and cmake after installation.

To import ARM Compiler v6.7.1 in your bash shell console:
~~~
export PATH=<DS-5_PATH>/sw/ARMCompiler6.7.1/bin:$PATH
export ARM_TOOL_VARIANT="ult"
export ARM_PRODUCT_PATH="<DS-5_PATH>/sw/mappings"
export ARMLMD_LICENSE_FILE="<LICENSE_FILE_PATH>"
~~~
To import cmake in your bash shell console:
~~~
export PATH=<CMAKE_PATH>/bin:$PATH
~~~

## On Windows
- uVision v5.24.1 or DS-5 v5.27.1 (DS-5 Ultimate Edition) which provides the
  ARM Compiler v6.7.1 compiler.
- Git client latest version (https://git-scm.com/download/win)
- Cmake v3.7.0 ( https://cmake.org/download/ )
- Cygwin ( https://www.cygwin.com/ ). Tests done with version 2.877 (64 bits)
- GNU make should be installed by selecting appropriate package during cygwin
  installation.
- Python3(native Windows version), with the following libraries:
  - pycryptodome (pip3 install --user pycryptodome)
  - pyasn1 (pip3 install --user pyasn1)
- Python3 pip

### Setup Cygwin to enable ARM Compiler v6.7.1 and cmake after installation.

Import ARM Compiler v6.7.1 in your shell console. To make this change permanent,
add the command line into ~/.bashrc

**DS5**
~~~
export PATH="/cygdrive/c/<DS-5 path>/sw/ARMCompiler6.7.1/bin":$PATH
export ARM_PRODUCT_PATH="C:/<DS-5 path>/sw/mappings"
~~~

**Keil**
~~~
export PATH="/cygdrive/c/<uVison path>/ARM/ARMCLANG/bin":$PATH
export ARM_PRODUCT_PATH="C:/<uVison path>/ARM/sw/mappings"
~~~

**Shared settings**
~~~
export ARM_TOOL_VARIANT="ult"
export ARMLMD_LICENSE_FILE="<LICENSE_FILE_PATH>"
~~~
To import cmake in your bash shell console:
~~~
export PATH=/cygdrive/c/<CMAKE_PATH>/bin:$PATH
~~~
--------------

*Copyright (c) 2017, Arm Limited. All rights reserved.*
