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
    commit: c05319dbb991d153081d5bf5314b8a635063b252
    license: BSD-3-Clause

TF-M Tests:
    repo: https://git.trustedfirmware.org/TF-M/tf-m-tests.git
    tag: TF-Mv1.3.0
    commit: d2c78e76df1783fc70ec902313daad6f44288d23.
    license: Apache 2.0

psa-arch-tests:
    repo: https://github.com/ARM-software/psa-arch-tests
    commit: b0635d9af8ba031e5d74614b2f0a811993dc122e
    license: Apache 2.0

See also west.yml for more dependencies.
