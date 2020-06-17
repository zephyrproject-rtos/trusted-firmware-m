##################
Trusted Firmware M
##################
Trusted Firmware M provides a reference implementation of secure world software
for Arm M-profile architecture.

.. Note::
    The software implementation contained in this project is designed to be a
    reference implementation of the Platform Security Architecture (PSA).

Terms ``TFM`` and ``TF-M`` are commonly used in documents and code and both
refer to ``Trusted Firmware M.`` :doc:`Glossary </docs/reference/glossary>` has the list
of terms and abbreviations.

#######
License
#######
The software is provided under a BSD-3-Clause :doc:`License </docs/contributing/lic>`.
Contributions to this project are accepted under the same license with developer
sign-off as described in the :doc:`Contributing Guidelines </docs/contributing/contributing>`.

This project contains code from other projects as listed below. The code from
external projects is limited to ``app``, ``bl2``, ``lib`` and ``platform``
folders. The original license text is included in those source files.

- The ``app`` folder contains files imported from CMSIS_5 project and the files
  have Apache 2.0 license.
- The ``bl2`` folder contains files imported from MCUBoot project and the files
  have Apache 2.0 license.
- The ``lib`` folder may contain 3rd party files with diverse licenses.
- The ``platform`` folder currently contains platforms support imported from
  the external project and the files may have different licenses.

.. Note::
    Any folder that contains or directly imports 3rd party code is kept in a
    specific subfolder named ``ext`` so that it can be isolated if required.
    The 'ext' folder can be seen in the folders above mentioned.

#########################
Release Notes and Process
#########################
The :doc:`Release Cadence and Process </docs/contributing/release_process>` provides
release cadence and process information.

The :doc:`Change Log & Release Notes </docs/reference/changelog>` provides details of
major features of the release and platforms supported.

###############
Getting Started
###############

************
Prerequisite
************
Trusted Firmware M provides a reference implementation of PSA specifications.
It is assumed that the reader is familiar with PSA concepts and terms. PSA
specifications can be found at
`Platform Security Architecture Resources <https://developer.arm.com/architectures/security-architectures/platform-security-architecture>`__.

The current TF-M implementation specifically targets TrustZone for ARMv8-M so a
good understanding of the v8-M architecture is also necessary. A good place to
get started with ARMv8-M is
`developer.arm.com <https://developer.arm.com/technologies/trustzone>`__.

**********************
Really getting started
**********************
Trusted Firmware M source code is available on
`git.trustedfirmware.org <https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git/>`__.

To build & run TF-M:

    - Follow the :doc:`SW requirements guide </docs/getting_started/tfm_sw_requirement>`
      to set up your environment.
    - Follow the
      :doc:`Build instructions </docs/getting_started/tfm_build_instruction>` to compile
      and build the TF-M source.
    - Follow the :doc:`User guide </docs/getting_started/tfm_user_guide>` for information
      on running the example.

To port TF-M to a another system or OS, follow the
:doc:`OS Integration Guide </docs/getting_started/tfm_integration_guide>`

Please also see the :doc:`glossary </docs/reference/glossary>` of terms used in the project.

:doc:`Contributing Guidelines </docs/contributing/contributing>` contains guidance on how to
contribute to this project.

Further documents can be found in the ``docs`` folder.

###################
Platforms supported
###################
    - Cortex-M55 system:

        - `Fast model FVP_SSE300_MPS2.
          <https://developer.arm.com/tools-and-software/open-source-software/arm-platforms-software/arm-ecosystem-fvps>`_

    - Cortex-M33 system:

        - `FPGA image loaded on MPS2 board.
          <https://developer.arm.com/products/system-design/development-boards/cortex-m-prototyping-systems/mps2>`_
        - `Fast model FVP_MPS2_AEMv8M.
          <https://developer.arm.com/products/system-design/fixed-virtual-platforms>`_
        - `Musca-A test chip board.
          <https://developer.arm.com/products/system-design/development-boards/iot-test-chips-and-boards/musca-a-test-chip-board>`_
        - `Musca-B1 test chip board.
          <https://developer.arm.com/products/system-design/development-boards/iot-test-chips-and-boards/musca-b-test-chip-board>`_
        - `Musca-S1 test chip board.
          <https://developer.arm.com/tools-and-software/development-boards/iot-test-chips-and-boards/musca-s1-test-chip-board>`_
        - `FPGA image loaded on MPS3 board.
          <https://developer.arm.com/tools-and-software/development-boards/fpga-prototyping-boards/mps3>`_
        - `Arm DesignStart FPGA on AWS Cloud.
          <https://developer.arm.com/docs/101965/0102/arm-designstart-fpga-on-cloud-arm-ds-getting-started>`_
        - `NXP LPC55S69.
          <https://www.nxp.com/products/processors-and-microcontrollers/arm-microcontrollers/general-purpose-mcus/lpc5500-cortex-m33/lpcxpresso55s69-development-board:LPC55S69-EVK>`_
        - `NUCLEO L552ZE Q.
          <https://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-mpu-eval-tools/stm32-mcu-mpu-eval-tools/stm32-nucleo-boards/nucleo-l552ze-q.html>`_
        - `DISCO L562QE.
          <https://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-mpu-eval-tools/stm32-mcu-mpu-eval-tools/stm32-discovery-kits/stm32l562e-dk.html>`_

    - Cortex-M23 system:

        - `FPGA image loaded on MPS2 board.
          <https://developer.arm.com/products/system-design/development-boards/cortex-m-prototyping-systems/mps2>`_

    - Dual Core Cortex-M system:

        - `Cypress PSoc64.
          <https://www.cypress.com/documentation/product-brochures/cypress-psoc-64-secure-microcontrollers>`_

####################
Feedback and support
####################
For this release, feedback is requested via email to
`tf-m@lists.trustedfirmware.org <tf-m@lists.trustedfirmware.org>`__.

###############
Version history
###############
+-------------+--------------+--------------------+---------------+
| Version     | Date         | Description        | PSA-arch tag  |
+=============+==============+====================+===============+
| v1.0-beta   | 2019-02-15   | 1.0-beta release   |       -       |
+-------------+--------------+--------------------+---------------+
| v1.0-RC1    | 2019-05-31   | 1.0-RC1 release    | v19.06_API0.9 |
+-------------+--------------+--------------------+---------------+
| v1.0-RC2    | 2019-10-09   | 1.0-RC2 release    | v19.06_API0.9 |
+-------------+--------------+--------------------+---------------+
| v1.0-RC3    | 2019-11-29   | 1.0-RC3 release    | v19.06_API0.9 |
+-------------+--------------+--------------------+---------------+
| v1.0        | 2020-03-27   | 1.0 release        | v20.03_API1.0 |
+-------------+--------------+--------------------+---------------+

--------------

*Copyright (c) 2017-2020, Arm Limited. All rights reserved.*
