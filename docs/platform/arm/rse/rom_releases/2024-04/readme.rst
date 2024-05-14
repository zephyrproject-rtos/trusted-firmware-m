**************************
RSE ROM Release 2024-04-08
**************************

.. contents:: Contents
    :depth: 1
    :local:

Features
========

- A ROM bootloader for RSE that supports chainloading a second immutable boot
  stage (BL1-2) from OTP.
- Integrity protection of the BL1-2 boot image using a provisioned SHA-256 hash
  of the image.
- Hardware-accelerated crypto operations using the built-in CryptoCell-3xx ROM
  driver.
- Support for hardware lifecycle management and provisioning using code and
  values loaded through a debugger.
- A DMA Initial Command Sequence placed at offset 0x1F000 in the ROM.
- Support for secure provisioning, using AEAD encryption with keys derived from
  the RTL key.
- Allows for setup of SAM configuration with provisioned values before CPU is
  started via the DMA ICS.
- Uses TRAM for BL1_1 data sections.

Known errata
============

- The build instructions for this release initially contained an incorrect
  address for the DMA ICS concatenation with the ROM code. The build
  instructions in this document have been updated and are now correct. The
  correct address is 0x1F000, instead of 0x1E000. Some distributed ROM binaries
  have also been impacted, and hence will not boot.

  If a ROM binary does not boot, it can be checked for this error using the
  command:

  .. code-block:: bash

    xxd -s 0x1F000 rom.bin

  If there is no output, or the output is all zero-words, then the ROM binary is
  affected by the issue. It is possible to fix a ROM binary affected by the
  issue by running the following command:

  .. code-block:: bash

    dd if=rom.bin bs=1k skip=120 seek=124 count=4 of=rom.bin

TF-M version
============

The RSE ROM image should be generated from TF-M commit hash:

.. code-block:: bash

    2ca8c58dc2feddc93b87585ffc07e4d169f54278

Tested Environment
==================

- arm-none-eabi-gcc --version | head -n 1

.. code-block:: bash

    arm-none-eabi-gcc (GNU Arm Embedded Toolchain 10.3-2021.10) 10.3.1 20210824 (release)

- python3 --version

.. code-block:: bash

    Python 3.11.6

- python3 -m pip list

.. code-block:: bash

    Package                   Version
    ------------------------- ---------
    attrs                     23.2.0
    cbor2                     5.6.2
    cffi                      1.16.0
    click                     8.1.7
    cryptography              42.0.5
    ecdsa                     0.18.0
    imgtool                   2.0.0
    intelhex                  2.3.0
    Jinja2                    3.1.3
    jsonschema                4.21.1
    jsonschema-specifications 2023.12.1
    kconfiglib                14.1.0
    MarkupSafe                2.1.5
    networkx                  3.3
    pip                       23.2
    pyasn1                    0.6.0
    pycparser                 2.22
    pyhsslms                  2.0.0
    PyYAML                    6.0.1
    referencing               0.34.0
    rpds-py                   0.18.0
    setuptools                68.1.2
    six                       1.16.0

Build command
=============

The RSE ROM image should be generated with the following build commands:

.. code-block:: bash

    python3 -m venv ./venv
    source ./venv/bin/activate

    python3 -m pip install -r ./tools/requirements.txt

    cmake -S . -B build -DTFM_PLATFORM=arm/rse/tc \
        -DTFM_TOOLCHAIN_FILE=toolchain_GNUARM.cmake \
        -DCMAKE_BUILD_TYPE=minsizerel \
        -DRSE_ENABLE_TRAM=ON \
        -DRSE_OTP_TRNG=ON \
        -DTFM_DUMMY_PROVISIONING=OFF \
        -DRSE_XIP=ON \
        -DTFM_PARTITION_DPE=OFF

    cmake --build build -- install

    srec_cat build/bin/bl1_1.bin  -Binary -offset 0x0 \
        build/bin/rom_dma_ics.bin -Binary -offset 0x1F000 \
        -o rse_rom_2024-04-08.bin -Binary

    truncate --size 131072 rse_rom_2024-04-08.bin

--------------

*Copyright (c) 2024, Arm Limited. All rights reserved.*
