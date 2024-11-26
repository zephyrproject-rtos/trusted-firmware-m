###############
RSE FWU Metdata
###############

As per the PSA firmware update architecture specification `_DEN0118
<https://developer.arm.com/documentation/den0118/latest/>`_ , fwu metadata and
private metdata is a collection of fields that primarily serves as an information
exchange channel between the firwmare update agent and the early stage bootloader.
Current code implements version 2 of the metadata as per the spec section A3.2.2.

For RSE, if option ``RSE_GPT_SUPPORT`` is enabled, then the firmware store
expects the GPT image with following layout:

+-------------------------------------------+
| Protective MBR                            |
+-------------------------------------------+
| Primary GPT Header                        |
+-------------------------------------------+
| GUID Partition Table                      |
|   +---------------------------------+     |
|   | FIP_A                           |     |
|   +---------------------------------+     |
|   | FIP_B                           |     |
|   +---------------------------------+     |
|   | FWU-Metadata                    |     |
|   +---------------------------------+     |
|   | Bkup-FWU-Metadata               |     |
|   +---------------------------------+     |
|   | private_metadata_1              |     |
|   +---------------------------------+     |
+-------------------------------------------+
| Backup Partition Table                    |
+-------------------------------------------+
| Secondary GPT Header                      |
+-------------------------------------------+

As per section A1.1. *Platform Boot* of the spec, the mechanism to
determine a failed boot attempt is platform specific. To meet this requirement,
a private metadata partition has been added to the RSE platform.

The partitions mentioned above are parsed during the early boot stages,
specifically BL1_2 and BL2, with BL2 being the first stage capable of protocol
updates. If any of the metadata partitions are missing, the bootloader enters an
error state and triggers a panic.

Upon locating the metadata during the parsing of the GPT image, its contents are
used to determine the appropriate bank from which the image should be loaded.

Failure and recovery actions
----------------------------

For any reason, if a failure is encountered while loading the image from the
bank pointed by metadata, then the *failed_boot_count* for corresponding bank is
incremented in the private metadata. And if the *failed_boot_count* exceeds the
``FWU_MAX_FAILED_BOOT``, then an image is restored from the the last known bank
which had a successful boot.

--------------

*SPDX-License-Identifier: BSD-3-Clause*

*SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors*
