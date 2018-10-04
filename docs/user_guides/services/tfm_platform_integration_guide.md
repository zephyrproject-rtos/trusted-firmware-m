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

## Current Service Limitations

The system reset functionality is only supported in isolation level 1.
Currently, the mechanism by which PRoT services should run in privileged mode in
level 3, it is not in place due to an ongoing work in TF-M Core. So, the
NVIC_SystemReset call performed by the service, it is expected to generate a
memory fault when it tries to access the SCB->AIRCR register in level 3
isolation.

--------------

*Copyright (c) 2018, Arm Limited. All rights reserved.*
