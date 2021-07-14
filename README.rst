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
    commit: a199c3047f320a2f82b9a0c27af5b50991184e0f
    license: BSD-3-Clause

TF-M Tests:
    repo: https://git.trustedfirmware.org/TF-M/tf-m-tests.git
    commit: b0c06b46b8a32d73794637e2a034a9cfbb4e883b
    license: Apache 2.0

psa-arch-tests:
    repo: https://github.com/ARM-software/psa-arch-tests
    commit: 02d145d80da31bc78ce486bda99ad55597fea0ca
    license: Apache 2.0

See also west.yml for more dependencies.
