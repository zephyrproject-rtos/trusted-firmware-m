# Copyright (c) 2020, Linaro. All rights reserved.
# Copyright (c) 2020, Arm Limited. All rights reserved.
# Copyright (c) 2021, NXP Semiconductors. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause

import os
import platform

os.chdir('../../../../../../build')
os.system('arm-none-eabi-objcopy -S -O ihex bin/tfm_s.axf tfm_s.hex')
os.system('arm-none-eabi-objcopy -S -O ihex bin/tfm_ns.axf tfm_ns.hex')

# Flash with JLinkExe

FILE = "flash.jlink"
if os.path.isfile(FILE):
    if platform.system() == 'Windows':
        os.system('del /f FILE')
    else:
        os.system('rm -rf FILE')
    
os.system('echo r >> ' + FILE)
os.system('echo erase  >> ' + FILE)
os.system('echo loadfile tfm_s.hex >> ' + FILE)
os.system('echo loadfile tfm_ns.hex  >> ' + FILE)
os.system('echo r >> ' + FILE)
os.system('echo go >> ' + FILE)
os.system('echo exit >> ' + FILE)

if platform.system() == 'Windows':
    os.system('JLink -device lpc55s69 -if swd -speed 2000 -autoconnect 1 -commanderscript ' + FILE)

else:
    os.system('JLinkExe -device lpc55s69 -if swd -speed 2000 -autoconnect 1 -commanderscript ' +FILE)
