# TF-M glossary of terms and abbreviations

| Term | Abbrev. | Description |
| --- | --- | --- |
| ** TF-M related ** | | |
| Trusted Firmware for M-class | TF-M | ARM TF-M provides a reference implementation of secure world software for ARMv8-M. |
| Trusted Firmware for M-class | TFM | ARM TF-M provides a reference implementation of secure world software for ARMv8-M. |
| Secure Processing Environment | SPE | PSA term. In TF-M this means the secure domain protected by TF-M|
| Non Secure Processing Enviroment| NSPE | PSA term. In TF-M this means non secure domain typically running an OS using services provided by TF-M|
| Secure Service | SS | A component within the TEE that is atomic from a security/trust point of view, i.e. which is viewed as a single entity from a TF-M point of view |
| Secure Partition | SP | A logical container for a single secure service |
| Secure Partition Manager | SPM | The TF-M component responsible for enumeration, management and isolation of multiple Secure Partitions within the TEE |
| Secure Function | SFN | An entry function to a secure service. Multiple SFN per SS are permitted |
| Secure Storage Service | SST | Secure storage service provided by TF-M |
| ** SSE-200 platform ** | | |
| Memory Protection Controller | MPC | Bus slave-side security controller for memory regions |
| Peripheral Protection Controller | PPC | Bus slave-side security controller for peripheral access |
| ** v8M-specific ** | | |
| Secure/Non-secure | S/NS | The separation provided by TrustZone hardware components in the system |
| Secure Attribution Unit | SAU | Hardware component providing isolation between Secure, Non-secure Callable and Non-secure addresses |
| ** M-class Generic ** | | |
| ARM Architecture Procedure Call Standard | AAPCS | The AAPCS defines how subroutines can be separately written, separately compiled, and separately assembled to work together. It describes a contract between a calling routine and a called routine |
| SuperVisor Call | SVC | ARMv7M assembly instruction to call a privileged handler function |
| Memory Protection Unit | MPU | Hardware component providing privilege control |

--------------

*Copyright (c) 2017, Arm Limited. All rights reserved.*
