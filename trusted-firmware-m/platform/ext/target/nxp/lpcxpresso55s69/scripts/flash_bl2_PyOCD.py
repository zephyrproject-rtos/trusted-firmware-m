# Copyright (c) 2021, NXP Semiconductors. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause

import os

# Move to the TFM build folder
os.chdir('../../../../../../build/bin')

target = 'LPC55S69'

# Flash with PyOCD
os.system('pyocd erase --mass -t ' + target)                                    # erase the flash memory
os.system('pyocd flash bl2.hex -t ' + target)                                   # flash the secure image into target
os.system('pyocd flash tfm_s_signed.bin --base-address 0x8000 -t ' + target)    # flash the signed secure image into target
os.system('pyocd flash tfm_ns_signed.bin --base-address 0x30000 -t ' + target)  # flash the signed non-secure image into target
