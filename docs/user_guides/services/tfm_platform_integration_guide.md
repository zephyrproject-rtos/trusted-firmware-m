# TF-M Platform Service Integration Guide

## Introduction

TF-M Platform service is a trusted service which allows secure partitions and
non-secure applications to interact with some platform-specific components.
There are a number of features which requires some interaction with
platform-specific components which are at the same time essential for the
security of the system.
Therefore, those components need to be handled by a secure partition which is
part of the trusted compute base.

These platform-specific components include system reset, power management,
Debug, GPIO, etc.

## TF-M Platform interfaces

The TF-M interfaces for the Platform service are located in
`interface/include/`.
The TF-M Platform service source files are located in
`secure_fw/services/platform`.

## TF-M Platform service

The Platform service exposes the following interfaces:
 - `enum tfm_platform_err_t tfm_platform_system_reset(void)`

The Platform service interfaces and types are defined and documented in
`interface/include/tfm_platform_api.h`

 - `platform_sp.h/c` : These files define and implement functionalities related
   to the platform service;
 - `tfm_platform_secure_api.c` : This file implements `tfm_platform_api.h`
 functions to be called from the secure partitions. This is the entry point when
 the secure partitions request an action to the Platform service
 (e.g system reset).

## Platform HAL system reset

The Platform service service relies on a platform-specific implementation to
perform some functionalities (e.g. system reset). The platform-specific
implementation of those APIs will be located in the platform service code
section (TF-M level 3 isolation) in order to protect it from a direct call from
other secure partitions.

For API specification, please check:
`platform/include/tfm_platform_system.h`

An implementation is provided in all the supported platforms. Please,
check  `platform/ext/target/<SPECIFIC_TARGET_FOLDER>/tfm_platform_system.c`

The API **must** be implemented by the system integrators for their targets.

## Platform pin service

This service is designed to perform secure pin services of the platform
(e.g alternate function setting, pin mode setting, etc).
The veneer implementation follows IOVEC API implementation, which allows
the NS application to pack many pin service requests into one service call
to reduce the overhead of the Secure-Non-Secure context switch.
Since packing many service requests into one call is application and use-case
specific, the API implementations in tfm_platform_api.c and
tfm_platform_secure_api.c follow the one service in one veneer call design but
the service implementation in tfm_platform_system.c is prepared to serve packed
requests.

## Current Service Limitations

The system reset functionality is only supported in isolation level 1.
Currently, the mechanism by which PRoT services should run in privileged mode in
level 3, it is not in place due to an ongoing work in TF-M Core. So, the
NVIC_SystemReset call performed by the service, it is expected to generate a
memory fault when it tries to access the SCB->AIRCR register in level 3
isolation.

## Debug authentication settings

A platform may provide the option to configure debug authentication. TF-M core
calls the HAL function `void tfm_spm_hal_init_debug(void)` which configures
debug outhentication based on the following defines:
 - `DAUTH_NONE`: Debugging the system is not enabled.
 - `DAUTH_NS_ONLY`: Invasive and non invasive debugging of non-secure code is
   enabled.
 - `DAUTH_FULL`: Invasive and non-invasive debugging of non-secure and secure
   code is enabled.
 - `DAUTH_CHIP_DEFAULT`: The debug auhentication options are used that are set
   by the chip vendor.

The desired debug authentication configuration can be selected by setting one of
the options above to the cmake command with the
`-DDEBUG_AUTHENTICATION="<define>"` option. The default value of
`DEBUG_AUTHENTICATION` is `DAUTH_CHIP_DEFAULT`.

*Note*: `void tfm_spm_hal_init_debug(void)` is called during the TF-M core
initialisation phase, before initialising secure partition. This means that BL2
runs with the chip default setting.

--------------

*Copyright (c) 2019, Arm Limited. All rights reserved.*
