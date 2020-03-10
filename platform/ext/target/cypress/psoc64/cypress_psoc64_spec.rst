########################
Cypress PSoC64 Specifics
########################

*************
Prerequisites
*************

PSoC64 must first be provisioned with SecureBoot firmware and a provisioning packet
containing policy and secure keys. Please refer to the guide at
https://www.cypress.com/documentation/software-and-drivers/psoc-64-secure-mcu-secure-boot-sdk-user-guide
Use the following policy file for provisioning and signing:
policy_multi_img_CM0p_CM4_debug_2M.json


Please make sure you have all required software installed as explained in the
:doc:`software requirements </docs/user_guides/tfm_sw_requirement>`.

Please also make sure that all the source code are fetched by following
:doc:`general building instruction </docs/user_guides/tfm_build_instruction>`.

Install CySecureTools.

.. code-block:: bash

    pip3 install cysecuretools==1.3.3

For more details please refer to
`CySecureTools <https://pypi.org/project/cysecuretools/1.3.3/>`_ page.

Install OpenOCD with PSoC6 support. It can be obtained from the Cypress
Programmer, download it from:
https://www.cypress.com/products/psoc-programming-solutions

******************************************
Building Multi-Core TF-M on Cypress PSoC64
******************************************

Configuring the build
=====================

The build configuration for TF-M is provided to the build system using command
line arguments:

.. list-table::
   :widths: 20 80

   * - -DPROJ_CONFIG=<file>
     - Specifies the way the application is built.

       | <file> is the absolute path to configurations file
         named as ``Config<APP_NAME>.cmake``.
       | e.g. On Linux:
         ``-DPROJ_CONFIG=`readlink -f ../configs/ConfigRegressionIPC.cmake```
       | Supported configurations files

           - IPC model without regression test suites in Isolation Level 1
             ``ConfigCoreIPC.cmake``
           - IPC model with regression test suites in Isolation Level 1
             ``ConfigRegressionIPC.cmake``
           - IPC model with PSA API test suite in Isolation Level 1
             ``ConfigPsaApiTestIPC.cmake``
           - IPC model without regression test suites in Isolation Level 2
             ``ConfigCoreIPCTfmLevel2.cmake``
           - IPC model with regression test suites in Isolation Level 2
             ``ConfigRegressionIPCTfmLevel2.cmake``
           - IPC model with PSA API test suite in Isolation Level 2
             ``ConfigPsaApiTestIPCTfmLevel2.cmake``

   * - -DTARGET_PLATFORM=psoc64
     - Specifies target platform name ``psoc64``

   * - -DCOMPILER=<compiler name>
     - Specifies the compiler toolchain
       The possible values are:

         - ``ARMCLANG``
         - ``GNUARM``

   * - -DCMAKE_BUILD_TYPE=<build type>
     - Configures debugging support.
       The possible values are:

         - ``Debug``
         - ``Release``


Build Instructions
==================

The following instructions build multi-core TF-M without regression test suites
in Isolation Level 1 on Linux.
Both the compiler and the debugging type can be changed to other configurations
listed above.

.. code-block:: bash

    cd <TF-M base folder>
    cd <trusted-firmware-m folder>

    mkdir <build folder>
    pushd <build folder>
    cmake -G"Unix Makefiles" -DPROJ_CONFIG=`readlink \
          -f ../configs/ConfigCoreIPC.cmake` \
          -DTARGET_PLATFORM=psoc64 \
          -DCOMPILER=ARMCLANG \
          -DCMAKE_BUILD_TYPE=Release \
          ../
    popd
    cmake --build <build folder> -- -j VERBOSE=1

The following instructions build multi-core TF-M with regression test suites
in Isolation Level 1 on Linux.
Both the compiler and the debugging type can be changed to other configurations
listed above.

.. code-block:: bash

    cd <TF-M base folder>
    cd <trusted-firmware-m folder>

    mkdir <build folder>
    pushd <build folder>
    cmake -G"Unix Makefiles" -DPROJ_CONFIG=`readlink \
          -f ../configs/ConfigRegressionIPC.cmake` \
          -DTARGET_PLATFORM=psoc64 \
          -DCOMPILER=ARMCLANG \
          -DCMAKE_BUILD_TYPE=Release ../
    popd
    cmake --build <build folder> -- -j VERBOSE=1

The following instructions build multi-core TF-M with PSA API test suite for
the attestation service in Isolation Level 1 on Linux.
Both the compiler and the debugging type can be changed to other configurations
listed above.

.. list-table::
   :widths: 20 80

   * - -DPSA_API_TEST_BUILD_PATH=<path> (optional)
     - Specifies the path to the PSA API build directory

         - ``${TFM_ROOT_DIR}/../psa-arch-tests/api-tests/BUILD`` (default)

   * - -D<PSA_API_TEST_xxx>=1 (choose exactly one)
     - Specifies the service to support
       The possible values are:

         - ``PSA_API_TEST_INITIAL_ATTESTATION``
         - ``PSA_API_TEST_CRYPTO``
         - ``PSA_API_TEST_PROTECTED_STORAGE``
         - ``PSA_API_TEST_INTERNAL_TRUSTED_STORAGE``

