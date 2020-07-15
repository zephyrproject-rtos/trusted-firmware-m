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

TF-M:
    repo: https://git.trustedfirmware.org/trusted-firmware-m.git
    tag: TF-Mv1.1
    commit: a6b336c1509fd5f5522450e3cec0fcd6c060f9c8
    BSD-3-Clause

TF-M Tests:
    repo: https://git.trustedfirmware.org/TF-M/tf-m-tests.git
    tag: TF-Mv1.1
    commit: 5a571808e7841f15cc966661a64dd6adb3b40f6c
    license: Apache 2.0

mcuboot:
    repo: https://github.com/JuulLabs-OSS/mcuboot.git
    tag: v1.6.0
    commit: 50d24a57516f558dac72bef634723b60c5cfb46b
    license: Apache 2.0

mbed-crypto:
    repo: https://github.com/ARMmbed/mbed-crypto.git
    tag: 3.0.1
    commit: 1146b4e06011b69a6437e6b728f2af043a06ec19
    license: Apache 2.0

psa-arch-tests:
    repo: https://github.com/ARM-software/psa-arch-tests
    tag: v20.03_API1.0
    commit: 3d986e72d9e084bb45763849699718f474fb10ba
    license: Apache 2.0
