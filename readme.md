# Trusted Firmware M - v0.1

Trusted Firmware M provides a reference implementation of secure world
software for ARMv8-M.

*Note:* The software implementation contained in this project is designed to
be a reference implementation of the Arm Platform Security Architecture (PSA).
It currently does not implement all the features of that architecture, however
we expect the code to evolve over 2018 along with the specifications.

`Terms 'TFM' and 'TF-M' are commonly used in documents and code and both
refer to Trusted Firmware M.`

[Glossary](glossary.md) has the list of terms and abbreviations.

## License

The software is provided under a BSD-3-Clause [License](license.md).
Contributions to this project are accepted under the same license with developer
sign-off as described in the [Contributing Guidelines](contributing.md).

This project contains code from other projects as listed below. The code from
external projects is limited to `app` and `platform` folders.
The original license text is included in those source files.

* The platform folder currently contains drivers imported from external project
  and the files have Apache 2.0 license.
* The app folder contains files imported from CMSIS_5 project
  and the files have Apache 2.0 license.

*Note* Any code that has license other than BSD-3-Clause is kept in
specific sub folders named `ext` so that it can isolated if required.

## This Release

This release includes

* A Secure FW with support for PSA Level 1 isolation on ARMv8M.
* The Interfaces exposed by the Secure FW to NS side.
* A blocking secure fw model with NS application example.
* Example secure service (or secure function) running within this SPE.
	* Only secure storage service is supported in this release.
* Testcases running baremetal and with RTX to test the functionality.
* Basic support for higher level isolation but it is `in progress with
limited testing`.
* Build system based on cmake and armclang.

### in progress

* GCC support
* Ongoing and incremental support for PSA features.
	* Level 3 PSA isolation
	* PSA Crypto API support
	* PSA IPC support
	* ...
* OS support and use case examples.
	* mbed OS upstream support
	* mbed cloud client examples
	* ...
* Additional platform support.
	* Musca test chip
	* Cortex M23 support
	* ...
* Ongoing security hardening, optimization and quality improvements.


### Platforms

Current release has been tested on

* Cortex M33 based SSE-200 system -
	* [FPGA running on MPS2 board.](https://developer.arm.com/products/system-design/development-boards/cortex-m-prototyping-systems/mps2)
	* [Fast model FVP_MPS2_AEMv8M.](https://developer.arm.com/products/system-design/fixed-virtual-platforms)

## Getting Started

### Prerequisite
Trusted Firmware M provides a reference implementation of PSA specifications.
It is assumed that the reader is familiar with PSA concepts and terms.
PSA specifications are currently not available in the public domain.

The current TF-M implementation specifically targets TrustZone for ARMv8-M so a
good understanding of the v8-M architecture is also necessary.
A good place to get started with ARMv8-M is
[develeper.arm.com](https://developer.arm.com/technologies/trustzone).


### Really getting started

Trusted Firmware M source code is available on
[github](https://github.com/ARM-software/trusted-firmware-m)

To build & run TF-M
- Follow the
 [SW requirements guide](docs/user_guides/tfm_sw_requirement.md)
 to set up your environment
- Follow the
 [Build instructions](docs/user_guides/tfm_build_instruction.md)
 to compile and build the TF-M source
- Follow the
 [User guide](docs/user_guides/tfm_user_guide.md)
for information on running the example

To port TF-M to a another system or OS, follow the
[OS Integration Guide](docs/user_guides/tfm_integration_guide.md)

Please also see the [glossary](glossary.md) of terms used in the project.

[Contributing Guidelines](contributing.md) contains guidance on how to
contribute to this project.

Further documents can be found in the [docs](docs) folder.

## Feedback and support

For this early access release, feedback is requested via email to
[support-trustedfirmware-m@arm.com ](support-trustedfirmware-m@arm.com).


## Version history

| Version | Date | Description |
|---------|------|-------------|
| 0.1   | 2017-12-15 | Early Access Release |


--------------

*Copyright (c) 2017, Arm Limited. All rights reserved.*
