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
    commit: 9d93424a12b8cc2ba56518f76edadeaabc29e143
    license: BSD-3-Clause

TF-M Tests:
    repo: https://git.trustedfirmware.org/TF-M/tf-m-tests.git
    commit: eeba9de2f5c19293038d7d551cc0ceffe32d8992..
    license: Apache 2.0

psa-arch-tests:
    repo: https://github.com/ARM-software/psa-arch-tests
    commit: cd3f3caf3a5bbb543af2206056668422f4e135b9.
    license: Apache 2.0

See also west.yml for more dependencies.
