#####################
Software requirements
#####################

.. |KEIL_VERSION| replace:: v5.25.2
.. |DEV_STUDIO_VERSION| replace:: 2019.1

A quick reference table is included in the
:ref:`docs/getting_started/tfm_sw_requirement:Tool & Dependency overview` section
of this document.

****************************
Supported build environments
****************************

TF-M officially supports a limited set of build environments and setups. In
this context, official support means that the environments listed below
are actively used by team members and active developers hence users should
be able to recreate the same configurations by following the instructions
described below. In case of problems, the TF-M team provides support
only for these environments, but building in other environments can still be
possible.

The following environments are supported:

    - Ubuntu 18.04 x64
    - Windows 10 x64

***********
C compilers
***********

To compile TF-M code, at least one of the supported compiler toolchains have to
be available in the build environment. The currently supported compiler
versions are:

    - Arm Compiler v6.10.1+
    - GNU Arm compiler v7.3.1+
    - IAR Arm compiler v8.42.x, v8.50.x

.. Note::
    - The Arm compilers above are provided via Keil uVision |KEIL_VERSION|
      or greater and Arm Development Studio
      |DEV_STUDIO_VERSION| or greater, or they can be downloaded as standalone
      packages from
      `here <https://developer.arm.com/products/software-development-tools/compilers/arm-compiler/downloads/version-6>`__.

    - The GNU Arm compiler can be downloaded from
      `here <https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads>`__.
      On the page select *GNU Arm Embedded Toolchain: 7-2018-q2-update* or an
      above version.

    - GNU Arm compiler version *10-2020-q4-major* has an issue in CMSE support.
      The bug is reported in `here <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99157>`__.
      Select other GNU Arm compiler versions instead.

    - The IAR Arm compilers above are provided via IAR Embedded Workbench (Windows) or
      IAR build tools for linux.
      For information, see
      `here <https://www.iar.com/iar-embedded-workbench/#!?architecture=Arm>`__ or
      `here <https://www.iar.com/iar-embedded-workbench/build-tools-for-linux/>`__.

*****
CMake
*****

The build-system is CMake based and supports the version 3.15 or higher.

    - Please use the latest build version available.
    - Recent versions of CMake can be downloaded from
      https://cmake.org/download/, and older releases are available from
      https://cmake.org/files.

CMake handles all external dependencies, but if you wish to alter this
behaviour, see :ref:`docs/getting_started/tfm_build_instruction:Manual
dependency management`

********
GNU make
********

TF-M project builds using the "Unix Makefiles" generator of CMake
and thus GNU make is needed for the build.

    - On Linux please use the version available in your distribution.
    - On Windows, GNU make v3.81 or higher shall work fine.

CMake generators other than "Unix Makefiles" may work too, but are not
officially supported. You may have a good luck with Ninja.

********************************************
Tools for configuring and programming boards
********************************************

Please follow a board secific documentation for building and programming
tools.

**************
Typical setups
**************

This section lists steps to set-up TF-M build environment under Linux and Windows.
Please also check the `Exceptions and special cases`_  chapter for any special
requirements depending on the tools you are using.

The build system can generate two documents: The Reference Manual and the
User Guide. Both documents can be generated in HTML and PDF format.

.. Note::
    Support for document generation in the build environment is not mandatory.
    Missing document generation tools will not block building the
    TF-M firmware.

Ubuntu
======

Setup needed tools for Ubuntu
-----------------------------

