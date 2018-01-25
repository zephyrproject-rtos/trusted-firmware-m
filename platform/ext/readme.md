# Details for the platform/ext folder

This folder has code that has been imported from other projects. This means the
files in this folder and subfolders have Apache 2.0 license which
is different to BSD 3.0 license applied to the parent TF-M project.


`NOTE` This folder is strictly Apache 2.0 with the exception of cmake files.
Maintainers should be consulted if this needs to be revisited.

## Sub-folders

### cmsis

This folder contains core and compiler specific header files imported from the
CMSIS_5 project.

### common
This folder contains stdout redirection to UART, a temporary memory mapped
flash implementation for the bootloader and mbedtls_config.h for all
the targets.

### drivers

This folder contains the headers with CMSIS compliant driver definitions that
that TF-M project expects a target to provide.

### target
This folder contains the files for individual target.


--------------

*Copyright (c) 2017-2018, Arm Limited. All rights reserved.*
