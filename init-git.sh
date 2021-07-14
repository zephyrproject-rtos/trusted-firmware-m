#!/bin/sh

TFM_GIT_SHA=a199c3047f320a2f82b9a0c27af5b50991184e0f
TFM_TESTS_GIT_SHA=b0c06b46b8a32d73794637e2a034a9cfbb4e883b
PSA_ARCH_TESTS_GIT_SHA=02d145d80da31bc78ce486bda99ad55597fea0ca


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

