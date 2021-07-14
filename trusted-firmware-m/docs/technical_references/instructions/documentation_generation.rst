########################
Documentation generation
########################

The build system is prepared to support generation of two documents:

- Reference Manual. Doxygen based.
- User Guide. Sphinx based.

Both documents can be generated in HTML and PDF format.

Support for document generation in the build environment is not mandatory.
Missing document generation tools will not block building the TF-M firmware.

***************************
Build TF-M Reference Manual
***************************

The following tools are needed:

    - Doxygen v1.8.0 or later
    - Graphviz dot v2.38.0 or later
    - PlantUML v1.2018.11 or later
    - Java runtime environment 1.8 or later (for running PlantUML)
    - LaTeX - for PDF generation only
    - PdfLaTeX - for PDF generation only

.. tabs::

    .. group-tab:: Linux

        1. Set-up the needed tools and environment:

        .. code-block:: bash

            sudo apt-get install -y doxygen graphviz default-jre
            mkdir ~/plantuml
            curl -L http://sourceforge.net/projects/plantuml/files/plantuml.jar/download --output ~/plantuml/plantuml.jar
            export PLANTUML_JAR_PATH=~/plantuml/plantuml.jar

            # For PDF generation
            sudo apt-get install -y doxygen-latex

        2. Currently, there are two ways of building TF-M reference manual:

            - Using the CMake build system as custom targets

            .. code-block:: bash

                cd <TF-M base folder>
                cmake -S . -B cmake_doc -DTFM_PLATFORM=arm/mps2/an521
                cmake --build cmake_doc -- tfm_docs_refman_html tfm_docs_refman_pdf

            The documentation files will be available under the directory ``cmake_doc/docs/reference_manual``.

            - Manually using the appropriate tools (`sphinx-build`_/ `Doxygen`_)

            .. code-block:: bash

                # Build the documentation from build_docs directory
                cd <TF-M base folder>
                mkdir build_docs
                cp docs/conf.py build_docs/conf.py
                cd build_docs
                sphinx-build ./ user_guide

                # Build the documentation from a custom location
                # setting the build_docs as input

                # Note that using this method will still generate the reference manual
                # to the  <TF-M base folder>/build_docs/reference_manual
                cd <TF-M base folder>/<OTHER_DIR>/<OTHER_DIR2>
                sphinx-build  <TF-M base folder>/build_docs/ <DESIRED_OUTPUT_DIR>

            .. Note::

                Invoking Sphinx-build will build both user_guide and
                reference_manual targets.

    .. group-tab:: Windows

        1. Download and install the following tools:

        - `Doxygen 1.8.8 <https://sourceforge.net/projects/doxygen/files/snapshots/doxygen-1.8-svn/windows/doxygenw20140924_1_8_8.zip/download>`__
        - `Graphviz 2.38 <https://graphviz.gitlab.io/_pages/Download/windows/graphviz-2.38.msi>`__
        - The Java runtime is part of the Arm DS installation or can be
          downloaded from `here <https://www.java.com/en/download/>`__
        - `PlantUML <http://sourceforge.net/projects/plantuml/files/plantuml.jar/download>`__
        -  `MikTeX <https://miktex.org/download>`__ - for PDF generation only

        2. Set the environment variables, assuming that:

        - doxygen, dot, and MikTeX binaries are available on the PATH.
        - Java JVM is used from Arm DS installation.

        .. code-block:: bash

            set PLANTUML_JAR_PATH=<plantuml_Path>\plantuml.jar
            set PATH=$PATH;<ARM_DS_PATH>\sw\java\bin

        3. Using the CMake build system as custom targets to build TF-M
           reference manual:

        .. code-block:: bash

            cd <TF-M base folder>
            cmake -S . -B cmake_doc -DTFM_PLATFORM=arm/mps2/an521
            cmake --build cmake_doc -- tfm_docs_refman_html tfm_docs_refman_pdf

        The documentation files will be available under the directory ``cmake_doc\docs\reference_manual``.

*********************
Build TF-M User Guide
*********************

