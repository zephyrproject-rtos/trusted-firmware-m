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
automatically done by Python scripts at compile time. Image verification is done
the same way in both operational modes. If new image fails during authentication
then MCUBoot erases the memory slot and starts the other image, after successful
authentication.

At build time automatically two binaries are generated:
```
<build_dir>/install/outputs/fvp/tfm_s_ns_signed_0.bin : Image linked for slot 0
                                                        memory partition
<build_dir>/install/outputs/fvp/tfm_s_ns_signed_1.bin : Image linked for slot 1
                                                        memory partition
```

### RAM Loading firmware upgrade
Musca A1 supports an image upgrade mode that is separate to both the swapping
and non-swapping modes. This is the `RAM loading` mode (please refer to the
table below). Like the non-swapping mode, this selects the newest image by
reading the image version numbers in the image headers, but instead of
executing it in place, the newest image is copied to RAM for execution. The
load address, the location in RAM where the image is copied to, is stored
in the image header.

### Summary of different modes for image upgrade

Different implementations of the image upgrade operation (whether through
swapping, non-swapping, or loading into RAM and executing from there)
are supported by the platforms. The table below shows which of these modes
are supported by which platforms:

|    -     | Without BL2 <sup>1</sup> | With BL2 <sup>2</sup> | With BL2 <sup>2</sup> |   With BL2 <sup>2</sup>  |
|:--------:|:------------------------:|:---------------------:|:---------------------:|:------------------------:|
|    -     |            XIP           |          XIP          |          XIP          |         Not XIP          |
|    -     |             -            |   Swap <sup>3</sup>   |  No-swap <sup>4</sup> | RAM loading <sup>5</sup> |
|  AN521   |            Yes           |          Yes          |          Yes          |            No            |
|  AN519   |            Yes           |          Yes          |          Yes          |            No            |
| Musca-A1 |            No            |          No           |          No           |            Yes           |
| Musca-B1 |            Yes           |          Yes          |          Yes          |            No            |

(1) To disable BL2, please turn off the `BL2` compiler switch in the
top-level configuration files or in the command line

(2) BL2 is enabled by default

(3) The image executes in-place (XIP) and is in swapping mode for image update
by default

(4) To enable XIP No-swap, set the configuration variable `MCUBOOT_NO_SWAP` to
`True` in the top-level configuration files, or include the `MCUBOOT_NO_SWAP`
macro in the command line

(5) To enable RAM loading, set the configuration variable `MCUBOOT_RAM_LOADING`
to `True` in the top-level configuration files, or include the
`MCUBOOT_RAM_LOADING` macro in the command line

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
- MCUBOOT\_RAM\_LOADING (default: False):
  - **True:** Activate RAM loading firmware upgrade operation, where latest
  image is copied to RAM and runs from there instead of being executed
  in-place.
  - **False:** Original firmware upgrade operation with image swapping.

### Image versioning
An image version number is written to its header by one of the python scripts,
and this number is used by the bootloader when the non-swapping mode is
enabled.

The version number of the image can manually be passed in through the command
line in the cmake configuration step:
```
cmake -G"Unix Makefiles" -DTARGET_PLATFORM=AN521 -DCOMPILER=ARMCLANG -DIMAGE_VERSION=1.2.3+4 ../
```
Alternatively, the version number can be less specific (e.g 1, 1.2, or 1.2.3),
where the missing numbers are automatically set to zero. The image version
number argument is optional, and if it is left out, then the version numbers of
the image(s) being built in the same directory will automatically change. In
this case, the last component (the build number) automatically increments from
the previous one: 0.0.0+1 -> 0.0.0+2, for as many times as the build is re-ran,
**until a number is explicitly provided**. If automatic versioning is in place
and then an image version number is provided for the first time, the new number
will take precedence and be used instead. All subsequent image versions are
then set to the last number that has been specified, and the build number would
stop incrementing. Any new version numbers that are provided will overwrite
the previous one: 0.0.0+1 -> 0.0.0+2. Note: To re-apply automatic image
versioning, please start a clean build without specifying the image version
number at all.

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
--data cpu0=<default_build_dir>/install/outputs/fvp/tfm_s_ns_signed_0.bin@0x10080000 \
--data cpu0=<regresssion_build_dir>/install/outputs/fvp/tfm_s_ns_signed_1.bin@0x10180000
```

#### Executing firmware upgrade on SSE 200 FPGA on MPS2 board
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

#### Executing firmware upgrade on Musca-B1 board
After two images have been built, they can be concatenated to create the
combined image using `srec_cat`:

- Linux: `srec_cat bl2/ext/mcuboot/mcuboot.bin -Binary -offset 0x0 tfm_sign_0.bin -Binary -offset 0x20000 tfm_sign_1.bin -Binary -offset 0x120000 -o tfm.hex -Intel`

- Windows: `srec_cat.exe bl2\ext\mcuboot\mcuboot.bin -Binary -offset 0x0 tfm_sign_0.bin -Binary -offset 0x20000 tfm_sign_1.bin -Binary -offset 0x120000 -o tfm.hex -Intel`

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

### RAM loading firmware upgrade
To enable RAM loading, please set `MCUBOOT_RAM_LOADING` to True (either in the
configuration file or through the command line), and then specify a destination
load address in RAM where the image can be copied to and executed from. The
`IMAGE_LOAD_ADDRESS` macro must be specified in the target dependent files,
for example with Musca A1, its `flash_layout.h` file in the `platform`
folder should include `#define IMAGE_LOAD_ADDRESS #0x10020000`

#### Executing firmware upgrade on Musca-A1 board
After two images have been built, they can be concatenated to create the
combined image using `srec_cat`:

```
Windows:
srec_cat.exe bl2\ext\mcuboot\mcuboot.bin -Binary -offset 0x200000
tfm_sign_old.bin -Binary -offset 0x220000 tfm_sign_new.bin -Binary -offset
0x320000 -o tfm.hex -Intel

Linux:
srec_cat bl2/ext/mcuboot/mcuboot.bin -Binary -offset 0x200000 tfm_sign_old.bin
-Binary -offset 0x220000 tfm_sign_new.bin -Binary -offset
0x320000 -o tfm.hex -Intel
```

The following message will be shown in case of successful firmware upgrade when,
RAM loading is enabled, notice that image with higher version number
(`version=0.0.0.2`) is executed:

```
[INF] Image 0: version=0.0.0.1, magic= good, image_ok=0xff
[INF] Image 1: version=0.0.0.2, magic= good, image_ok=0xff
[INF] Image has been copied from slot 1 in flash to SRAM address 0x10020000
[INF] Booting image from SRAM at address 0x10020000
[INF] Bootloader chainload address offset: 0x20000
[INF] Jumping to the first image slot
[Sec Thread] Secure image initializing!
```

--------------

*Copyright (c) 2018, Arm Limited. All rights reserved.*
