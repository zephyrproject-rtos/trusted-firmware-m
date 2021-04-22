# Copyright (c) 2020, Linaro. All rights reserved.
# Copyright (c) 2020, Arm Limited. All rights reserved.
# Copyright (c) 2021, NXP Semiconductors. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause

import os
import platform

os.chdir('../../../../../../build')

# Flash with JLinkExe

FILE = "flash.jlink"
if os.path.isfile(FILE):
    if platform.system() == 'Windows':
        os.system('del /f FILE')
    else:
        os.system('rm -rf FILE')
    
os.system('echo r >> ' + FILE)
os.system('echo erase  >> ' + FILE)
os.system('echo loadfile bin/bl2.hex >> ' + FILE)
os.system('echo loadfile bin/tfm_s_signed.bin 0x8000  >> ' + FILE)
os.system('echo loadfile bin/tfm_ns_signed.bin 0x30000  >> ' + FILE)
os.system('echo r >> ' + FILE)
os.system('echo go >> ' + FILE)
os.system('echo exit >> ' + FILE)

if platform.system() == 'Windows':
    os.system('JLink -device lpc55s69 -if swd -speed 2000 -autoconnect 1 -commanderscript ' + FILE)

else:
    os.system('JLinkExe -device lpc55s69 -if swd -speed 2000 -autoconnect 1 -commanderscript ' +FILE)