.. code-block:: bash

    cd <TF-M base folder>
    cd <trusted-firmware-m folder>

    mkdir <build folder>
    pushd <build folder>
    cmake ../ \
        -G"Unix Makefiles" \
        -DPROJ_CONFIG=`readlink -f ../configs/ConfigPsaApiTestIPC.cmake` \
        -DPSA_API_TEST_BUILD_PATH=../psa-arch-tests/api-tests/BUILD_ATT.GNUARM
        -DPSA_API_TEST_INITIAL_ATTESTATION=1 \
        -DTARGET_PLATFORM=psoc64 \
        -DCOMPILER=ARMCLANG \
        -DCMAKE_BUILD_TYPE=Release
    popd
    cmake --build <build folder> -- -j VERBOSE=1

The following instructions build multi-core TF-M without regression test suites
in Isolation Level 2 on Linux.
Both the compiler and the debugging type can be changed to other configurations
listed above.

.. code-block:: bash

    cd <TF-M base folder>
    cd <trusted-firmware-m folder>

    mkdir <build folder>
    pushd <build folder>
    cmake -G"Unix Makefiles" \
          -DPROJ_CONFIG=`readlink -f ../configs/ConfigCoreIPCTfmLevel2.cmake` \
          -DTARGET_PLATFORM=psoc64 \
          -DCOMPILER=ARMCLANG \
          -DCMAKE_BUILD_TYPE=Release \
          ../
    popd
    cmake --build <build folder> -- -j VERBOSE=1

The following instructions build multi-core TF-M with regression test suites
in Isolation Level 2 on Linux.
Both the compiler and the debugging type can be changed to other configurations
listed above.

.. code-block:: bash

    cd <TF-M base folder>
    cd <trusted-firmware-m folder>

    mkdir <build folder>
    pushd <build folder>
    cmake -G"Unix Makefiles" \
          -DPROJ_CONFIG=`readlink \
          -f ../configs/ConfigRegressionIPCTfmLevel2.cmake` \
          -DTARGET_PLATFORM=psoc64 \
          -DCOMPILER=ARMCLANG \
          -DCMAKE_BUILD_TYPE=Release \
          ../
    popd
    cmake --build <build folder> -- -j VERBOSE=1

The following instructions build multi-core TF-M with PSA API test suite for
the protected storage service in Isolation Level 2 on Linux.
Both the compiler and the debugging type can be changed to other configurations
listed above.

.. list-table::
   :widths: 20 80

   * - -DPSA_API_TEST_BUILD_PATH=<path> (optional)
     - Specifies the path to the PSA API build directory

         - ``${TFM_ROOT_DIR}/../psa-arch-tests/api-tests/BUILD`` (default)

   * - -D<PSA_API_TEST_xxx>=1 (choose exactly one)
     - Specifies the service to support
       The possible values are:

         - ``PSA_API_TEST_INITIAL_ATTESTATION``
         - ``PSA_API_TEST_CRYPTO``
         - ``PSA_API_TEST_PROTECTED_STORAGE``
         - ``PSA_API_TEST_INTERNAL_TRUSTED_STORAGE``

.. code-block:: bash

    cd <TF-M base folder>
    cd <trusted-firmware-m folder>

    mkdir <build folder>
    pushd <build folder>
    cmake ../ \
        -G"Unix Makefiles" \
        -DPROJ_CONFIG=`readlink -f ../configs/ConfigPsaApiTestIPCTfmLevel2.cmake` \
        -DPSA_API_TEST_BUILD_PATH=../psa-arch-tests/api-tests/BUILD_PS.GNUARM
        -DPSA_API_TEST_PROTECTED_STORAGE=1 \
        -DTARGET_PLATFORM=psoc64 \
        -DCOMPILER=ARMCLANG \
        -DCMAKE_BUILD_TYPE=Release
    popd
    cmake --build <build folder> -- -j VERBOSE=1

**********************
Signing the images
**********************

First, convert tfm_s.axf and tfm_ns.axf images to hex format. This also places
resulting files one folder level up.

GNUARM build:

.. code-block:: bash

    arm-none-eabi-objcopy -O ihex <build folder>/secure_fw/tfm_s.axf <build folder>/tfm_s.hex
    arm-none-eabi-objcopy -O ihex <build folder>/app/tfm_ns.axf <build folder>/tfm_ns.hex

ARMCLANG build:

.. code-block:: bash

    fromelf --i32 --output=<build folder>/tfm_s.hex <build folder>/secure_fw/tfm_s.axf
    fromelf --i32 --output=<build folder>/tfm_ns.hex <build folder>/app/tfm_ns.axf

Copy secure keys used in the board provisioning process to
platform/ext/target/cypress/psoc64/security/keys:

-MCUBOOT_CM0P_KEY_PRIV.pem - private OEM key for signing CM0P image
-USERAPP_CM4_KEY_PRIV.pem  - private OEM key for signing CM4 image

