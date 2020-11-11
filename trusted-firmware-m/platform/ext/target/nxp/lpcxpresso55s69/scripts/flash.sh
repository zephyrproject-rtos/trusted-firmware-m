#!/bin/bash
# Copyright (c) 2020, Linaro. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause

# Exit on error
set -e

# Flash with JLinkExe
FILE=flash.jlink
if test -f "$FILE"; then
    rm $FILE
fi
echo "r
erase
loadfile bin/bl2.hex
loadfile bin/tfm_s_signed.bin 0x8000
loadfile bin/tfm_ns_signed.bin 0x30000
r
go
exit" >> $FILE
JLinkExe -device lpc55s69 -if swd -speed 2000 -autoconnect 1 -commanderscript $FILE
