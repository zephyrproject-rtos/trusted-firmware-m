#-------------------------------------------------------------------------------
# Copyright (c) 2021-2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

import argparse
import struct
import secrets

def struct_pack(objects, pad_to=0):
    defstring = "<"
    for obj in objects:
        defstring += str(len(obj)) + "s"

    size = struct.calcsize(defstring)
    if size < pad_to:
        defstring += str(pad_to - size) + "x"
    elif size > pad_to and pad_to != 0:
        print("Error padding struct of size {} to {}".format(size, pad_to))
        exit(1);

    return (bytes(struct.pack(defstring, *objects)))

parser = argparse.ArgumentParser()
parser.add_argument("--provisioning_code", help="the input provisioning code", required=True)
parser.add_argument("--provisioning_data", help="the input provisioning data", required=True)
parser.add_argument("--provisioning_values", help="the input provisioning values", required=True)
parser.add_argument("--magic", help="the magic constant to insert at the start and end", required=True)
parser.add_argument("--bl1_2_padded_hash_input_file", help="the hash of the final bl1_2 image", required=False)
parser.add_argument("--bl1_2_input_file", help="the final bl1_2 image", required=False)
parser.add_argument("--bundle_output_file", help="bundle output file", required=False)
args = parser.parse_args()

with open(args.provisioning_code, "rb") as in_file:
    code = in_file.read()

with open(args.provisioning_data, "rb") as in_file:
    data = in_file.read()

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

patch_bundle = struct_pack([
    bl1_2_padded_hash,
    bl1_2
])

code = struct_pack([code], pad_to=0x8000)
values = struct_pack([patch_bundle, values[len(patch_bundle):]], pad_to=0x3800)
data = struct_pack([values, data], pad_to=0x3E00)

bundle = struct_pack([
    int(args.magic, 16).to_bytes(4, 'little'),
    code,
    data,
    bytes(16), # Replace with GCM TAG
    int(args.magic, 16).to_bytes(4, 'little'),
])

with open(args.bundle_output_file, "wb") as out_file:
    out_file.write(bundle)
