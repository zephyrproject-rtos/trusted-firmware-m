# Trusted Firmware M user guide
How to compile and run TF-M and example test application for CoreLink SSE-200
subsystem on the MPS2 board and on the Fast Model(FVP).

Follow [build instruction](./tfm_build_instruction.md) to build the binaries.
Follow [secure boot](./tfm_secure_boot.md) to build the binaries with or without
BL2 bootloader.

## Execute TF-M example and regression tests on MPS2 boards and FVP ##
The BL2 bootloader and TF-M example application and tests run correctly on
SMM-SSE-200 for MPS2 (AN521) and on the Fixed Virtual Platform model
FVP_MPS2_AEMv8M version 11.2.23.

### To run the example code on FVP_MPS2_AEMv8M
Using FVP_MPS2_AEMv8M provided by DS-5 v5.27.1.

*FVP reference guide can be found
[here](https://developer.arm.com/docs/100966/latest)*

#### Example application and regression tests without BL2 bootloader
Add `tfm_s.axf` and `tfm_ns.axf` to symbol files in Debug Configuration menu.
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
--application cpu0=<build_dir>/install/outputs/fvp/tfm_s.axf \
--data cpu0=<build_dir>/install/outputs/fvp/tfm_ns.bin@0x00100000

```

#### Example application and regression tests with BL2 bootloader
To test TF-M with bootloader, one must apply the following changes:

* Add `mcuboot.axf` to symbol files in DS-5 in Debug Configuration menu.
* Replace the last two lines of the previous command with this:
```
--application cpu0=<build_dir>/install/outputs/fvp/mcuboot.axf \
--data cpu0=<build_dir>/install/outputs/fvp/tfm_s_ns_signed.bin@0x10080000
```

#### Test software upgrade with BL2 bootloader
BL2 bootloader is mandatory to test software update. Furthermore two TF-M blob
must be built. Outputs of example application and regression test can be used to
test it. Load output of example application to slot 0 (0x10080000) and output of
regression test to slot 1 (0x10180000). Add the following line to the
end of the previous chapter:
```
--data cpu0=<build_dir>/install/outputs/fvp/tfm_s_ns_signed.bin@0x10180000
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

1. Copy `mcuboot.bin` and `tfm_sign.bin` files from
   `<build_dir>/install/outputs/AN521/` to `<MPS2 device name>/SOFTWARE/`
2. Open `<MPS2 device name>/MB/HBI0263C/AN521/images.txt`
3. Update the `AN521/images.txt` file as follows:
```
TITLE: Versatile Express Images Configuration File
[IMAGES]
TOTALIMAGES: 2                     ;Number of Images (Max: 32)
IMAGE0ADDRESS: 0x10000000
IMAGE0FILE: \Software\mcuboot.bin  ; BL2 bootloader
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
  Description: 'Get handle interface (DEPRECATED)'
This test is DEPRECATED and the test execution was SKIPPED
  TEST PASSED!
> Executing 'TFM_SST_TEST_2003'
  Description: 'Get handle with null handle pointer (DEPRECATED)'
This test is DEPRECATED and the test execution was SKIPPED
  TEST PASSED!
> Executing 'TFM_SST_TEST_2004'
  Description: 'Write interface'
  TEST PASSED!
> Executing 'TFM_SST_TEST_2005'
  Description: 'Read interface'
....

```

Note: SST reliability tests take a few minutes to run on the MPS2.

#### Example application without BL2 bootloader
1. Copy `tfm_s.bin` and `tfm_ns.bin` files from
   `<build_dir>/install/outputs/AN521/` to `<MPS2 device name>/SOFTWARE/`
2. Open `<MPS2 device name>/MB/HBI0263C/AN521/images.txt`
3. Update the `AN521/images.txt` file as follows:
```
TITLE: Versatile Express Images Configuration File
[IMAGES]
TOTALIMAGES: 2                   ;Number of Images (Max: 32)
IMAGE0ADDRESS: 0x10000000
IMAGE0FILE: \Software\tfm_s.bin  ; Secure code
IMAGE1ADDRESS: 0x00100000
IMAGE1FILE: \Software\tfm_ns.bin ; Non-secure code
```
4. Close `<MPS2 device name>/MB/HBI0263C/AN521/images.txt`
5. Unmount/eject the `<MPS2 device name>` unit
6. Reset the board to execute the TF-M example application
7. After completing the procedure you should be able to visualize on the serial
   port (baud 115200 8n1) the following messages:

```
[Sec Thread] Secure image initializing!
```

#### Regression tests without BL2 bootloader
After completing the procedure you should be able to visualize on the serial
port (baud 115200 8n1) the following messages:

```
[Sec Thread] Secure image initializing!

#### Execute test suites for the secure storage service ####
Running Test Suite SST secure interface tests (TFM_SST_TEST_2XXX)...

> Executing 'TFM_SST_TEST_2001'
  Description: 'Create interface'
  TEST PASSED!
> Executing 'TFM_SST_TEST_2002'
  Description: 'Get handle interface (DEPRECATED)'
This test is DEPRECATED and the test execution was SKIPPED
  TEST PASSED!
> Executing 'TFM_SST_TEST_2003'
  Description: 'Get handle with null handle pointer (DEPRECATED)'
This test is DEPRECATED and the test execution was SKIPPED
  TEST PASSED!
> Executing 'TFM_SST_TEST_2004'
  Description: 'Write interface'
  TEST PASSED!
> Executing 'TFM_SST_TEST_2005'
  Description: 'Read interface'
....

```

## Execute TF-M example and regression tests on Musca test chip boards ##

#### Example application with BL2 bootloader

1. Create a unified hex file comprising of both mcuboot and tfm_sign binary
  * For Musca-A1
    * Windows
    `srec_cat.exe bl2\ext\mcuboot\mcuboot.bin -Binary -offset 0x200000 tfm_sign.bin -Binary -offset 0x220000 -o tfm.hex -Intel`
    * Linux
    `srec_cat bl2/ext/mcuboot/mcuboot.bin -Binary -offset 0x200000 tfm_sign.bin -Binary -offset 0x220000 -o tfm.hex -Intel`
  * For Musca-B1
    * Windows
    `srec_cat.exe bl2\ext\mcuboot\mcuboot.bin -Binary -offset 0x0 tfm_sign.bin -Binary -offset 0x20000 -o tfm.hex -Intel`
    * Linux
    `srec_cat bl2/ext/mcuboot/mcuboot.bin -Binary -offset 0x0 tfm_sign.bin -Binary -offset 0x20000 -o tfm.hex -Intel`
2. Plug the Musca board into your computer. The board should appear as a USB
   drive
3. Copy `tfm.hex` to the USB drive
4. Reset the board to execute the TF-M example application
5. After completing the procedure you should be able to see on the UART0
 (baud 115200 8n1) the following messages:

```
[INF] Starting bootloader
[INF] Image 0: magic=good, copy_done=0xff, image_ok=0xff
[INF] Scratch: magic=bad, copy_done=0x5, image_ok=0xd9
[INF] Boot source: slot 0
[INF] Swap type: none
[INF] Bootloader chainload address offset: 0x20000
[INF] Jumping to the first image slot
[Sec Thread] Secure image initializing!
```

#### Regression tests with BL2 bootloader

After completing the procedure you should see the following messages on the
UART0 (baud 115200 8n1):

```
[INF] Starting bootloader
[INF] Image 0: magic=good, copy_done=0xff, image_ok=0xff
[INF] Scratch: magic=bad, copy_done=0x5, image_ok=0x9
[INF] Boot source: slot 0
[INF] Swap type: none
[INF] Bootloader chainload address offset: 0x20000
[INF] Jumping to the first image slot
[Sec Thread] Secure image initializing!

#### Execute test suites for the secure storage service ####
Running Test Suite SST secure interface tests (TFM_SST_TEST_2XXX)...
> Executing 'TFM_SST_TEST_2001'
  Description: 'Create interface'
  TEST PASSED!
> Executing 'TFM_SST_TEST_2002'
  Description: 'Get handle interface (DEPRECATED)'
This test is DEPRECATED and the test execution was SKIPPED
  TEST PASSED!
> Executing 'TFM_SST_TEST_2003'
  Description: 'Get handle with null handle pointer (DEPRECATED)'
This test is DEPRECATED and the test execution was SKIPPED
  TEST PASSED!
> Executing 'TFM_SST_TEST_2004'
  Description: 'Get attributes interface'
  TEST PASSED!
> Executing 'TFM_SST_TEST_2005'
  Description: 'Get attributes with null attributes struct pointer'
....

```

#### Example application or regression tests on Musca-B1 without BL2 bootloader
Follow the above procedures, but create a unified hex file out of `tfm_s.bin`
and `tfm_ns.bin`:

* Windows
`srec_cat.exe app\secure_fw\tfm_s.bin -Binary -offset 0x0 app\tfm_ns.bin -Binary -offset 0xA0000 -o tfm.hex -Intel`
* Linux
`srec_cat app/secure_fw/tfm_s.bin -Binary -offset 0x0 app/tfm_ns.bin -Binary -offset 0xA0000 -o tfm.hex -Intel`

## Firmware upgrade and image validation with BL2 bootloader
High level operation of BL2 bootloader and instructions for testing firmware
upgrade is described in [secure boot](tfm_secure_boot.md) document.

--------------

*Copyright (c) 2017 - 2018, Arm Limited. All rights reserved.*
