Corstone SSE-315 with Ethos-U65 Example Subsystem for Ecosystem FVP
===================================================================

Introduction
------------

Corstone-315 is an Arm reference subsystem for secure System on Chips containing
an Armv8.1-M Cortex-M85 processor and LCM, KMU and SAM IPs. and an
Ethos-U65 neural network processor. It is an MPS4 based platform.

This platform port supports all TF-M regression tests (Secure and Non-secure)
with Isolation Level 1 and 2.

.. note::

   For Ethos-U65 IP this platform support only provides base address,
   interrupt number and an example NPU setup as non-secure, unprivileged.

.. note::

   For Armclang compiler v6.18 or later version is required.

Building TF-M
-------------

Follow the instructions in :doc:`Building instructions </building/tfm_build_instruction>`.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Build instructions with platform name: arm/mps4/corstone315
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
``-DTFM_PLATFORM=arm/mps4/corstone315``

.. note::

   The built binaries can be run on the Corstone-315 Ethos-U65 Ecosystem FVP
   (FVP_Corstone_SSE-315).

To run the example code on Corstone-315 Ethos-U65 Ecosystem FVP
---------------------------------------------------------------
FVP is available to download `here <https://developer.arm.com/tools-and-software/open-source-software/arm-platforms-software/arm-ecosystem-fvps>`__

#. Install the FVP
#. Copy ``bl1_1.bin``, ``cm_provisioning_bundle.bin``, ``dm_provisioning_bundle.bin``,
   ``bl2_signed.bin`` and ``tfm_s_ns_signed.bin``  files from
   build dir to ``<FVP installation path>/models/Linux64_GCC-6.4/``
#. Navigate to the same directory and execute the following command to start FVP::

    $ ./FVP_Corstone_SSE-315 --data "bl1_1.bin"@0x11000000
                   --data "cm_provisioning_bundle.bin"@0x12024000
                   --data "dm_provisioning_bundle.bin"@0x1202aa00
                   --data "bl2_signed.bin"@0x12031400
                   --data "tfm_s_ns_signed.bin"@0x38000000


#. After completing the procedure you should be able to see similar messages
   to this on the serial port (baud 115200 8n1)::

    Trying 127.0.0.1...
    Connected to localhost.
    Escape character is '^]'.
    [INF] Starting TF-M BL1_1
    [INF] Beginning provisioning
    [INF] TP mode set complete, system will now reset.
    [INF] Starting TF-M BL1_1
    [INF] Beginning provisioning
    [INF] Waiting for CM provisioning bundle
    [INF] Enabling secure provisioning mode, system will now reset.
    [INF] Starting TF-M BL1_1
    [INF] Beginning provisioning
    [INF] Waiting for CM provisioning bundle
    [INF] Running CM provisioning bundle
    [INF] Starting TF-M BL1_1
    [INF] Beginning provisioning
    [INF] Waiting for DM provisioning bundle
    [INF] Enabling secure provisioning mode, system will now reset.
    [INF] Starting TF-M BL1_1
    [INF] Beginning provisioning
    [INF] Waiting for DM provisioning bundle
    [INF] Running DM provisioning bundle
    [INF] Starting TF-M BL1_1
    [INF] Jumping to BL1_2
    [INF] starting TF-M bl1_2
    [INF] Attempting to boot image 0
    [INF] BL2 image decrypted successfully
    [INF] BL2 image validated successfully
    [INF] Jumping to BL2
    [INF] Starting bootloader
    [INF] Image index: 1, Swap type: none
    [INF] Image index: 0, Swap type: none
    [INF] Bootloader chainload address offset: 0x0
    [INF] Jumping to the first image slot
    Booting TF-M v2.0.0+d09934721
    Non-Secure system starting...

.. note::

   Some of the messages above are only visible when ``CMAKE_BUILD_TYPE`` is set
   to ``Debug``.

-------------

*Copyright (c) 2024, Arm Limited. All rights reserved.*
