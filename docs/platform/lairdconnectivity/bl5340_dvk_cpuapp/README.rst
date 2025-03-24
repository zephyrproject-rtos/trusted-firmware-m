Laird Connectivity BL5340
=========================

The BL5340 development kit (DVK, 750-03293) is a single-board development kit
for the evaluation and development on the Laird Connectivity BL5340 module
which contains a Nordic Semiconductor nRF5340 System-on-Chip (SoC).

The BL5340 is a module with a dual-core nRF5340 SoC based on the Arm®
Cortex®-M33 architecture, with:

* a full-featured ARM Cortex-M33F core with DSP instructions, FPU, and
  ARMv8-M Security Extension, running at up to 128 MHz, referred to as
  the **Application MCU**
* a secondary ARM Cortex-M33 core, with a reduced feature set, running at
  a fixed 64 MHz, referred to as the **Network MCU**.

The BL5340 Application MCU supports the Armv8m Security Extension.

Documentation
-------------

The following links provide useful information about the BL5340

BL5340 website:
   https://www.lairdconnect.com/wireless-modules/bluetooth-modules/bluetooth-5-modules/bl5340-series-multi-core-bluetooth-52-802154-nfc-modules

Nordic Semiconductor TechDocs: https://docs.nordicsemi.com

Building TF-M on BL5340 Application MCU
---------------------------------------

To build an S and NS application image for the BL5340 Application MCU run the
following commands:

.. note::

    On OS X change ``readlink`` to ``greadlink``, available by running
    ``brew install coreutils``.

.. code:: bash

    $ mkdir build && cd build
    $ cmake -DTFM_PLATFORM=lairdconnectivity/bl5340_dvk_cpuapp \
            -DTFM_TOOLCHAIN_FILE=../toolchain_GNUARM.cmake \
            -G"Unix Makefiles" ../
    $ make install

.. note::

    Currently, applications can only be built using GCC
    (GNU ARM Embedded toolchain).

.. note::

    For BL2 (MCUBoot) logging output to be available, the project
    needs to be built with Debug configuration (CMAKE_BUILD_TYPE=Debug).

Flashing and debugging with on-board Segger J-Link
--------------------------------------------------

The BL5340 DVK is equipped with a Debug IC (Atmel ATSAM3U2C) which provides the
following functionality:

* Segger J-Link firmware and desktop tools
* SWD debug for the BL5340 module
* USB CDC ACM Serial Port bridged to the second UART peripheral
* Segger RTT Console

To install the J-Link Software and documentation pack, follow the steps below:

#. Download the appropriate package from the
`J-Link Software and documentation pack`_ website
#. Depending on your platform, install the package or run the installer
#. When connecting a J-Link-enabled board such as a BL5340 DVK, a serial port
should come up

nRF Util Installation
*********************

nRF Util allows you to control your BL5340 module from the
command line, including resetting it, erasing or programming the flash memory
and more.

To install nRF Util:

1. Visit `nRF Util product page`_.
2. Download the executable.
3. Follow the `nRF Util installation instructions`_.
4. Install ``nrfutil device`` subcommand for programming, flashing, and erasing devices:

   .. code-block:: console

      nrfutil install device

After installing, make sure that ``nrfutil.exe`` is somewhere in your executable
path to be able to invoke it from anywhere.

BL2, S, and NS application images can be flashed into BL5340 separately or may
be merged together into a single binary.

Flashing the BL5340 DVK
***********************

To program the flash with a compiled TF-M image (i.e. S, NS or both) after
having followed the instructions to install the Segger J-Link Software and the
nRF Util, follow the steps below:

Generate Intel hex files from the output binary (bin) files as follows:

.. code-block:: console

   srec_cat install/outputs/LAIRDCONNECTIVITY/BL5340_DVK_CPUAPP/tfm_s_ns_signed.bin -binary --offset=0x10000 -o install/outputs/LAIRDCONNECTIVITY/BL5340_DVK_CPUAPP/tfm_s_ns_signed.hex -intel

* Connect the micro-USB cable to the BL5340 DVK and to your computer
* Erase the flash memory in the BL5340 module:

.. code-block:: console

   nrfutil device erase --all --x-family nrf53

* Flash the BL2 and the TF-M image binaries from the sample folder of your choice:

.. code-block:: console

   nrfutil device program --x-family nrf53 --firmware <sample folder>/install/outputs/LAIRDCONNECTIVITY/BL5340_DVK_CPUAPP/bl2.hex --options chip_erase_mode=ERASE_RANGES_TOUCHED_BY_FIRMWARE
   nrfutil device program --x-family nrf53 --firmware <sample folder>/install/outputs/LAIRDCONNECTIVITY/BL5340_DVK_CPUAPP/tfm_s_ns_signed.hex --options chip_erase_mode=ERASE_RANGES_TOUCHED_BY_FIRMWARE

* Reset and start TF-M:

.. code-block:: console

   nrfutil device reset --x-family nrf53

Flashing the BL5340 DVK (Secondary slot in QSPI, with BL2)
**********************************************************

To program the flash with a compiled TF-M image (i.e. S, NS or both) after
having followed the instructions to install the Segger J-Link Software and the
nRF Util to the secondary , follow the steps below:

Generate Intel hex files from the output binary (bin) files as follows:

.. code-block:: console

   srec_cat install/outputs/LAIRDCONNECTIVITY/BL5340_DVK_CPUAPP/tfm_s_ns_signed.bin -binary --offset=0x10000000 -o install/outputs/LAIRDCONNECTIVITY/BL5340_DVK_CPUAPP/tfm_s_ns_signed_qspi.hex -intel

* Connect the micro-USB cable to the BL5340 DVK and to your computer
* Erase the flash memory in the BL5340 module:

.. code-block:: console

   nrfutil device erase --all --x-family nrf53

* Flash the BL2 and the TF-M image binaries from the sample folder of your choice:

.. code-block:: console

   nrfutil device program --x-family nrf53 --firmware <sample folder>/install/outputs/LAIRDCONNECTIVITY/BL5340_DVK_CPUAPP/bl2.hex --options chip_erase_mode=ERASE_RANGES_TOUCHED_BY_FIRMWARE
   nrfutil device program --x-family nrf53 --firmware <sample folder>/install/outputs/LAIRDCONNECTIVITY/BL5340_DVK_CPUAPP/tfm_s_ns_signed.hex --options ext_mem_erase_mode=ERASE_RANGES_TOUCHED_BY_FIRMWARE

* Reset and start TF-M:

.. code-block:: console

   nrfutil device reset --x-family nrf53


Secure UART Console on BL5340 DVK
*********************************

SECURE_UART1 is enabled by default when building TF-M on the BL5340 DVK, so the
secure firmware console output is available via USART1.

Non-Secure console output is available via USART0.

.. note::

    By default USART0 and USART1 outputs are routed to separate serial ports.

.. _nRF Util product page: https://www.nordicsemi.com/Products/Development-tools/nRF-Util/

.. _nRF Util installation instructions: https://docs.nordicsemi.com/bundle/nrfutil/page/guides/installing.html

.. _J-Link Software and documentation pack: https://www.segger.com/jlink-software.html

--------------

*Copyright (c) 2020, Nordic Semiconductor. All rights reserved.*
*Copyright (c) 2021, Laird Connectivity. All rights reserved.*
