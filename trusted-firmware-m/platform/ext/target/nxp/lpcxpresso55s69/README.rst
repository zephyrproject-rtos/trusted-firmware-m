LPCXpresso55S69
===============

Building TF-M
-------------

To build a S and NS application along with a BL2 (bootloader) image for the
LPCXpresso55S69 run the following commands:

.. code:: bash

    $ mkdir build && cd build
    $ cmake -DTFM_PLATFORM=nxp/lpcxpresso55s69 \
            -DTFM_TOOLCHAIN_FILE=../toolchain_GNUARM.cmake \
            -DCMAKE_BUILD_TYPE=Relwithdebinfo ../
    $ make install

.. Note::

    Currently ``Debug`` cannot be selected as build type and regression tests
    cannot be run on the board without modifying the flash layout due to the
    amount of available on-chip flash memory.
    To run the S and NS regression tests (``TEST_S=ON`` and ``TEST_NS=ON``) the
    secondary image areas must be set to 0 (firmware updates are not possible)
    and in parallel the size of the primary regions must be increased in the
    ``platform\ext\target\nxp\lpcxpresso55s69\partition\flash_layout.h`` file
    in order for the S and NS images to fit in the flash.

Building TF-M without BL2
-------------------------

To build a S and NS application image for the LPCXpresso55S69 run the
following commands:

.. code:: bash

    $ mkdir build && cd build
    $ cmake -DTFM_PLATFORM=nxp/lpcxpresso55s69 \
            -DTFM_TOOLCHAIN_FILE=../toolchain_GNUARM.cmake \
            -DBL2=OFF \
            ../
    $ make install

Flashing and debugging with Segger J-Link
-----------------------------------------

The LPCXpresso55S69 ships, by default, with DAPLink firmware, which may
not work reliably on Rev A2 hardware outside of the MCUXpresso or other
IDEs.

It is highly recommended to update the LPC-Link 2 debugger on the
development board with the firmware provided by Segger, which makes the
device behave as if there is an on-board J-Link debugger.

Update the LPC-Link 2 to Segger J-Link
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-  Install a recent version of the `J-Link Software and Documentation
   Pack <https://www.segger.com/downloads/jlink#J-LinkSoftwareAndDocumentationPack>`__.
   Version **6.56d** was used with this guide.

    **Warning**: The **6.60x** J-Link software package seems to have
    problems connecting to the J-Link device, and hangs with
    ``Connecting to J-Link via   USB...``. **6.56d** is recommended
    until this issue is resolved.

-  Update the on-board LPC-Link 2 to use the latest J-Link firmware,
   following the instructions from Segger: `Getting Started with
   LPC-Link
   2 <https://www.segger.com/products/debug-probes/j-link/models/other-j-links/lpc-link-2/>`__.

You can flash the **NXP LPCXpresso On-Board** firmware image with
``lpcscrypt`` from the ``lpcscrypt_2.1.0_842/scripts`` folder as
follows:

.. code:: bash

    $ ./program_JLINK ../probe_firmware/LPCXpressoV2/Firmware_JLink_LPCXpressoV2_20190404.bin

Flash images with ``JLinkExe``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Connect to the board using ``JLinkExe``:

::

    $ JLinkExe -device lpc55s69 -if swd -speed 2000 -autoconnect 1

    SEGGER J-Link Commander V6.56d (Compiled Dec 12 2019 13:03:13)
    DLL version V6.56d, compiled Dec 12 2019 13:03:00

    Connecting to J-Link via USB...O.K.
    Firmware: J-Link LPCXpresso V2 compiled Apr  4 2019 16:54:03
    Hardware version: V1.00
    S/N: 723153991
    VTref=3.300V
    Device "LPC55S69" selected.
    ...
    Cortex-M33 identified.

Flash the BL2, secure and non-secure images:

::

    J-Link> loadfile bin/bl2.hex
    J-Link> loadfile bin/tfm_s_signed.bin 0x00008000
    J-Link> loadfile bin/tfm_ns_signed.bin 0x00030000

When BL2 is disabled, generate Intel hex files from the output axf (elf)
files and then flash the secure and non-secure images:

::

    $ arm-none-eabi-objcopy -S --gap-fill 0xff -O ihex bin/tfm_s.axf tfm_s.hex
    $ arm-none-eabi-objcopy -S --gap-fill 0xff -O ihex bin/tfm_ns.axf tfm_ns.hex
    $ JLinkExe -device lpc55s69 -if swd -speed 2000 -autoconnect 1
    ....
    J-Link> loadfile tfm_s.hex
    J-Link> loadfile tfm_ns.hex

    **Note**: At present, the ``r`` (reset) command doesn't seem to
    respond, so you can reset the device to start firmware execution via
    the physical reset button.

