.. _Kconfig_system:

##################
The Kconfig System
##################
The Kconfig system is an additional tool for users to change configuration options of TF-M.

.. figure:: kconfig_header_file_system.png

It handles dependencies and validations automatically when you change configurations so that the
generated configuration options are always valid.

It consists of `The Kconfig tool`_ and the `The Kconfig files`_.

****************
The Kconfig tool
****************
The Kconfig tool is a python script based on `Kconfiglib <https://github.com/ulfalizer/Kconfiglib>`__
to launch the menuconfig interfaces and generate the following config files:

- CMake config file

  Contains CMake cache variables of building options.
  This file should be passed to the build system via command line option ``TFM_EXTRA_CONFIG_PATH``.

- Header file

  Contains component options in the header file system.
  This file should be passed to the build system via the command line option ``PROJECT_CONFIG_HEADER_FILE``.

- The .config file

  The ``.config`` file which contains all the above configurations in the Kconfig format.
  It is only used to allow users to make adjustments basing on the previous settings.
  The Kconfig tool will load it if it exists.

How To Use
==========
The script takes three arguments at maximum.

- '-k', '--kconfig-file'

  Required. The root Kconfig file.

- '-u', '--ui'

  Optional. The menuconfig interface to launch, ``gui`` or ``tui``.
  Refer to `Menuconfig interfaces <https://github.com/ulfalizer/Kconfiglib#menuconfig-interfaces>`__
  for interface details. Only the first two are supported.
  If no UI is selected, the tool generates config files with default values.

- '-o', '--output_path'

  Required. The output directory to hold the generated files.

The script can be used as a standalone tool.
You can pass the config files to build system via command line option ``TFM_EXTRA_CONFIG_PATH`` and
``PROJECT_CONFIG_HEADER_FILE`` respectively, as mentioned above.

The TF-M build system has also integrated the tool.
You only need to set ``USE_KCONFIG_TOOL`` to ``ON/TRUE/1`` and CMake will launch the GUI menuconfig
for users to adjust configurations and automatically load the generated config files.

.. note::

  - Only GUI menuconfig can be launched by CMake for the time being.
  - Due to the current limitation of the tool, you are not allowed to change the values of build
    options that of which platforms have customized values. And there is no prompt messages either.

*****************
The Kconfig files
*****************
The Kconfig files are the files written in the
`Kconfig language <https://www.kernel.org/doc/html/latest/kbuild/kconfig-language.html#kconfig-language>`__
to describe config options.
They also uses some Kconfiglib extensions such as ``osource`` and ``rsource`` so they can only work
with the Kconfiglib.

Component options are gathered together in a seperate menu ``TF-M component configs`` because these
options are changed less frequently.

--------------

*Copyright (c) 2022, Arm Limited. All rights reserved.*
