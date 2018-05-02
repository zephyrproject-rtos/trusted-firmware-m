# TF-M integration guide
The purpose of this document is to provide a guide on how to integrate TF-M with
other hardware platforms and operating systems.
## How to build TF-M
Follow the [Build instructions](tfm_build_instruction.md).

## How to export files for building non-secure applications
Explained in the [Build instructions](tfm_build_instruction.md).

## How to add a new platform
The hardware platforms currently supported are:

* Soft Macro Model (SMM) Cortex-M33 SSE-200 subsystem for MPS2+ (AN521)
* Cortex-M23 IoT Kit subsystem for MPS2+ (AN519)
* Musca-A1 test chip board (Cortex-M33 SSE-200 subsystem)

The files related to the supported platforms are contained under the `platform`
subfolder. The platform specific files are under `platform/ext/target`, which is
organized by boards (e.g. `platform/ext/target/mps2`), while the folder
`platform/ext/common` is used to store source and header files which are
platform generic.  
More information about subsystems supported by the MPS2+ board can be found in:
[MPS2+ homepage](https://developer.arm.com/products/system-design/development-boards/fpga-prototyping-boards/mps2)  
More information about Musca-A1 test chip board can be found in:
[Musca-A1 homepage](https://developer.arm.com/products/system-design/development-boards/iot-test-chips-and-boards/musca-a1-test-chip-board)

#### generic drivers and startup/scatter files
The addition of a new platform means the creation of a new subfolder inside
`target/<board_name>` to provide an implementation of the drivers currently used
by TF-M, in particular MPC, PPC, and USART drivers. In addition to the drivers,
startup and scatter files need to be provided for the supported toolchains.
There are also board specific drivers which are used by the board platform to
interact with the external world, for example during tests, that have to be
provided, e.g. to blink LEDs or count time in the MPS2 board.

`Note: Currently SST and BL2 bootloader use different flash interface`

#### target configuration files
Inside the base root folder of the selected target, each implementation has to
provide its own copy of `target_cfg.c/.h`. This file has target specific
configuration functions and settings that are called by the TF-M during the
platform configuration step during TF-M boot. Examples of the configurations
performed during this phase are the MPC configuration, the SAU configuration,
or eventually PPC configuration if supported by the hardware platform.
Similarly, the `uart_stdout.c` is used to provide functions needed to redirect
the stdout on UART (this is currently used by TF-M to log messages).

#### platform retarget files
An important part that each new platform has to provide is the set of retarget
files which are contained inside the `retarget` folder. These files define the
peripheral base addresses for the platform, both for the secure and non-secure
aliases (when available), and bind those addresses to the base addresses used by
the devices available in the hardware platform.

## How to integrate another OS
To work with TF-M, the OS needs to support the Armv8-M architecture and,
in particular, it needs to be able to run in the non-secure world. More
information about OS migration to the Armv8-M architecture can be found in the
[OS requirements](os_migration_guide_armv8m.md). Depending upon the system
configuration this may require configuring drivers to use appropriate address
ranges.

#### interface with TF-M
The NS side is only allowed to call TF-M secure functions (veneers) from the
NS Handler mode.
For this reason, the API is a collection of SVC functions in the
`install/tfm/inc` folder. For example, the SVC interface for the Secure STorage
(SST) service is described in the file `tfm_sst_svc_handler.h` as a collection
of SVC functions which have to be registered within the SVC handler
mechanism, therefore OS needs to support user defined SVCs.
If the OS does not support user defined SVCs, it needs to be extended in
this way. Once the SVC interface functions are registered within the SVC
handler mechanism, the services can be called from the non-secure world
applications (running in Thread mode) using a wrapper API which is described in
`tfm_sst_api.h`. This API is a wrapper for the SVC interface, its purpose is
to request Handler mode through the SVC instructions encoded with the
corresponding SVC number previously registered with the SVC handler and to
handle the return value from the service to the caller.
The secure storage service also needs the NS side to provide an implementation
for the function `tfm_sst_get_cur_id()` which is used to retrieve the numerical
ID associated to the running thread. A primitive implementation is
provided in `tfm_sst_id_mngr_dummy.c`. It is system integrators responsibility
to implement the SST ID manager based on their threat model.

#### interface with non-secure world regression tests
A non-secure application that wants to run the non-secure regression tests
needs to call the `start_integ_test()`. This function is exported into the
header file `integ_test.h` inside the `install` folder structure in the test
specific files, i.e. `install/tfm/test/inc`. The non-secure regression tests are
precompiled and delivered as a static library which is available in
`install/tfm/test/lib`, so that the non-secure application needs to link against
the library to be able to invoke the `start_integ_test()` function.
The SST non-secure side regression tests rely on some OS functionality
e.g. threads, mutexes etc. These functions comply with CMSIS RTOS2 standard and
have been exported as thin wrappers defined in `os_wrapper.h` contained in
`install/tfm/test/inc`. OS needs to provide the implementation of these wrappers
to be able to run the tests.

--------------

*Copyright (c) 2017-2018, Arm Limited. All rights reserved.*