Complete ``JLinkExe`` Build/Flash Bash Scripts
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The bash scripts in the ``platform/ext/target/nxp/lpcxpresso55s69/scripts``
folder can be saved in the ``build`` folder to rebuild and flash the BL2,
the TF-M S and NS binaries in one step.

The scripts assumes they are being run inside the ``build`` folder, which
you have previously created at ``trusted-firmware-m/build``. The build
script must be run in an empty ``build`` folder for the first time.

Debugging with Segger Ozone
---------------------------

If you have a commercially licensed Segger J-Link, or if you meet the
license terms for it's use, `Segger's cross-platform Ozone
tool <https://www.segger.com/products/development-tools/ozone-j-link-debugger/>`__
can be used to debug TF-M firmware images.

To debug, flash the BL2, S and NS firmware images using the ``flash.sh``
script or command-line options described earlier in this guide, and
configure a new project on Ozone as follows:

-  Device: LPC55S69
-  Target Interface: SWD
-  Target Interface Speed: 2 MHz
-  Host Interface: USB
-  Program File: build/secure\_fw/tfm\_s.axf (etc.)

Once the project has been setup, and the firmware has previously been
flashed to the board, connect to the target via:

-  Debug > Start Debug Session > Attach to a Running Program

At this point, you can set a breakpoint somewhere in the code, such as
in ``startup_LPC55S69_cm33_core0.s`` at the start of the
``Reset_Handler``, or near a line like ``bl    SystemInit``, or at
another appropriate location, and reset the device to debug.

Debugging with GDB
------------------

    **NOTE**: If you are debugging, make sure to set the
    ``-DCMAKE_BUILD_TYPE`` value to ``-DCMAKE_BUILD_TYPE=Debug`` when
    building TF-M so that debug information is available to GDB.

    **NOTE**: When debugging with the mbed-crypto library, you also require an
    additional ``-DMBEDCRYPTO_BUILD_TYPE=DEBUG`` compile-time switch.


Start the GDB server, pointing to the secure application image:

.. code:: bash

    JLinkGDBServer -device lpc55s69 -if swd -speed 2000

Connecting to the GDB server in ``tui`` mode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In a separate terminal, start the GDB client in ``tui`` (text UI) mode:

.. code:: bash

    $ arm-none-eabi-gdb --tui secure_fw/tfm_s.axf

Then from the client connect to the remote GDB server we started
earlier:

With ``JLinkGDBServer`` (default port 2331):

.. code:: bash

    (gdb) target remote:2331
    Remote debugging using :2331

Reset and stop at ``main``
~~~~~~~~~~~~~~~~~~~~~~~~~~

Set a breakpoint at ``main()`` (found in ``tfm_core.c``), reset the
device (``monitor reset``), and continue (``c``) execution.

::

    (gdb) break main
    Breakpoint 1 at 0x10024220: file [path]/secure_fw/core/tfm_core.c, line 189.
    (gdb) monitor reset
    (gdb) c
    Continuing.
    Note: automatically using hardware breakpoints for read-only addresses.

    Breakpoint 1, main ()
        at [path]/secure_fw/core/tfm_core.c:189
    189     tfm_arch_init_secure_msp((uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK_MSP,

Commonly used GDB commands
~~~~~~~~~~~~~~~~~~~~~~~~~~

You can start, step through, and analyse the code using some of the
following GDB commands:

+-------------------+---------------------------------------------------------+
| GDB Command       | Description                                             |
+===================+=========================================================+
| ``next``          | Execute the next statement in the program               |
+-------------------+---------------------------------------------------------+
| ``step``          | Step until new source line, entering called functions   |
+-------------------+---------------------------------------------------------+
| ``until <n>``     | Run until source line ``n`` in the current file         |
+-------------------+---------------------------------------------------------+
| ``info locals``   | Display the local variables and their current values    |
+-------------------+---------------------------------------------------------+
| ``bt``            | Display a stack backtrace up to the current function    |
+-------------------+---------------------------------------------------------+
| ``print <x>``     | Print the expression (ex. ``print my_var``)             |
+-------------------+---------------------------------------------------------+
| ``x``             | Examine memory (ex. ``x/s *my_string``)                 |
+-------------------+---------------------------------------------------------+

From here, you should consult a tutorial or book on GDB to know how to debug
common problems.

--------------

*Copyright (c) 2020, Linaro. All rights reserved.*
*Copyright (c) 2020, Arm Limited. All rights reserved.*
*SPDX-License-Identifier: BSD-3-Clause*
