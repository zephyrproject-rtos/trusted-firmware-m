# Trusted Firmware M secure boot
For secure devices it is security critical to enforce firmware authenticity to
protect against execution of malicious software. This is implemented by building
a trust chain where each step in the execution chain authenticates the next
step before execution. The chain of trust in based on a "Root of Trust" which
is implemented using asymmetric cryptography. The Root of Trust is a combination
of an immutable bootloader and a public key(ROTPK).

# Second stage bootloader in TF-M
To implement secure boot functionality an external project MCUBoot has been
integrated to TF-M. For further information please refer to the
[MCUBoot homepage](https://www.mcuboot.com/).
Original source-code is available at
[GitHub](https://github.com/runtimeco/mcuboot).
This document contains information about MCUBoot modifications and how MCUBoot
has been integrated to TF-M.

Bootloader is started when CPU is released from reset. It runs in secure mode.
It authenticates the firmware image by hash (SHA-256) and digital signature
(RSA-2048) validation. Public key, that the checks happens against, is built
into the bootloader image. Metadata of the image is delivered together with the
image itself in a header and trailer section. In case of successful
authentication, bootloader passes execution to the secure image. Execution never
returns to bootloader until next reset.

A default RSA key pair is stored in the repository, public key is in `keys.c`
and private key is in `root-rsa-2048.pem`. `DO NOT use them in production code,
they are exclusively for testing!` Private key must be stored in a safe place
outside of the repository. `Imgtool.py` can be used to generate new key pairs.

The bootloader handles the secure and non-secure images as a single blob which
is contiguous in the device memory. At compile time these images are
concatenated and signed with RSA-2048 digital signature. Preparation of payload
is done by Python scripts: `bl2/ext/mcuboot/scripts/`. At the end of a
successful build signed TF-M payload can be found in:
```
<build_dir>/install/outputs/fvp/tfm_sign.bin
```

## Integration with TF-M
MCUBoot assumes a predefined memory layout which is described below
(applicable for AN521). It is mandatory to define slot 0, slot 1 and scratch
partitions, but their size can be changed:
```
- 0x0000_0000 - 0x0007_FFFF:    BL2 bootloader - MCUBoot
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

## Firmware upgrade operation
MCUBoot handles only the firmware authenticity check after start-up and the
firmware switch part of the firmware update process. Downloading the new version
of the firmware is out-of-scope for MCUBoot. MCUBoot supports two different ways
to switch to the new firmware and it is assumed that firmware images are
executed-in-place (XIP). The default behaviour is the image swapping. In this
case active firmware is always executed from slot 0 and slot 1 is a staging area
for new images. Before executing the new firmware image, the content of the two
memory slots must be physically swapped. The other option is the non-swapping
version, which eliminates the complexity of image swapping and its
administration. Active image can be executed from either memory slot, but new
firmware must be linked to the address space of the proper (currently inactive)
memory slot.

### Swapping operation
Active image is stored in slot 0, and this image is started always by the
bootloader. Therefore images must be linked to slot 0. If the bootloader finds a
valid image in slot 1, which is marked for upgrade, then contents of slot 0 and
slot 1 will be swapped, before starting the new image from slot 0. Scratch area
is used as a temporary storage place during image swapping. Update mark from
slot 1 is removed when the swapping is successful. The boot loader can revert
the swapping as a fall-back mechanism to recover the previous working firmware
version after a faulty update. The swap operation is fail-safe and resistant to
power-cut failures. For more details please refer to the MCUBoot [documentation](https://www.mcuboot.com/mcuboot/design.html).

### Non-swapping operation
This operation can be turned on with `MCUBOOT_NO_SWAP` compile time switch. See
[next chapter](#Build time configuration). When enabling non-swapping operation
then the active image flag is moved between slots during firmware upgrade. If
firmware is executed-in-place (XIP), then two firmware images must be generated.
One of them is linked to be executed from slot 0 memory region and the other
from slot 1. The firmware upgrade client, which downloads the new image, must be
aware, which slot hosts the active firmware and which acts as a staging area and
it is responsible for downloading the proper firmware image. At boot time
MCUBoot inspects the version number in the image header and passes execution to
the newer firmware version. New image must be marked for upgrade which is
automatically done by Python scripts at compile time. Revert functionality is
also supported. Image verification is done the same way in both operational
modes. If new image fails during authentication then MCUBoot erases the memory
slot and starts the other image, after successful authentication.

## Build time configuration
MCUBoot related compile time switches can be set in the high level build
configuration files:
```
ConfigDefault.cmake
ConfigCoreTest.cmake
ConfigRegression.cmake
```

Compile time switches:

- BL2 (default: True):
  - **True:** TF-M built together with bootloader. MCUBoot is executed after
  reset and it authenticates TF-M and starts secure code.
  - **False:** TF-M built without bootloader. Secure image linked to the
  beginning of the device memory and executed after reset. If it is false then
  using any of the further compile time switches are invalid.
- MCUBOOT\_NO\_SWAP (default: False):
  - **True:** Activate non-swapping firmware upgrade operation.
  - **False:** Original firmware upgrade operation with image swapping.

Image version number can be specified in:
```
bl2/ext/mcuboot/MCUBoot.cmake
```

At build time automatically two binaries are generated:
```
<build_dir>/install/outputs/fvp/tfm_s_ns_signed_0.bin : Image linked for slot 0
                                                        memory partition
<build_dir>/install/outputs/fvp/tfm_s_ns_signed_1.bin : Image linked for slot 1
                                                        memory partition
```
## Testing firmware upgrade
As downloading the new firmware image is out of scope for MCUBoot, the update
process is started from a state where the original and the new image are already
programmed to the appropriate memory slots. To generate the original and a new
firmware package, TF-M is built twice with different build configurations.

### Swapping firmware upgrade
Run TF-M build twice with two different build configuration: default and
regression. Save the artefacts between builds, because second run can overwrite
original binaries. Download default build to slot 0 and regression build to
slot 1.

#### Executing firmware upgrade on FVP\_MPS2\_AEMv8M
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
--data cpu0=<default_build_dir>/install/outputs/fvp/tfm_s_ns_signed.bin@0x10080000 \
--data cpu0=<regresssion_build_dir>/install/outputs/fvp/tfm_s_ns_signed.bin@0x10180000
```

#### Executing firmware upgrade on SSE 200 FPGA on MPS2 board
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

The following message will be shown in case of successful firmware upgrade,
`Swap type: test` indicates that images were swapped:
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

### Non-swapping firmware upgrade
Follow the same instructions as in case of swapping build including these
changes:

- Set MCUBOOT\_NO\_SWAP compile time switch to true before build.
- Increase the image version number between the two build run.

### Executing firmware upgrade on FVP\_MPS2\_AEMv8M
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
--data cpu0=<default_build_dir>/install/outputs/fvp/tfm_s_ns_signed_0.bin@0x10080000 \
--data cpu0=<regresssion_build_dir>/install/outputs/fvp/tfm_s_ns_signed_1.bin@0x10180000
```

### Executing firmware upgrade on SSE 200 FPGA on MPS2 board
```
TITLE: Versatile Express Images Configuration File
[IMAGES]
TOTALIMAGES: 3                     ;Number of Images (Max: 32)
IMAGE0ADDRESS: 0x00000000
IMAGE0FILE: \Software\mcuboot.axf  ; BL2 bootloader
IMAGE1ADDRESS: 0x10080000
IMAGE1FILE: \Software\tfm_sig0.bin ; TF-M example application binary blob
IMAGE2ADDRESS: 0x10180000
IMAGE2FILE: \Software\tfm_sig1.bin ; TF-M regression test binary blob
```

The following message will be shown in case of successful firmware upgrade,
notice that image with higher version number (`version=1.2.3.5`) is executed:
```
[INF] Starting bootloader
[INF] Image 0: version=1.2.3.4, magic= good, image_ok=0xff
[INF] Image 1: version=1.2.3.5, magic= good, image_ok=0xff
[INF] Booting image from slot 1
[INF] Bootloader chainload address offset: 0x180000
[INF] Jumping to the first image slot
[Sec Thread] Secure image initializing!

#### Execute test suites for the Secure area ####
Running Test Suite SST secure interface tests (TFM_SST_TEST_2XXX)...
...
```

*Note* Firmware upgrade support for Musca-A1 board is not yet supported.

--------------

*Copyright (c) 2018, Arm Limited. All rights reserved.*
