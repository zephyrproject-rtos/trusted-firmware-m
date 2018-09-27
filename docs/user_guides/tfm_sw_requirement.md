# TF-M Software requirements

## Supported build environments
TF-M officially supports a limited set of build environments and setups. In
this context, official support means that the environments listed below
are actively used by team members and active developers hence users should
be able to recreate the same configurations by following the instructions
described below.  In case of problems, the TF-M team provides support only for
these environments, but building in other environments can still be possible.

The following environments are supported:
- Windows 10 x64 + Cygwin x64 (example configuration is provided for this
  Windows setup only).
- Windows 10 x64 + msys2 x64.
- Windows 10 x65 + git-bash (MinGW) + gnumake from DS-5 or msys2.
- Ubuntu 16.04 x64
- Ubuntu 18.04 x64

## Supported C compilers
To compile TF-M code, at least one of the supported compiler toolchains have to
be available in the build environment.
The currently supported compiler versions are:
- Arm Compiler v6.7.1
- Arm Compiler v6.9
- Arm Compiler v6.10
- Arm Compiler v6.11
- GNU Arm compiler v6.3.1
- GNU Arm compiler v7.3

**Notes:**

  - The Arm compilers above are provided via Keil uVision v5.24.1 or greater,
     DS-5 v5.27.1 or greater, and Development Studio 2018.0, or they can be
     downloaded as standalone packages from [here](https://developer.arm.com/products/software-development-tools/compilers/arm-compiler/downloads/version-6).

  - Arm compiler specific environment variable may need updating based
    on specific products and licenses as explained in
    [product-and-toolkit-configuration](https://developer.arm.com/products/software-development-tools/license-management/resources/product-and-toolkit-configuration).

  - The GNU Arm compiler can be downloaded from [here](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads).
    On the page select *GNU Arm Embedded Toolchain: 6-2017-q1-update* or
    *GNU Arm Embedded Toolchain: 7-2018-q2-update*

## Supported CMake versions
The build-system is CMake based and supports the following versions:
- 3.7
- 3.10
- 3.11
- 3.12
- 3.13
- 3.14

Please use the latest build version available (i.e. 3.7.2 instead of 3.7.0).
While it is preferable to use the newest version this is not required and any
version from the above list should work.

Recent versions of CMake can be downloaded from https://cmake.org/download/, and
older releases are available from https://cmake.org/files.

## Supported GNU make versions
The TF-M team builds using the "Unix Makefiles" generator of CMake and thus
GNU make is needed for the build. On Linux please use the version
available from the official repository of your distribution.

On Windows the following binaries are supported:
- GNU make v4.2.1 executable from Cygwin
- GNU make v4.2.1 executable from msys2
- GNU make v4.2 executable from DS5 v5.29.1 (see <DS-5_PATH>/bin)

CMake is quiet tolerant to GNU make versions and basically any "reasonably
recent" GNU make version shall work.

CMake generators other than "Unix Makefiles" may work, but are not officially
supported.

## Example setups
This section lists dependencies and some exact and tested steps to set-up a
TF-M-m build environment under various OSes.

### Ubuntu
- DS-5 v5.27.1.
- Git tools v2.10.0
- CMake (see the "Supported CMake versions" chapter)
- GNU Make (see the "Supported make versions" chapter)
- Python3, with the following libraries:
  - pycrypto
  - pyasn1
  - yaml
  - jinja2 v2.10
  - sudo apt-get install python3-crypto python3-pyasn1 python3-yaml python3-jinja2
- SRecord v1.58 (for Musca test chip boards)

#### Setup a shell to enable compiler toolchain and CMake after installation.

To import Arm Compiler v6.7.1 in your bash shell console:

**Note:** Arm compiler specific environment variable may need updating based
on specific products and licenses as explained in
[product-and-toolkit-configuration](https://developer.arm.com/products/software-development-tools/license-management/resources/product-and-toolkit-configuration).

~~~
export PATH=<DS-5_PATH>/sw/ARMCompiler6.7.1/bin:$PATH
export ARM_TOOL_VARIANT="ult"
export ARM_PRODUCT_PATH="<DS-5_PATH>/sw/mappings"
export ARMLMD_LICENSE_FILE="<LICENSE_FILE_PATH>"
~~~
To import CMake in your bash shell console:
~~~
export PATH=<CMAKE_PATH>/bin:$PATH
~~~
To import GNU Arm in your bash shell console:
~~~
export PATH=<GNU_ARM_PATH>/bin:$PATH
~~~

### Windows + Cygwin
- uVision v5.24.1 or DS-5 v5.27.1 (DS-5 Ultimate Edition) which provides the
  Arm Compiler v6.7.1 compiler or GNU Arm compiler v6.3.1.
- Git client latest version (https://git-scm.com/download/win)
- CMake (see the "Supported CMake versions" chapter)
- [Cygwin]( https://www.cygwin.com/ ). Tests done with version 2.877 (64 bits)
- GNU make should be installed by selecting appropriate package during cygwin
  installation.
- Python3 [(native Windows version)](https://www.python.org/downloads/), with the following libraries:
  - pycryptodome (pip3 install --user pycryptodome)
  - pyasn1 (pip3 install --user pyasn1)
  - pyyaml (pip3 install --user pyyaml)
  - jinja2 (pip3 install --user jinja2)
- Python3 pip
- [SRecord v1.63](https://sourceforge.net/projects/srecord/) (for Musca test chip boards)

#### Setup Cygwin to enable a compiler toolchain and CMake after installation.

If applicable, import Arm Compiler v6.7.1 in your shell console. To make this
change permanent, add the command line into ~/.bashrc

**DS-5**

**Notes:**

  - Arm compiler specific environment variable may need updating based
    on specific products and licenses as explained in
    [product-and-toolkit-configuration](https://developer.arm.com/products/software-development-tools/license-management/resources/product-and-toolkit-configuration).

  - Arm licensing related environment variables must use Windows paths,
    and not the Cygwin specific one relative to */cygrive*.

~~~
export PATH="/cygdrive/c/<DS-5_PATH>/sw/ARMCompiler6.7.1/bin":$PATH
export ARM_PRODUCT_PATH="C:/<DS-5_PATH>/sw/mappings"
export ARM_TOOL_VARIANT="ult"
export ARMLMD_LICENSE_FILE="<LICENSE_FILE_PATH>"
~~~

**Keil MDK Arm**

**Notes:**
- Arm compiler specific environment variable may need updating based
  on specific products and licenses as explained in
  [product-and-toolkit-configuration](https://developer.arm.com/products/software-development-tools/license-management/resources/product-and-toolkit-configuration).

~~~
export PATH="/cygdrive/c/<uVision path>/ARM/ARMCLANG/bin":$PATH
~~~

If applicable, import GNU Arm compiler v6.3.1 in your shell console. To make
this change permanent, add the command line into ~/.bashrc

**GNU Arm**
~~~
export PATH=<GNU_ARM_PATH>/bin:$PATH
~~~

To import CMake in your bash shell console:

**CMake**

~~~
export PATH=/cygdrive/c/<CMAKE_PATH>/bin:$PATH
~~~

### Building documentation
The build system is prepared to support generation of two documents. The
Reference Manual which is Doxygen based, and the User Guide which is Sphinx
based.
Both document can be generated in HTML and PDF format.

*Note* support for document generation in the build environment is not
mandatory. Missing document generation tools will not block building the TF-M
firmware.

#### To compile the TF-M Reference Manual
The following additional tools are needed:
- Doxygen v1.8.0 or later
- Graphviz dot v2.38.0 or later
- PlantUML v1.2018.11 or later
- Java runtime environment 1.8 or later (for running PlantUML)

For PDF generation the following tools are needed in addition to the above list:
- LaTeX
- PdfLaTeX

##### Set-up the needed tools

###### Linux
- sudo apt-get install -y doxygen graphviz default-jre
- mkdir ~/plantuml; curl -L http://sourceforge.net/projects/plantuml/files/plantuml.jar/download --output ~/plantuml/plantuml.jar

For PDF generation:
- sudo apt-get install -y doxygen-latex

###### Windows
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

###### Configure the shell

####### Linux
~~~
export PLANTUML_JAR_PATH="~/plantuml/plantuml.jar"
~~~

####### Windows + Cygwin
Assumptions for the settings below:
- plantuml.jar is available at c:\plantuml\plantuml.jar
- doxygen, dot, and MikTeX binaries are available on the PATH.

~~~
export PLANTUML_JAR_PATH="c:/plantuml/plantuml.jar"
export PATH=$PATH:/cygdrive/c/<DS-5 path>/sw/java/bin
~~~

#### To compile the TF-M User Guide
The following additional tools are needed:
- Python3 and the following modules:
    - Sphinx v1.7.9
    - m2r v0.2.0
- Graphviz dot v2.38.0 or later
- PlantUML v1.2018.11 or later
- Java runtime environment 1.8 or later (for running PlantUML)

For PDF generation the following tools are needed in addition to the above list:
- LaTeX
- PdfLaTeX

##### Set-up the needed tools

###### Linux
- sudo apt-get install -y python3 graphviz default-jre
- pip --user install m2r Sphinx sphinx-rtd-theme
- mkdir ~/plantuml; curl -L http://sourceforge.net/projects/plantuml/files/plantuml.jar/download --output ~/plantuml/plantuml.jar

For PDF generation:
- sudo apt-get install -y doxygen-latex

###### Windows
- Python3 [(native Windows version)](https://www.python.org/downloads/)
- pip --user install m2r Sphinx sphinx-rtd-theme
- [Graphviz 2.38](https://graphviz.gitlab.io/_pages/Download/windows/graphviz-2.38.msi)
- The Java runtime is part of the DS5 installation or can be
  [downloaded from here](https://www.java.com/en/download/)
- [PlantUML](http://sourceforge.net/projects/plantuml/files/plantuml.jar/download)

For PDF generation:
- [MikTeX](https://miktex.org/download)

*Note* When building the documentation the first time, MikTeX might prompt for
installing missing LaTeX components. Please allow the MikTeX package manager to
set-up these.

##### Configure the shell

###### Linux

~~~
export PLANTUML_JAR_PATH="~/plantuml/plantuml.jar"
~~~

###### Windows + Cygwin

Assumptions for the settings below:
- plantuml.jar is available at c:\plantuml\plantuml.jar
- doxygen, dot, and MikTeX binaries are available on the PATH.

~~~
export PLANTUML_JAR_PATH="c:/plantuml/plantuml.jar"
export PATH=$PATH:/cygdrive/c/<DS-5 path>/sw/java/bin
~~~

--------------

*Copyright (c) 2017-2019, Arm Limited. All rights reserved.*
