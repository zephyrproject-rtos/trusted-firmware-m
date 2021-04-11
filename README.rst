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
    tag: TF-Mv1.3.0
    commit: aa7d90a23b045e50c26ef66b11250842b665f54b
    license: BSD-3-Clause

TF-M Tests:
    repo: https://git.trustedfirmware.org/TF-M/tf-m-tests.git
    tag: TF-Mv1.3.0
    commit: e7430d13b388a106c9f4cfb534e7340ce1482575
    license: Apache 2.0

psa-arch-tests:
    repo: https://github.com/ARM-software/psa-arch-tests
    commit: b0635d9af8ba031e5d74614b2f0a811993dc122e
    license: Apache 2.0

See also west.yml for more dependencies.
