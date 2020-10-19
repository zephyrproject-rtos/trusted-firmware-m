#!/bin/sh

TFM_GIT_SHA=8bebd05745a8b27dccc6403f0215fa6e39de3bc1
TFM_TESTS_GIT_SHA=652bb8a1502c95bfd1ee8e44ccd1126ce74ae41f
MCUBOOT_GIT_SHA=v1.6.0
PSA_ARCH_TESTS_GIT_SHA=v20.03_API1.0

if [ ! -d trusted-firmware-m/.git ]; then
   git clone https://git.trustedfirmware.org/trusted-firmware-m.git -b ${TFM_GIT_SHA} --bare trusted-firmware-m/.git
   cd trusted-firmware-m
   git init
   git checkout -f
   cd ..
fi

if [ ! -d mcuboot/.git ]; then
   git clone https://github.com/JuulLabs-OSS/mcuboot.git -b v1.4.0 -b ${MCUBOOT_GIT_SHA} --bare mcuboot/.git
   cd mcuboot
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

