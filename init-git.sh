#!/bin/sh

TFM_GIT_SHA=TF-Mv1.0
CMSIS_5_GIT_SHA=5.5.0
MBED_CRYPTO_GIT_SHA=mbedcrypto-3.0.1
PSA_ARCH_TESTS_GIT_SHA=v20.03_API1.0

if [ ! -d trusted-firmware-m/.git ]; then
   git clone https://git.trustedfirmware.org/trusted-firmware-m.git -b ${TFM_GIT_SHA} --bare trusted-firmware-m/.git
   cd trusted-firmware-m
   git init
   git checkout -f
   cd ..
fi

if [ ! -d CMSIS_5/.git ]; then
   git clone https://github.com/ARM-software/CMSIS_5.git -b ${CMSIS_5_GIT_SHA} --bare CMSIS_5/.git
   cd CMSIS_5
   git init
   git checkout -f ${CMSIS_5_GIT_SHA} CMSIS/RTOS2
   git lfs fetch
   git lfs checkout CMSIS/RTOS2
   cd ..
fi

if [ ! -d mbed-crypto/.git ]; then
   git clone https://github.com/ARMmbed/mbed-crypto.git -b ${MBED_CRYPTO_GIT_SHA} --bare mbed-crypto/.git
   cd mbed-crypto
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