The following tools are needed:

    - Python3 and the following modules:
    - Sphinx v2.0.1
    - m2r v0.2.0
    - sphinxcontrib-plantuml
    - sphinxcontrib-svg2pdfconverter
    - sphinx-rtd-theme
    - docutils v0.16
    - Graphviz dot v2.38.0 or later
    - PlantUML v1.2018.11 or later
    - Java runtime environment 1.8 or later (for running PlantUML)
    - LaTeX - for PDF generation only
    - PdfLaTeX - for PDF generation only
    - librsvg2-bin - a SVG pictures renderer library to support
      sphinxcontrib-svg2pdfconverter

.. tabs::

    .. group-tab:: Linux

        1. Set-up the tools and environment:

        .. code-block:: bash

            sudo apt-get install -y python3 graphviz default-jre librsvg2-bin
            pip install -r tools/requirements.txt
            mkdir ~/plantuml
            curl -L http://sourceforge.net/projects/plantuml/files/plantuml.jar/download --output ~/plantuml/plantuml.jar

            # For PDF generation
            sudo apt-get install -y doxygen-latex
            export PLANTUML_JAR_PATH=~/plantuml/plantuml.jar

        2. Currently, there are two ways of building TF-M user guide:

            - Using the CMake build system as custom targets

            .. code-block:: bash

                cd <TF-M base folder>
                cmake -S . -B cmake_doc -DTFM_PLATFORM=arm/mps2/an521
                cmake --build cmake_doc -- tfm_docs_userguide_html tfm_docs_userguide_pdf

            The documentation files will be available under the directory ``cmake_doc/docs/user_guide``.

            - Manually using the appropriate tools (`sphinx-build`_/ `Doxygen`_)

            .. code-block:: bash

                # Build the documentation from build_docs directory
                cd <TF-M base folder>
                mkdir build_docs
                cp docs/conf.py build_docs/conf.py
                cd build_docs
                sphinx-build ./ user_guide

                # Build the documentation from a custom location
                # setting the build_docs as input

                # Note that using this method will still generate the reference manual
                # to the  <TF-M base folder>/build_docs/reference_manual
                cd <TF-M base folder>/<OTHER_DIR>/<OTHER_DIR2>
                sphinx-build  <TF-M base folder>/build_docs/ <DESIRED_OUTPUT_DIR>

            .. Note::

                Invoking Sphinx-build will build both user_guide and
                reference_manual targets.

    .. group-tab:: Windows

        1. Download and install the following tools:

        - `Graphviz 2.38 <https://graphviz.gitlab.io/_pages/Download/windows/graphviz-2.38.msi>`__
        - The Java runtime is part of the Arm DS installation or can be `downloaded from here <https://www.java.com/en/download/>`__
        - `PlantUML <http://sourceforge.net/projects/plantuml/files/plantuml.jar/download>`__
        - `MikTeX <https://miktex.org/download>`__ - for PDF generation only
        - Python3 `(native Windows version) <https://www.python.org/downloads/>`__
        - The necessary Python3 packages are listed in the requirements.txt file.

        To install all needed packages just do:

        .. code-block:: bash

            pip install -r tools\requirements.txt

        .. Note::
            When building the documentation the first time, MikTeX might prompt
            for installing missing LaTeX components. Please allow the MikTeX
            package manager to set-up these.

        2. Set the environment variables, assuming that:

        - plantuml.jar is available at c:\\plantuml\\plantuml.jar
        - doxygen, dot, and MikTeX binaries are available on the PATH.
        - Java JVM is used from DS5 installation.

        .. code-block:: bash

            set PLANTUML_JAR_PATH=<plantuml_Path>\plantuml.jar
            set PATH=$PATH;<ARM_DS_PATH>\sw\java\bin

        3. Using the CMake build system as custom targets to build TF-M user
           guide:

        .. code-block:: bash

            cd <TF-M base folder>
            cmake -S . -B cmake_doc -DTFM_PLATFORM=arm/mps2/an521
            cmake --build cmake_doc -- tfm_docs_userguide_html tfm_docs_userguide_pdf

        The documentation files will be available under the directory ``cmake_doc\docs\user_guide``.

.. _sphinx-build: https://www.sphinx-doc.org/en/master/man/sphinx-build.html
.. _Doxygen: https://www.doxygen.nl

--------------

*Copyright (c) 2017-2021, Arm Limited. All rights reserved.*
