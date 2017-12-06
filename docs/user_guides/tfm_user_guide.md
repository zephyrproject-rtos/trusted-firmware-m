# Trusted Firmware M user guide
How to compile and run TF-M example application and tests for CoreLink SSE-200
subsystem on the MPS2 board and the Fast Model(FVP).

Follow [build instruction](./tfm_build_instruction.md) to build the binaries.

## Execute TF-M example and regression tests on MPS2 boards and FVP ##
The TF-M example application and tests run correctly on SMM-SSE-200 for
MPS2 (AN521) and on the Fixed Virtual Platform model FVP_MPS2_AEMv8M
version 11.2.23.

### To run the example code on FVP_MPS2_AEMv8M
Using FVP_MPS2_AEMv8M provided by DS-5 v5.27.1.

*FVP reference guide can be found
[here](https://developer.arm.com/docs/100966/latest)*

#### Example application

```
<DS5_PATH>/sw/models/bin/FVP_MPS2_AEMv8M  \
--parameter fvp_mps2.platform_type=2 \
--parameter cpu0.baseline=0 \
--start cpu0=0x10000004 \
--parameter cpu0.INITVTOR_S=0x10000000 \
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
```
<DS5_PATH>/sw/models/bin/FVP_MPS2_AEMv8M \
--parameter fvp_mps2.platform_type=2 \
--parameter cpu0.baseline=0 \
--start cpu0=0x10000004 \
--parameter cpu0.INITVTOR_S=0x10000000 \
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

### To run the example code on SSE 200 FPGA on MPS2 board
FPGA image is available to download [here](https://developer.arm.com/products/
system-design/development-boards/cortex-m-prototyping-systems/mps2)

To run TF-M example application and tests in the MPS2 board, it is required to
have SMM-SSE-200 for MPS2 (AN521) image in the MPS2 board SD card.
The image should be located in
`<MPS2 device name>/MB/HBI0263<board revision letter>/AN521`

The MPS2 board tested is HBI0263C referred also as MPS2+.

`Note: If you change the exe names, MPS2 expects file names in 8.3 format.`

#### Example application

1. Copy tfm_s.axf and tfm_ns.axf files in <MPS2 device name>/SOFTWARE/
2. Open <MPS2 device name>/MB/HBI0263<board revision letter>/AN521/images.txt
3. Update the AN521/images.txt file as follows:
```
TITLE: Versatile Express Images Configuration File
[IMAGES]
TOTALIMAGES: 2                   ;Number of Images (Max: 32)
IMAGE0ADDRESS: 0x00000000
IMAGE0FILE: \Software\tfm_s.axf  ; TF-M example application secure binary
IMAGE1ADDRESS: 0x00000000
IMAGE1FILE: \Software\tfm_ns.axf ; TF-M example application non-secure binary
```
4. Close <MPS2 device name>/MB/HBI0263C/AN521/images.txt
5. Unmount/eject the <MPS2 device name> unit
6. Reset the board to execute the TF-M example application
7. After completing the procedure you should be able to visualize on the serial
   port (baud 115200 8n1) the following messages:

```
At the moment the examples application only shows following -

[Sec Thread] Secure image initializing!
```

#### Regression tests

After completing the procedure you should be able to visualize on the serial
port (baud 115200 8n1) the following messages:

```
[Sec Thread] Secure image initializing!

#### Execute test suites for the Secure area ####
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

Note: SST reliability tests may take more than 40 minutes to run on the
      MPS2.

--------------

*Copyright (c) 2017, Arm Limited. All rights reserved.*