Tools required to build sources:

    - Arm DS |DEV_STUDIO_VERSION| (Arm DS Ultimate Edition)
      or GNU Arm compiler v7.3.1+
    - Git tools v2.10.0
    - CMake (see the `CMake`_ chapter)
    - GNU Make (see the `GNU make`_ chapter)
    - `SRecord v1.58 <http://srecord.sourceforge.net/download.html/>`__
      (for Musca test chip boards)
    - Python3 and the pip package manager (from Python 3.4 it's included)
    - Python3 packages as listed in tools/requirements.txt file

Tools required to generate documentation:

    - Doxygen v1.8.0 or later - for reference manual
    - Graphviz dot v2.38.0 or later
    - PlantUML v1.2018.11 or later
    - Java runtime environment 1.8 or later (for running PlantUML)
    - LaTeX - for PDF generation only
    - PdfLaTeX - for PDF generation only
    - Python3 packages as listed in tools/requirements.txt - for user manual

Download and install the required tools:

.. code-block:: bash

    sudo apt-get update
    sudo apt-get install -y git curl wget build-essential libssl-dev python3 \
    python3-pip doxygen graphviz default-jre doxygen-latex
    mkdir ~/plantuml
    curl -L http://sourceforge.net/projects/plantuml/files/plantuml.jar/download --output ~/plantuml/plantuml.jar

Clone the TF-M Sources
(see :ref:`docs/getting_started/tfm_build_instruction:Getting the source-code`),
then

.. code-block:: bash

    pip3 install --upgrade pip
    cd trusted-firmware-m
    pip3 install -r tools/requirements.txt

Finally, install C compiler (see `C compilers`_), CMake (see `CMake`_)
and SRecord as required.

Setup environment variables for Ubuntu
--------------------------------------

To import CMake in your bash shell console:

.. code-block:: bash

    export PATH=<CMake path>/bin:$PATH

To import Arm Compiler v6.13 in your bash shell console:

.. code-block:: bash

    export PATH=<ARM_DS_PATH>/sw/ARMCompiler6.13/bin:$PATH
    export ARM_TOOL_VARIANT=ult
    export ARM_PRODUCT_PATH=<ARM_DS_PATH>/sw/mappings
    export ARMLMD_LICENSE_FILE=<LICENSE_FILE_PATH>

To import GNU Arm in your bash shell console:

.. code-block:: bash

    export PATH=<bash path>/bin:$PATH

To import IAR Arm compiler in your bash shell console:

.. code-block:: bash

    export PATH=<IAR compiler path>/bin:$PATH

To import PLATUML:

.. code-block:: bash

    export PLANTUML_JAR_PATH=~/plantuml/plantuml.jar

Windows
=======

Setup needed tools for Windows
------------------------------

Tools required to build sources:

    - uVision |KEIL_VERSION| or Arm DS |DEV_STUDIO_VERSION| (Arm DS Ultimate Edition)
      or GNU Arm compiler v7.3.1+
    - Git client latest version (https://git-scm.com/download/win)
    - CMake (`native Windows version <https://cmake.org/download/>`__,
      see the `CMake`_ chapter)
    - GNU make (http://gnuwin32.sourceforge.net/packages/make.htm)
    - `SRecord v1.63 <https://sourceforge.net/projects/srecord/>`__ (for Musca test
      chip boards)
    - Python3 `(native Windows version) <https://www.python.org/downloads/>`__ and
      the pip package manager (from Python 3.4 it's included)
    - Python3 packages as listed in tools/requirements.txt file

Tools required to generate documentation:

    - `Doxygen v1.8.0
      <https://sourceforge.net/projects/doxygen/files/snapshots/doxygen-1.8-svn/
      windows/doxygenw20140924_1_8_8.zip/download>`__
      or later - for reference manual
    - `Graphviz dot v2.38.0
      <https://graphviz.gitlab.io/_pages/Download/windows/graphviz-2.38.msi>`__
      or later
    - `PlantUML v1.2018.11
      <http://sourceforge.net/projects/plantuml/files/plantuml.jar/download>`__
      or later
    - The Java runtime 1.8 or later (for running PlantUML) is part of the Arm DS
      installation or can be downloaded from
      `here <https://www.java.com/en/download/>`__
    - LaTeX - for PDF generation only
    - PdfLaTeX - for PDF generation only
    - `MikTeX <https://miktex.org/download>`__ - for PDF generation only
    - Python3 packages as listed in tools/requirements.txt - for user manual

Download and install the required tools from above.

Clone the TF-M Sources
(see :ref:`docs/getting_started/tfm_build_instruction:Getting the source-code`),
then

.. code-block:: bash

    cd trusted-firmware-m
    pip install -r tools\requirements.txt

Finally, install C compiler (see `C compilers`_), CMake (see `CMake`_)
and SRecord as required.

Setup environment variables for Windows
---------------------------------------
Set the environment variables, assuming that:

    - CMake, doxygen, dot, and MikTeX binaries are available on the PATH.
    - Java JVM is used from Arm DS installation.
    - plantuml.jar is available at c:\\plantuml\\plantuml.jar

To import CMake to your PATH variable:

.. code-block:: bash

    set PATH=<CMake_Path>\bin;$PATH

To import PLATUML:

.. code-block:: bash

    set PLANTUML_JAR_PATH=<plantuml_Path>\plantuml.jar

To set PATH for ARMDS:

.. code-block:: bash

    set PATH=$PATH;<ARM_DS_PATH>\sw\java\bin

There are several configurations depending on a toolset you are using.
The typical cases are listed below.

Armclang + Arm DS

.. code-block:: bash

    set PATH=<ARM_DS_PATH>\sw\ARMCompiler6.13\bin;$PATH
    set ARM_PRODUCT_PATH=<ARM_DS_PATH>\sw\mappings
    set ARM_TOOL_VARIANT=ult
    set ARMLMD_LICENSE_FILE=<LICENSE_FILE_PATH>

Armclang + Keil MDK Arm

.. code-block:: bash

    set PATH=<uVision path>\ARM\ARMCLANG\bin;$PATH

GNU Arm

.. code-block:: bash

    set PATH=<GNU Arm path>\bin;$PATH

****************************
Exceptions and special cases
****************************

ArmClang
========
    - Arm compiler specific environment variable may need updating based on
      specific products and licenses as explained in
      `product-and-toolkit-configuration <https://developer.arm.com/products/software-development-tools/license-management/resources/product-and-toolkit-configuration>`__.

MikTeX
======
    - When building the documentation the first time, MikTeX might prompt for
      installing missing LaTeX components. Please allow the MikTeX package
      manager to set-up these.

**************************
Tool & Dependency overview
**************************

To build the TF-M firmware the following tools are needed:

.. csv-table:: Tool dependencies
   :header: "Name", "Version", "Component"

   "C compiler",See `C compilers`_,"Firmware"
   "CMake",See `CMake`_,
   "GNU Make",See `GNU make`_,
   "tf-m-tests",`CMake`_ handles it,
   "mbed-crypto",`CMake`_ handles it,
   "MCUboot",`CMake`_ handles it,
   "Python",3.x,"Firmware, User Guide"
   "yaml",,"Firmware"
   "pyasn1",,"Firmware"
   "jinja2",,"Firmware"
   "cryptography",,"Firmware"
   "cbor",,"Firmware"
   "click",,"Firmware"
   "imgtool",,"Firmware"
   "Doxygen",">1.8","Reference manual"
   "Sphinx","=1.7.9","User Guide"
   "sphinxcontrib-plantuml",,"User Guide"
   "sphinxcontrib-svg2pdfconverter",,"User Guide"
   "sphinx-trd-theme",,"User Guide"
   "Git",,
   "PlantUML",">v1.2018.11","Reference Manual, User Guide"
   "Graphviz dot",">v2.38.0","Reference manual"
   "Java runtime environment (JRE)",">1.8","Reference Manual, User Guide"
   "LaTex",,"pdf version of Reference Manual and User Guide"
   "PdfLaTex",,"pdf version of Reference Manual and User Guide"

Dependency chain:

.. uml::

   @startuml
    skinparam state {
      BackgroundColor #92AEE0
      FontColor black
      FontSize 16
      AttributeFontColor black
      AttributeFontSize 16
      BackgroundColor<<pdf>> #A293E2
      BackgroundColor<<doc>> #90DED6
    }
    state fw as "Firmware" : TF-M binary
    state c_comp as "C Compiler" : C99
    state gmake as "GNU make"
    state u_guide as "User Guide" <<doc>>
    state refman as "Reference Manual" <<doc>>
    state rtd_theme as "sphinx-rtd-theme" <<doc>>
    state sphnix_puml as "sphinxcontrib-plantuml" <<doc>>
    state sphnix_svg as "sphinxcontrib-svg2pdfconverter" <<doc>>
    state JRE as "JRE" <<doc>> : Java Runtime Environment
    state gwiz as "Graphwiz dot" <<doc>>
    state Sphinx as "Sphinx" <<doc>>
    state m2r as "m2r" <<doc>>
    state PlantUML as "PlantUML" <<doc>>
    state LaTex as "LaTex" <<pdf>>
    state PdfLaTex as "PdfLaTex" <<<<pdf>>>>
    state Doxygen as "Doxygen" <<doc>>

    [*] --> fw
    fw --> c_comp
    fw --> CMake
    CMake --> gmake
    fw --> cryptography
    fw --> pyasn1
    fw --> yaml
    fw --> jinja2
    fw --> cbor
    fw --> click
    fw --> imgtool
    cryptography --> Python3
    pyasn1 --> Python3
    yaml --> Python3
    jinja2 --> Python3
    cbor --> Python3
    click --> Python3
    imgtool --> Python3

    [*] --> u_guide
    u_guide --> Sphinx
    Sphinx --> m2r
    Sphinx --> rtd_theme
    Sphinx --> sphnix_puml
    Sphinx --> sphnix_svg
    m2r --> Python3
    rtd_theme --> Python3
    sphnix_puml --> Python3
    sphnix_svg --> Python3
    Sphinx --> PlantUML
    PlantUML --> JRE
    PlantUML --> gwiz
    Sphinx --> LaTex
    LaTex --> PdfLaTex

    [*] --> refman
    refman --> Doxygen
    Doxygen --> PlantUML
    Doxygen --> LaTex
    state Legend {
      state x as "For PDF generation only" <<pdf>>
      state y as "For document generation only" <<doc>>
      state z as "Mandatory"
    }

   @enduml

--------------

*Copyright (c) 2017-2021, Arm Limited. All rights reserved.*
