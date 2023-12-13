#######
Roadmap
#######

TF-M has been under active development since it was launched in Q1'18. It is
being designed to include

1. Secure boot ensuring integrity of runtime images and responsible for firmware upgrade.
2. Runtime firmware consisting of TF-M Core responsible for secure isolation,
   execution and communication aspects. and a set of Secure Services providing
   services to the Non-Secure and Secure Applications. The secures services
   currently supported are Secure Storage, Cryptography, Firmware Update,
   Attestation and Platform Services

If you are interested in collaborating on any of the roadmap features or other
features, please mail TF-M mailing list

******************
Supported Features
******************
- PSA Firmware Framework v1.0, 1.1 Extension including IPC and SFN modes.
- PSA Level1, 2 and 3 Isolation.
- Secure Boot (mcuboot upstream) including generic fault injection mitigations
- PSA Protected Storage, Internal Trusted Storage v1.0 and Encrypted ITS
- PSA Cryptov1.0 (uses Mbed TLS v3.4.0)
- PSA Initial Attestation Service v1.0
- PSA Firmware Update v1.0
- PSA ADAC Specification Implementation
- Base Config
- kconfig based configuration
- Profile Small, Medium, ARoT-less Medium, Large
- Secure Partition Interrupt Handling, Pre-emption of SPE execution
- Platform Reset Service
- Dual CPU
- Open Continuous Integration (CI) System
- Boot and Runtime Crypto Hardware Integration
- Fault Injection Handling library to mitigate against physical attacks
- Threat Model
- Arm v8.1-M Privileged Execute Never (PXN) attribute and Thread reentrancy disabled (TRD)
- FPU, MVE Support
- CC-312 PSA Cryptoprocessor Driver Interface

******
CQ4'23
******
- TF-M v2.0.0 release
- Mbed TLS 3.5.0, mcuboot 2.0.0 Integration
- Design, prototype: Supporting multiple clients i.e. TF-M supporting multiple on
  core and off core clients on heterogeneous (e.g. Cortex-A + Cortex-M platforms)
- Demonstrating TLS in Non-Secure using PSA Crypto APIs in TF-M
- Build System Enhancements - Separate Secure, Non-Secure Builds
- Mailbox interrupt handling

******
Future
******
- Long Term Stable (LTS) support
- Implement support for multiple clients
- Remote Test Infrastructure
- MISRA testing
- TF-M Performance - Further Benchmarking and Optimization
- Scheduler - Multiple Secure Context Implementation
- Arm v8.1-M Architecture Enablement - PAC/BTI
- PSA FWU Service Enhancements
- PSA ADAC Spec - Enhancements and Testing
- Arm v8.1-M Unprevileged Debug
- [Secure Storage] Extended PSA APIs, Key Diversification Enhancements
- [Audit Logs] Secure Storage, Policy Manager
- PSA FF Lifecycle API
- Fuzz Testing

--------------

*Copyright (c) 2017-2023, Arm Limited. All rights reserved.*
