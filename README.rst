trusted-firmware-m
##################

Trusted Firmware M provides a reference implementation of secure world software
for ARMv8-M. The TF-M itself is provided under a BSD-3-Clause.

.. Note::
    The software implementation contained in this project is designed to be a
    reference implementation of the Platform Security Architecture (PSA).
    It currently does not implement all the features of that architecture,
    however we expect the code to evolve along with the specifications.


This module in Zephyr has include TF-M and it's dependencies, they are:

TF-Mv1.0-RC2: https://git.trustedfirmware.org/trusted-firmware-m.git/tag/?h=TF-Mv1.0-RC2
    Commit: 4117e0353c55dce739f550515c5b007411142ba7

psa-arch-tests: https://github.com/ARM-software/psa-arch-tests/tree/v19.06_API0.9
    Commit: b6220d145c5220ff0fc265703573d8365d7564b8

mbedtls 2.7.9: https://github.com/ARMmbed/mbedtls.git
    Commit: 3187e7ca986fe199313343b0c810e41b543ef78a

mbed crypto 1.1.0: https://github.com/ARMmbed/mbed-crypto.git
    Commit: 47f2de132936905d97a93e2ddf7f5237ab232fbe

CMSIS_5.2: https://github.com/ARM-software/CMSIS_5.git
    Commit: 80cc44bba16cb4c8f495b7aa9709d41ac50e9529
