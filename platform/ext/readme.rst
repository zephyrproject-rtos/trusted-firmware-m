###################################
Details for the platform/ext folder
###################################
This folder has code that has been imported from other projects. This means the
files in this folder and subfolders have Apache 2.0 license which is different
to BSD 3.0 license applied to the parent TF-M project.

.. Note::
    This folder is strictly Apache 2.0 with the exception of cmake files.
    Maintainers should be consulted if this needs to be revisited.

***********
Sub-folders
***********

cmsis
=====
This folder contains core and compiler specific header files imported from the
``CMSIS_5`` project.

common
======
This folder contains stdout redirection to UART, a temporary memory mapped flash
implementation for the bootloader and tfm\_mbedtls\_config.h for all the
targets.

drivers
=======
This folder contains the headers with CMSIS compliant driver definitions that
that TF-M project expects a target to provide.

target_cfg.h
------------
This file is expected to define the following macros respectively.

- ``TFM_DRIVER_STDIO`` - This macro should expand to a structure of type
  ``ARM_DRIVER_USART``. TFM redirects its standard input and output to this
  instance of USART.
- ``NS_DRIVER_STDIO`` - This macro should expand to a structure of type
  ``ARM_DRIVER_USART``. Non-Secure application redirects its standard input and
  output to this instance of USART.

target
======
This folder contains the files for individual target.

Flash layout header file
------------------------
Target must provide a header file, called ``flash_layout.h``, which defines the
information explained in the follow subsections. The defines must be named
as they are in the subsections.

BL2 bootloader
^^^^^^^^^^^^^^
The BL2 bootloader requires the following definitions:

- ``FLASH_BASE_ADDRESS`` - Defines the first valid address in the flash.
- ``FLASH_AREA_BL2_OFFSET`` - Defines the offset from the flash base address
  where the BL2 - MCUBOOT area starts.
- ``FLASH_AREA_BL2_SIZE`` - Defines the size of the BL2 area.
- ``FLASH_AREA_IMAGE_PRIMARY_OFFSET`` - Defines the offset from the flash base
  address where the primary image area starts, which hosts the active firmware
  image.
- ``FLASH_AREA_IMAGE_PRIMARY_SIZE`` - Defines the size of the primary image
  area.
- ``FLASH_AREA_IMAGE_SECONDARY_OFFSET`` - Defines the offset from the flash base
  address where the secondary image area starts, which is a placeholder for new
  firmware images.
- ``FLASH_AREA_IMAGE_SECONDARY_SIZE`` - Defines the size of the secondary image
  area.
- ``FLASH_AREA_IMAGE_SCRATCH_OFFSET`` - Defines the offset from the flash base
  address where the scratch area starts, which is used during image swapping.
- ``FLASH_AREA_IMAGE_SCRATCH_SIZE`` - Defines the size of the scratch area. The
  minimal size must be as the biggest sector size in the flash.
- ``FLASH_DEV_NAME`` - Specifies the flash device used by BL2.

Assemble tool
^^^^^^^^^^^^^
The ``assemble.py`` tool is used to concatenate secure and non-secure binary
to a single binary blob. It requires the following definitions:

- ``SECURE_IMAGE_OFFSET`` - Defines the offset from the single binary blob base
  address, where the secure image starts.
- ``SECURE_IMAGE_MAX_SIZE`` - Defines the maximum size of the secure image area.
- ``NON_SECURE_IMAGE_OFFSET`` - Defines the offset from the single binary blob
  base address,   where the non-secure image starts.
- ``NON_SECURE_IMAGE_MAX_SIZE`` - Defines the maximum size of the non-secure
  image area.

Image tool
^^^^^^^^^^^^^
The ``imgtool.py`` tool is used to handle the tasks related to signing the
binary. It requires the following definitions:

- ``SIGN_BIN_SIZE`` - Defines the size of the concatenated binary that is to
  be signed.
- ``IMAGE_LOAD_ADDRESS`` - Defines the address to where the image is loaded
  and is executed from. Only used in case the ``MCUBOOT_UPGRADE_STRATEGY``
  is configured to be ``RAM_LOADING``.

Secure Storage (SST) Service definitions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The SST service requires the following definitions:

- ``SST_FLASH_AREA_ADDR`` - Defines the flash area address where the secure
  store area starts.
- ``SST_SECTOR_SIZE`` - Defines the size of the flash sectors.
- ``SST_NBR_OF_SECTORS`` - Defines the number of sectors available for the
  secure area.
- ``SST_FLASH_DEV_NAME`` - Specifies the flash device used by SST to store the
  data.
- ``SST_FLASH_PROGRAM_UNIT`` - Defines the smallest flash programmable unit in
  bytes.
- ``SST_MAX_ASSET_SIZE`` - Defines the maximum asset size to be stored in the
  SST area.
- ``SST_NUM_ASSETS`` - Defines the maximum number of assets to be stored in the
  SST area.

.. Note::

    The sectors must be consecutive.

***************************************
Expose target support for HW components
***************************************
Services may require HW components to be supported by the target to enable some
features (e.g. SST service with rollback protection, etc). The following
definitions need to be set in the .cmake file if the target has the following
HW components:

- ``TARGET_NV_COUNTERS_ENABLE`` - Specifies that the target has non-volatile
  (NV) counters.

--------------

*Copyright (c) 2017-2019, Arm Limited. All rights reserved.*
