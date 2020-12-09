trusted-firmware-m
##################

Trusted Firmware M provides a reference implementation of secure world software
for ARMv8-M. The TF-M itself is provided under a BSD-3-Clause.

.. Note::
    The software implementation contained in this project is designed to be a
    reference implementation of the Platform Security Architecture (PSA).
    It currently does not implement all the features of that architecture,
    however we expect the code to evolve along with the specifications.

This module in Zephyr has included TF-M and it's dependencies, they are:

TF-M:
    repo: https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git
    tag: TF-Mv1.2.0
    commit: c78be620c0fee08888956646b8f02fd03ab88567
    BSD-3-Clause

TF-M Tests:
    repo: https://git.trustedfirmware.org/TF-M/tf-m-tests.git
    tag: TF-Mv1.2.0
    commit: ccda809801e529250b47c9ac470cf94daef1bb1b
    license: Apache 2.0

psa-arch-tests:
    repo: https://github.com/ARM-software/psa-arch-tests
    tag: v20.03_API1.0
    commit: 3d986e72d9e084bb45763849699718f474fb10ba
    license: Apache 2.0

See also west.yml for more dependencies.
