#!/bin/sh

TFM_GIT_SHA=9d93424a12b8cc2ba56518f76edadeaabc29e143.
TFM_TESTS_GIT_SHA=eeba9de2f5c19293038d7d551cc0ceffe32d8992..
PSA_ARCH_TESTS_GIT_SHA=cd3f3caf3a5bbb543af2206056668422f4e135b9.


if [ ! -d trusted-firmware-m/.git ]; then
   git clone https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git -b ${TFM_GIT_SHA} --bare trusted-firmware-m/.git
   cd trusted-firmware-m
   git init
   git checkout -f
   cd ..
fi

if [ ! -d tf-m-tests/.git ]; then
   git clone https://git.trustedfirmware.org/TF-M/tf-m-tests.git -b ${TFM_TESTS_GIT_SHA} --bare tf-m-tests/.git
   cd tf-m-tests
   git init
   git checkout -f
   cd ..
fi

if [ ! -d psa-arch-tests/.git ]; then
   git clone https://github.com/ARM-software/psa-arch-tests.git -b ${PSA_ARCH_TESTS_GIT_SHA} --bare psa-arch-tests/.git
   cd psa-arch-tests
   git init
   git checkout -f
   cd ..
fi

