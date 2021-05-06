# Copyright (c) 2020, Linaro. All rights reserved.
# Copyright (c) 2020, Arm Limited. All rights reserved.
# Copyright (c) 2021, NXP Semiconductors. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause

import os
import platform

# Move to the TFM build folder
os.chdir('../../../../../../build/bin')

# Define JLink configuration script file
FILE = "flash.jlink"

# Remove previous flash.jlink script file
if os.path.isfile(FILE):
    if platform.system() == 'Windows':
        os.system('del /f /q FILE')
    else:
        os.system('rm -rf FILE')

# Write the JLink configuration into flash.jlink script    
os.system('echo r >> ' + FILE)                                   # reset the target
os.system('echo erase  >> ' + FILE)                              # erase the flash memory
os.system('echo loadfile bl2.hex >> ' + FILE)                    # flash the bootloader file into target
os.system('echo loadfile tfm_s_signed.bin 0x8000  >> ' + FILE)   # flash the signed secure image into target
os.system('echo loadfile tfm_ns_signed.bin 0x30000  >> ' + FILE) # flash the signed non-secure image into target
os.system('echo r >> ' + FILE)                                   # reset the target
os.system('echo go >> ' + FILE)                                  # run the program
os.system('echo exit >> ' + FILE)                                # exit the JLinkCommander 

# Upload the configuration from flash.jlink script into the target device
if platform.system() == 'Windows':
    os.system('JLink -device lpc55s69 -if swd -speed 2000 -autoconnect 1 -commanderscript ' + FILE)

else:
    os.system('JLinkExe -device lpc55s69 -if swd -speed 2000 -autoconnect 1 -commanderscript ' +FILE)
