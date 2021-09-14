######################
Floating-Point Support
######################

TF-M adds several configuration flags to control Floating point (FP) [1]_
support in TF-M Secure Processing Environment (SPE).
Non Secure Processing Environment (NSPE) is not allowed to access Floating
Point Unit (FPU) [2]_ when FP support is enabled in SPE at current stage.

* Support GNU Arm Embedded Toolchain [3]_.
* Support FP Application Binary Interface (ABI) [4]_ types: software, hybird,
  hardware.
* Support lazy stacking on/off.
* Support Inter-Process Communication (IPC) [5]_ model in TF-M, and doesn't
  support LIBRARY model.
* Support Armv8-M [6]_ mainline or later.
* Support isolation level 1,2,3.

FP support is enabled on Arm musca S1 [7]_ platform as a reference
implementation. Please refer to musca s1 platform code when FP support is
enabled on other platforms.

Secure libraries are compiled with ``COMPILER_CP_FLAG`` and linked with
``LINKER_CP_OPTION`` for different FP ABI types. All those libraries shall be
built with the same FP ABI type. Otherwise, linking errors may occur due to FP
ABI type conflicts.

If FP ABI types mismatch error is generated during build, pleae check whether
the library is compiled with ``COMPILER_CP_FLAG``.
Example:

.. code-block:: cmake

      target_compile_options(lib
          PRIVATE
              ${COMPILER_CP_FLAG}
      )

===================================
CMake configurations for FP support
===================================
The following CMake configurations configure ``COMPILER_CP_FLAG`` in TF-M SPE.

* ``CONFIG_TFM_SPE_FP`` are used to configure FP ABI type for secure side.

  +-------------------+---------------------------+
  | CONFIG_TFM_SPE_FP | FP ABI type [2]_ [3]_     |
  +===================+===========================+
  | 0 (default)       | Software                  |
  +-------------------+---------------------------+
  | 1                 | Hybird                    |
  +-------------------+---------------------------+
  | 2                 | Hardware                  |
  +-------------------+---------------------------+

  FP software ABI type is default for secure side in TF-M (mfloat-abi=soft).

* ``CONFIG_TFM_LAZY_STACKING_SPE`` is used to enable/disable lazy stacking
  feature from secure side. This feature is only valid for FP hardware or
  hybird option.

  +------------------------------+---------------------------+
  | CONFIG_TFM_LAZY_STACKING_SPE | Description               |
  +==============================+===========================+
  | 0FF                          | Disable lazy stacking     |
  +------------------------------+---------------------------+
  | ON (default)                 | Enable lazy stacking      |
  +------------------------------+---------------------------+

* ``CONFIG_TFM_FP_ARCH`` specifies which FP architecture is available on the
  target, valid for FP hardware or hybird option.

  FP architecture is processor dependent. For GNUARM compiler, example value
  are: auto, fpv5-d16, fpv5-sp-d16, etc.

  Default value of ``CONFIG_TFM_FP_ARCH`` for GNUARM compiler is fpv5-sp-d16.

  This parameter shall be specified by platform. Please check compiler
  reference manual and processor hardware manual for more details to set
  correct FPU configuration for platform.


*********
Reference
*********
.. [1] `High-Performance Hardware Support for Floating-Point Operations <https://www.arm.com/why-arm/technologies/floating-point>`_

.. [2] `Cortex-M4 Technical Reference Manual <https://developer.arm.com/documentation/ddi0439/b/Floating-Point-Unit/About-the-FPU>`_

.. [3] `GNU Arm Embedded Toolchain <https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm>`_

.. [4] `Float Point ABI <https://www.keil.com/support/man/docs/armclang_ref/armclang_ref_chr1417451577871.htm>`_

.. [5] :doc:`TF-M Inter-Process Communication </docs/technical_references/design_docs/tfm_psa_inter_process_communication>`

.. [6] `Armv8-M Architecture Reference Manual <https://developer.arm.com/documentation/ddi0553/latest>`_

.. [7] `Musca-S1 Test Chip Board <https://developer.arm.com/tools-and-software/development-boards/iot-test-chips-and-boards/musca-s1-test-chip-board>`_


--------------

*Copyright (c) 2021, Arm Limited. All rights reserved.*
