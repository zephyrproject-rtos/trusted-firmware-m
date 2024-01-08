#-------------------------------------------------------------------------------
# Copyright (c) 2021-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

import argparse
import binascii

import sys
import os
sys.path.append(os.path.join(sys.path[0],'..'))
from provisioning_common_utils import *

parser = argparse.ArgumentParser()
parser.add_argument("--provisioning_code", help="the input provisioning code", required=True)
parser.add_argument("--provisioning_data", help="the input provisioning data", required=True)
parser.add_argument("--provisioning_values", help="the input provisioning values", required=True)
parser.add_argument("--tp_mode", help="the test or production mode", required=True)
parser.add_argument("--krtl_file", help="the RTL key file", required=True)
parser.add_argument("--bl1_2_padded_hash_input_file", help="the hash of the final bl1_2 image", required=True)
parser.add_argument("--bl1_2_input_file", help="the final bl1_2 image", required=True)
parser.add_argument("--otp_dma_ics_input_file", help="OTP DMA ICS input file", required=True)
parser.add_argument("--bundle_output_file", help="bundle output file", required=True)
args = parser.parse_args()

with open(args.provisioning_code, "rb") as in_file:
    code = in_file.read()

# It's technically possible to have provisioning code that requires no DATA
# section, so this is optional.
try:
    with open(args.provisioning_data, "rb") as in_file:
        data = in_file.read()
except FileNotFoundError:
    data = bytes(0)

with open(args.provisioning_values, "rb") as in_file:
    values = in_file.read()

if args.bl1_2_padded_hash_input_file:
    with open(args.bl1_2_padded_hash_input_file, "rb") as in_file:
        bl1_2_padded_hash = in_file.read()
else:
    bl1_2_padded_hash = bytes(0)

if args.bl1_2_input_file:
    with open(args.bl1_2_input_file, "rb") as in_file:
        bl1_2 = in_file.read()
else:
    bl1_2 = bytes(0)

assert(args.tp_mode == "TCI" or args.tp_mode == "PCI")
if args.tp_mode == "TCI":
    tp_mode = 0x111155AA
elif args.tp_mode == "PCI":
    tp_mode = 0x2222AA55


with open(args.krtl_file, "rb") as in_file:
    input_key = in_file.read()

with open(args.otp_dma_ics_input_file, "rb") as in_file:
    otp_dma_ics = in_file.read()
otp_dma_ics = struct_pack([ otp_dma_ics], pad_to=0x400 - 4)
otp_ics_crc = binascii.crc32(otp_dma_ics).to_bytes(4, byteorder='little')
otp_dma_ics = struct_pack([otp_ics_crc, otp_dma_ics])

# These are always required
patch_bundle = struct_pack([
    bl1_2_padded_hash,
    bl1_2,
    otp_dma_ics,
])

values = patch_binary(values, patch_bundle, 0)

bundle = encrypt_bundle(code, 0xB000, values, 0x3800, data, 0x3E00, 0xC0DEFEED,
                        input_key, 0, tp_mode, "CM_PROVISIONING")

with open(args.bundle_output_file, "wb") as out_file:
    out_file.write(bundle)