Note: provisioned board in SECURE claimed state is required, otherwise refer to
Cypress documentation for details on the provisioning process.

Sign the images (sign.py overwrites unsigned files with signed ones):

.. code-block:: bash

    ./platform/ext/target/cypress/psoc64/security/sign.py \
      -p platform/ext/target/cypress/psoc64/security/policy_multi_img_CM0p_CM4_debug_2M.json \
      -d cy8ckit-064b0s2-4343w \
      -s <build folder>/tfm_s.hex \
      -n <build folder>/tfm_ns.hex

Note: each image can be signed individually, for example:

.. code-block:: bash

    ./platform/ext/target/cypress/psoc64/security/sign.py \
      -p platform/ext/target/cypress/psoc64/security/policy_multi_img_CM0p_CM4_debug_2M.json \
      -d cy8ckit-064b0s2-4343w \
      -n <build folder>/tfm_ns.hex

.. code-block:: bash

    ./platform/ext/target/cypress/psoc64/security/sign.py \
      -p platform/ext/target/cypress/psoc64/security/policy_multi_img_CM0p_CM4_debug_2M.json \
      -d cy8ckit-064b0s2-4343w \
      -s <build folder>/tfm_s.hex

Running the sign.py script will result in creation of the following files:

* tfm_<s/ns>_signed.hex    - signed image for programming
* tfm_<s/ns>_unsigned.hex  - a copy of original unsigned hex file for reference
* tfm_<s/ns>_upgrade.hex   - signed image for upgrade (if device policy
  specifies upgrade slot). Flashing this image into device will
  trigger the image update. Upgrade image from the
  secondary slot will be moved to the primary slot.

**********************
Programming the Device
**********************

After building and signing, the TFM images must be programmed into flash
memory on the PSoC64 device. There are three methods to program it.

DAPLink mode
============

Using KitProg3 mode button, switch it to DAPLink mode.
Mode LED should start blinking rapidly and depending on the host computer
settings DAPLINK will be mounted as a media storage device.
Otherwise, mount it manually.

Copy tfm hex files one by one to the DAPLINK device:

.. code-block:: bash

    cp <build folder>/tfm_ns_signed.hex <mount point>/DAPLINK/; sync
    cp <build folder>/tfm_s_signed.hex <mount point>/DAPLINK/; sync

OpenOCD v.2.2
=============

Using KitProg3 mode button, switch to KitProg3 CMSIS-DAP BULK mode.
Status LED should be ON and not blinking.
To program the signed tfm_s and tfm_ns images to the device with openocd
(assuming OPENOCD_PATH is pointing at the openocd installation directory)
run the following commands:

.. code-block:: bash

    OPENOCD_PATH=<cyprogrammer dir>/openocd
    BUILD_DIR=<build folder>

    ${OPENOCD_PATH}/bin/openocd \
            -s ${OPENOCD_PATH}/scripts \
            -f interface/kitprog3.cfg \
            -c "set ENABLE_ACQUIRE 0" \
            -f target/psoc6_2m_secure.cfg \
            -c "init; reset init; flash write_image erase ${BUILD_DIR}/tfm_s_signed.hex" \
            -c "resume; reset; exit"

    ${OPENOCD_PATH}/bin/openocd \
            -s ${OPENOCD_PATH}/scripts \
            -f interface/kitprog3.cfg \
            -c "set ENABLE_ACQUIRE 0" \
            -f target/psoc6_2m_secure.cfg \
            -c "init; reset init; flash write_image erase ${BUILD_DIR}/tfm_ns_signed.hex" \
            -c "resume; reset; exit"

Optionally, erase SST partition:

.. code-block:: bash

    ${OPENOCD_PATH}/bin/openocd \
            -s ${OPENOCD_PATH}/scripts \
            -f interface/kitprog3.cfg \
            -f target/psoc6_2m_secure.cfg \
            -c "init; reset init" \
            -c "flash erase_address 0x101c0000 0x10000" \
            -c "shutdown"

Note that the ``0x101C0000`` in the command above must match the SST start
address of the secure primary image specified in the file:

    platform/ext/target/cypress/psoc64/partition/flash_layout.h

so be sure to change it if you change that file.


PyOCD v.0.23.0
==============

PyOCD v0.23.0 is installed by CySecureTools automatically. It can be used
to program TFM images into the board.

Using KitProg3 mode button, switch to KitProg3 CMSIS-DAP BULK mode.
Status LED should be ON and not blinking.
To program the signed tfm_s and tfm_ns images to the device with pyocd
run the following commands:

.. code-block:: bash

    pyocd flash  -t cy8c64xa_cm4_full_flash ${BUILD_DIR}/tfm_s_signed.hex

    pyocd flash  -t cy8c64xa_cm4_full_flash ${BUILD_DIR}/tfm_ns_signed.hex

Optionally, erase SST partition:

.. code-block:: bash

    pyocd erase -t cy8c64xa_cm4_full_flash 0x101c0000+0x10000

*Copyright (c) 2017-2020, Arm Limited. All rights reserved.*

*Copyright (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.*
