# Trusted Firmware M user guide
How to compile and run TF-M and example test application for CoreLink SSE-200
subsystem on the MPS2 board and on the Fast Model(FVP).

Follow [build instruction](./tfm_build_instruction.md) to build the binaries.

## Execute TF-M example and regression tests on MPS2 boards and FVP ##
The BL2 bootloader and TF-M example application and tests run correctly on
SMM-SSE-200 for MPS2 (AN521) and on the Fixed Virtual Platform model
FVP_MPS2_AEMv8M version 11.2.23.

### To run the example code on FVP_MPS2_AEMv8M
Using FVP_MPS2_AEMv8M provided by DS-5 v5.27.1.

*FVP reference guide can be found
[here](https://developer.arm.com/docs/100966/latest)*

#### Example application
Add `tfm_s.axf` and `tfm_ns.axf` to symbol files in Debug Configuration menu.
```
<DS5_PATH>/sw/models/bin/FVP_MPS2_AEMv8M  \
--parameter fvp_mps2.platform_type=2 \
--parameter cpu0.baseline=0 \
--parameter cpu0.INITVTOR_S=0x10080400 \
--parameter cpu0.semihosting-enable=0 \
--parameter fvp_mps2.DISABLE_GATING=0 \
--parameter fvp_mps2.telnetterminal0.start_telnet=1 \
--parameter fvp_mps2.telnetterminal1.start_telnet=0 \
--parameter fvp_mps2.telnetterminal2.start_telnet=0 \
--parameter fvp_mps2.telnetterminal0.quiet=0 \
--parameter fvp_mps2.telnetterminal1.quiet=1 \
--parameter fvp_mps2.telnetterminal2.quiet=1 \
--application cpu0=<build_dir>/app/tfm_ns.axf \
--application cpu0=<build_dir>/app/secure_fw/tfm_s.axf

```
#### Regression tests
Add `tfm_s.axf` and `tfm_ns.axf` to symbol files in Debug Configuration menu.
```
<DS5_PATH>/sw/models/bin/FVP_MPS2_AEMv8M \
--parameter fvp_mps2.platform_type=2 \
--parameter cpu0.baseline=0 \
--parameter cpu0.INITVTOR_S=0x10080400 \
--parameter cpu0.semihosting-enable=0 \
--parameter fvp_mps2.DISABLE_GATING=0 \
--parameter fvp_mps2.telnetterminal0.start_telnet=1 \
--parameter fvp_mps2.telnetterminal1.start_telnet=0 \
--parameter fvp_mps2.telnetterminal2.start_telnet=0 \
--parameter fvp_mps2.telnetterminal0.quiet=0 \
--parameter fvp_mps2.telnetterminal1.quiet=1 \
--parameter fvp_mps2.telnetterminal2.quiet=1 \
--application cpu0=<build_dir>/app/tfm_ns.axf \
--application cpu0=<build_dir>/app/secure_fw/tfm_s.axf
```
#### Running example application and regression test with BL2 bootloader
To test TF-M with bootloader, one must apply the following changes:

* Add `mcuboot.axf` to symbol files in DS-5 in Debug Configuration menu.
* Change the value of `cpu0.INITVTOR_S` parameter and the last two lines of the
previous command for this:
```
...
--parameter cpu0.INITVTOR_S=0x10000000 \
...
--application cpu0=<build_dir>/bl2/ext/mcuboot/mcuboot.axf \
--data cpu0=<build_dir>/tfm_sign.bin@0x10080000
```

### To run the example code on SSE 200 FPGA on MPS2 board
FPGA image is available to download [here](https://developer.arm.com/products/system-design/development-boards/cortex-m-prototyping-systems/mps2)

To run BL2 bootloader and TF-M example application and tests in the MPS2 board,
it is required to have SMM-SSE-200 for MPS2 (AN521) image in the MPS2 board
SD card. The image should be located in
`<MPS2 device name>/MB/HBI0263<board revision letter>/AN521`

The MPS2 board tested is HBI0263C referred also as MPS2+.

`Note: If you change the exe names, MPS2 expects file names in 8.3 format.`

#### Example application

1. Copy `mcuboot.axf` and `tfm_sign.bin` files in `<MPS2 device name>/SOFTWARE/`
2. Open `<MPS2 device name>/MB/HBI0263C/AN521/images.txt`
3. Update the `AN521/images.txt` file as follows:
```
TITLE: Versatile Express Images Configuration File
[IMAGES]
TOTALIMAGES: 2                     ;Number of Images (Max: 32)
IMAGE0ADDRESS: 0x00000000
IMAGE0FILE: \Software\mcuboot.axf  ; BL2 bootloader
IMAGE1ADDRESS: 0x10080000
IMAGE1FILE: \Software\tfm_sign.bin ; TF-M example application binary blob
```
4. Close `<MPS2 device name>/MB/HBI0263C/AN521/images.txt`
5. Unmount/eject the `<MPS2 device name>` unit
6. Reset the board to execute the TF-M example application
7. After completing the procedure you should be able to visualize on the serial
   port (baud 115200 8n1) the following messages:


```
[INF] Starting bootloader
[INF] Image 0: magic=good, copy_done=0xff, image_ok=0xff
[INF] Scratch: magic=bad, copy_done=0x5, image_ok=0xcf
[INF] Boot source: slot 0
[INF] Swap type: none
[INF] Bootloader chainload address offset: 0x80000
[INF] Jumping to the first image slot
[Sec Thread] Secure image initializing!
```

#### Regression tests

After completing the procedure you should be able to visualize on the serial
port (baud 115200 8n1) the following messages:

```
[INF] Starting bootloader
[INF] Image 0: magic=good, copy_done=0xff, image_ok=0xff
[INF] Scratch: magic=bad, copy_done=0x5, image_ok=0xcf
[INF] Boot source: slot 0
[INF] Swap type: none
[INF] Bootloader chainload address offset: 0x80000
[INF] Jumping to the first image slot
[Sec Thread] Secure image initializing!

#### Execute test suites for the secure storage service ####
Running Test Suite SST secure interface tests (TFM_SST_TEST_2XXX)...

> Executing 'TFM_SST_TEST_2001'
  Description: 'Create interface'
  TEST PASSED!
> Executing 'TFM_SST_TEST_2002'
  Description: 'Get handle interface'
  TEST PASSED!
> Executing 'TFM_SST_TEST_2003'
  Description: 'Get attributes interface'
  TEST PASSED!
> Executing 'TFM_SST_TEST_2004'
  Description: 'Write interface'
  TEST PASSED!
> Executing 'TFM_SST_TEST_2005'
  Description: 'Read interface'
....

```

Note: SST reliability tests take approximately three minutes to run on the MPS2.

## Execute TF-M example and regression tests on Musca-A1 test chip board ##

#### Example application

1. Create a unified hex file comprising of both mcuboot and tfm_sign binary
   * Windows
 `srec_cat.exe bl2\ext\mcuboot\mcuboot.bin -Binary -offset 0x200000 tfm_sign.bin -Binary -offset 0x210000 -o tfm.hex -Intel`
   * Linux
 `srec_cat bl2/ext/mcuboot/mcuboot.bin -Binary -offset 0x200000 tfm_sign.bin -Binary -offset 0x210000 -o tfm.hex -Intel`
2. Plug in the Musca-A1 board in your computer. The board should appear as USB drive
3. Drag and drop `tfm.hex` to the USB drive
4. Reset the board to execute the TF-M example application
5. After completing the procedure you should be able to see on the UART0
 (baud 115200 8n1) the following messages:

```
[INF] Starting bootloader
[INF] Image 0: magic=good, copy_done=0xff, image_ok=0xff
[INF] Scratch: magic=bad, copy_done=0x5, image_ok=0xd9
[INF] Boot source: slot 0
[INF] Swap type: none
[INF] Bootloader chainload address offset: 0x10000
[INF] Jumping to the first image slot
[Sec Thread] Secure image initializing!
```

#### Regression tests

After completing the procedure you should be able to see on the UART0
(baud 115200 8n1) the following messages:

```
[INF] Starting bootloader
[INF] Image 0: magic=good, copy_done=0xff, image_ok=0xff
[INF] Scratch: magic=bad, copy_done=0x5, image_ok=0x9
[INF] Boot source: slot 0
[INF] Swap type: none
[INF] Bootloader chainload address offset: 0x10000
[INF] Jumping to the first image slot
[Sec Thread] Secure image initializing!

#### Execute test suites for the secure storage service ####
Running Test Suite SST secure interface tests (TFM_SST_TEST_2XXX)...
> Executing 'TFM_SST_TEST_2001'
  Description: 'Create interface'
  TEST PASSED!
> Executing 'TFM_SST_TEST_2002'
  Description: 'Get handle interface'
  TEST PASSED!
> Executing 'TFM_SST_TEST_2003'
  Description: 'Get handle with null handle pointer'
  TEST PASSED!
> Executing 'TFM_SST_TEST_2004'
  Description: 'Get attributes interface'
  TEST PASSED!
> Executing 'TFM_SST_TEST_2005'
  Description: 'Get attributes with null attributes struct pointer'
....

```

## Software upgrade and image validation with BL2 bootloader

BL2 bootloader is an integrated and ported version of an external project:
MCUBoot. You can find further information about design and operation on
its [website](https://www.mcuboot.com/) or
[GitHub](https://github.com/runtimeco/mcuboot) page. Its goal is to ensure that
only authenticated software is allowed to run on the device. Bootloader is
started when CPU is released from reset. It runs in secure state. Public key is
built into the bootloader image. This is used when validating the digital
signature of the TF-M payload. In case of successful authentication, bootloader
passes execution to the secure image. Execution never returns to bootloader
until next reset.

A default RSA key pair is stored in the repository, public key is in keys.c and
private key is in root-rsa-2048.pem. DO NOT use them in production code, they
are exclusively for testing! Private key must be stored in a safe place outside
of the repository. Imgtool.py can be used to generate new key pairs.

The bootloader handles the secure and non-secure images as single binary blob
which is contiguous in the device memory. At compile time these images are
concatenated and signed with RSA-2048 digital signature. At the end of a
successful build signed TF-M payload can be found here:
```
<build_dir>/tfm_sign.bin
```

The device memory is partitioned in the following way:
```
- 0x0000_0000 - 0x0007_FFFF:    BL2 bootloader
- 0x0008_0000 - 0x000F_FFFF:    Slot 0 : Single binary blob: Secure + Non-Secure
                                image; Primary memory partition
  - 0x0008_0000 - 0x0008_03FF:  Common image header
  - 0x0008_0400 - 0x0008_xxxx:  Secure image
  - 0x0008_xxxx - 0x0010_03FF:  Padding (with 0xFF)
  - 0x0010_0400 - 0x0010_xxxx:  Non-secure image
  - 0x0010_xxxx - 0x0010_xxxx:  Hash value(SHA256) and RSA signature
                                of combined image

- 0x0018_0000 - 0x0027_FFFF:    Slot 1 : Secure + Non-Secure image; Secondary
                                memory partition, structured identically to slot
                                0
- 0x0028_0000 - 0x0037_FFFF:    Scratch area, used during image swapping
```
Original image is stored in slot 0, always image in this memory partition is
started by the bootloader. Therefore images must always be linked to slot 0
memory address. If bootloader finds a valid image in slot 1, which is marked for
upgrade, contains a 4 bytes magic number at the end of the partition, then
content of slot 0 and slot 1 will be swapped, before starting the new image from
slot 0. Magic number in slot 1 is cleared during swap operation. Bootloader has
a revert capability, in case of faulty firmware was upgraded and resistant
against power-cut failures. These functionalities are described detailed in the
original project documentation.

### Test software upgrade and image validation

To test software upgrade functionality, two TF-M blobs are needed. Download old
image to slot 0 and new image to slot 1. In the following example TF-M example
application is used as old software and TF-M regression test is used as new
software. Follow the previous instructions with small changes.

#### Test software upgrade on FVP_MPS2_AEMv8M
```
<DS5_PATH>/sw/models/bin/FVP_MPS2_AEMv8M  \
--parameter fvp_mps2.platform_type=2 \
--parameter cpu0.baseline=0 \
--parameter cpu0.INITVTOR_S=0x10000000 \
--parameter cpu0.semihosting-enable=0 \
--parameter fvp_mps2.DISABLE_GATING=0 \
--parameter fvp_mps2.telnetterminal0.start_telnet=1 \
--parameter fvp_mps2.telnetterminal1.start_telnet=0 \
--parameter fvp_mps2.telnetterminal2.start_telnet=0 \
--parameter fvp_mps2.telnetterminal0.quiet=0 \
--parameter fvp_mps2.telnetterminal1.quiet=1 \
--parameter fvp_mps2.telnetterminal2.quiet=1 \
--application cpu0=<build_dir>/bl2/ext/mcuboot/mcuboot.axf \
--data cpu0=<example_app_build_dir>/tfm_sign.bin@0x10080000 \
--data cpu0=<regresssion_test_build_dir>/tfm_sign.bin@0x10180000
```

#### Test software upgrade on SSE 200 FPGA on MPS2 board
```
TITLE: Versatile Express Images Configuration File
[IMAGES]
TOTALIMAGES: 3                     ;Number of Images (Max: 32)
IMAGE0ADDRESS: 0x00000000
IMAGE0FILE: \Software\mcuboot.axf  ; BL2 bootloader
IMAGE1ADDRESS: 0x10080000
IMAGE1FILE: \Software\tfm_sig1.bin ; TF-M example application binary blob
IMAGE2ADDRESS: 0x10180000
IMAGE2FILE: \Software\tfm_sig2.bin ; TF-M regression test binary blob
```

The same messages will be showed as in case of regression test with one
difference, `Swap type: none` will be replaced with `Swap type: test`:
```
[INF] Image 0: magic=good, copy_done=0xff, image_ok=0xff
[INF] Scratch: magic=bad, copy_done=0x5, image_ok=0xcf
[INF] Boot source: slot 0
[INF] Swap type: test
[INF] Bootloader chainload address offset: 0x80000
[INF] Jumping to the first image slot
[Sec Thread] Secure image initializing!

#### Execute test suites for the secure storage service ####
Running Test Suite SST secure interface tests (TFM_SST_TEST_2XXX)...
...
```
This indicates that software upgrade happened.

*Note* Software upgrade support for Musca-A1 board is not yet supported.

--------------

*Copyright (c) 2017 - 2018, Arm Limited. All rights reserved.*
