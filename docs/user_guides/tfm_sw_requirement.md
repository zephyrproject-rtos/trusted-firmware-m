# TF-M Software requirements

## To compile TF-M code
At least one of the supported compiler toolchains have to be available in the
build environment.
The currently supported compiler versions are:
- ARM Compiler v6.7.1
- ARM Compiler v6.9
- ARM Compiler v6.10
- ARM Compiler v6.11
- GNU Arm compiler v6.3.1
- GNU Arm compiler v7.3

*Note* The ARM compilers above are provided via Keil uVision v5.24.1 or greater,
DS-5 v5.27.1 or greater, and Development Studio 2018.0, or they can be
downloaded as standalone packages from [here](https://developer.arm.com/products/software-development-tools/compilers/arm-compiler/downloads/version-6).

*Note* The GNU Arm compiler can be downloaded from [here](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads).
On the page select *GNU Arm Embedded Toolchain: 6-2017-q1-update* or *GNU Arm
Embedded Toolchain: 7-2018-q2-update*

*Note* In this version, you also need manually clone CMSIS_5 and mbedtls
repositories at the same location where tfm repo is located.

*Note* ARM compiler specific environment variable may need updating based
on specific products and licenses as explained in
[product-and-toolkit-configuration](https://developer.arm.com/products/software-development-tools/license-management/resources/product-and-toolkit-configuration).


### Linux
- DS-5 v5.27.1.
- Git tools v2.10.0
- Cmake v3.7.0
  https://cmake.org/download/
- GNU Make v3.81
- Python3, with the following libraries:
  - pycrypto
  - pyasn1
  - yaml
  - sudo apt-get install python3-crypto python3-pyasn1 python3-yaml
- SRecord v1.58 (for Musca test chip boards)


#### Setup a shell to enable compiler toolchain and cmake after installation.

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
To import GNU Arm in your bash shell console:
~~~
export PATH=<GNU_ARM_PATH>/bin:$PATH
~~~

### On Windows
- uVision v5.24.1 or DS-5 v5.27.1 (DS-5 Ultimate Edition) which provides the
  ARM Compiler v6.7.1 compiler or GNU Arm compiler v6.3.1.
- Git client latest version (https://git-scm.com/download/win)
- Cmake v3.7.0 ( https://cmake.org/download/ )
- Cygwin ( https://www.cygwin.com/ ). Tests done with version 2.877 (64 bits)
- GNU make should be installed by selecting appropriate package during cygwin
  installation.
- Python3(native Windows version), with the following libraries:
  - pycryptodome (pip3 install --user pycryptodome)
  - pyasn1 (pip3 install --user pyasn1)
  - pyyaml (pip3 install --user pyyaml)
- Python3 pip
- SRecord v1.63 (for Musca test chip boards)

#### Setup Cygwin to enable a compiler toolchain and cmake after installation.

If applicable, import ARM Compiler v6.7.1 in your shell console. To make this
change permanent, add the command line into ~/.bashrc

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

If applicable, import GNU Arm compiler v6.3.1 in your shell console. To make
this change permanent, add the command line into ~/.bashrc

**GNU Arm**
~~~
export PATH=<GNU_ARM_PATH>/bin:$PATH
~~~

To import cmake in your bash shell console:

**cmake**

~~~
export PATH=/cygdrive/c/<CMAKE_PATH>/bin:$PATH
~~~

## To compile the TF-M Reference Manual
The following additional tools are needed:
- Doxygen v1.8.0 or later
- Graphviz dot v2.38.0 or later
- PlantUML v1.2018.11 or later
- Java runtime environment 1.8 or later (for running PlantUML)

For PDF generation the following tools are needed in addition to the above list:
- LaTeX
- PdfLaTeX

*Note* support for document generation in the build environment is not
mandatory. Missing document generation tools will not block building the TF-M
firmware.

###Set-up the needed tools

#### Linux
- sudo apt-get install -y doxygen graphviz default-jre
- mkdir ~/plantuml; curl -L http://sourceforge.net/projects/plantuml/files/plantuml.jar/download --output ~/plantuml/plantuml.jar

For PDF generation:
- sudo apt-get install -y doxygen-latex

#### Windows
- [Doxygen 1.8.8](https://sourceforge.net/projects/doxygen/files/snapshots/doxygen-1.8-svn/windows/doxygenw20140924_1_8_8.zip/download)
- [Graphviz 2.38](https://graphviz.gitlab.io/_pages/Download/windows/graphviz-2.38.msi)
- The Java runtime is part of the DS5 installation or can be
  [downloaded from here](https://www.java.com/en/download/)
- [PlantUML](http://sourceforge.net/projects/plantuml/files/plantuml.jar/download)

For PDF generation:
- [MikTeX](https://miktex.org/download)

*Note* When building the documentation the first time, MikTeX might prompt for
installing missing LaTeX components. Please allow the MikTeX package manager to
set-up these.

### Configure the shell

#### Linux
~~~
export PLANTUML_JAR_PATH="~/plantuml/plantuml.jar"
~~~

#### Windows
Assumptions for the settings below:
- plantuml.jar is available at c:\plantuml\plantuml.jar
- doxygen, dot, and MikTeX binaries are available on the PATH.

~~~
export PLANTUML_JAR_PATH="c:/plantuml/plantuml.jar"
export PATH=$PATH:/cygdrive/c/<DS-5 path>/sw/java/bin
~~~

--------------
*Copyright (c) 2017-2019, Arm Limited. All rights reserved.*

