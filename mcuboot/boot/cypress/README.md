### Port of MCUBoot library to be used with Cypress targets

**Solution Description**

Given solution demonstrates operation of MCUBoot on Cypress' PSoC6 device.

There are four applications implemented:
* MCUBootApp - PSoC6 MCUBoot-based bootloading application;
* BlinkyApp - simple PSoC6 blinking LED application which is a target of BOOT/UPGRADE;

The default flash map for MCUBootApp implemented is next:

* [0x10000000, 0x10018000] - MCUBootApp (bootloader) area;
* [0x10018000, 0x10028000] - primary slot for BlinkyApp;
* [0x10028000, 0x10038000] - secondary slot for BlinkyApp;
* [0x10038000, 0x10039000] - scratch area;

The flash map is defined through sysflash.h and cy_flash_map.c.

MCUBootApp checks image integrity with SHA256, image authenticity with EC256 digital signature verification and uses completely SW implementation of cryptographic functions based on mbedTLS Library.

**Downloading Solution's Assets**

There is a set assets required:

* MCUBooot Library (root repository)
* PSoC6 Peripheral Drivers Library (PDL)
* mbedTLS Cryptographic Library

Those are represented as submodules.

To retrieve source code with subsequent submodules pull:

    git clone --recursive https://github.com/JuulLabs-OSS/mcuboot.git

Submodules can also be updated and initialized separately:

    cd mcuboot
    git submodule update --init --recursive



**Building Solution**

Root directory for build is **boot/cypress.**

This folder contains make files infrastructure for building both MCUBoot Bootloader and sample BlinkyApp application used for Bootloader demo functionality.

Instructions on how to build and upload MCUBootApp bootloader application and sample user applocation are located in `Readme.md` files in corresponding folders.

**Currently supported platforms:**

* PSOC_062_2M - for MCUBoot, BlinkyApp;

**Build environment troubleshooting:**

Following CLI / IDE are supported for project build:

* Cygwin on Windows systems
* unix style shells on *nix systems
* Eclipse / ModusToolbox ("makefile project from existing source")

*Make* - make sure it is added to system's `PATH` variable and correct path is first in the list;

*Python/Python3* - make sure you have correct path referenced in `PATH`;

*Msys2* - to use systems PATH navigate to msys2 folder, open `msys2_shell.cmd`, uncomment set `MSYS2_PATH_TYPE=inherit`, restart MSYS2 shell.

This will inherit system's PATH so should find `python3.7` installed in regular way as well as imgtool and its dependencies.

