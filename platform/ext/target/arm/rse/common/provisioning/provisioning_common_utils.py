#-------------------------------------------------------------------------------
# Copyright (c) 2021-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

import struct
import secrets
from cryptography.hazmat.primitives.ciphers.aead import AESCCM
from cryptography.hazmat.primitives import cmac
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
import hashlib

def patch_binary(binary, patch_bundle, offset):
    return binary[:offset] + patch_bundle + binary[offset + len(patch_bundle):]

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

def encrypt_bundle(code, code_pad, values, values_pad, data,
                   data_and_values_pad, magic, input_key):
    code = struct_pack([code], pad_to=code_pad)
    values = struct_pack([values], pad_to=values_pad)
    data = struct_pack([values, data], pad_to=data_and_values_pad)

    ccm_iv = secrets.token_bytes(12)

    to_auth = struct_pack([
        magic.to_bytes(8, 'little'),
    ])

    to_encrypt = struct_pack([
        code,
        data,
    ])

    encrypted_data = AESCCM(input_key, tag_length=16).encrypt(ccm_iv, to_encrypt, to_auth)
    tag = encrypted_data[-16:]
    encrypted_data = encrypted_data[:-16]

    bundle = struct_pack([
        to_auth,
        encrypted_data,
        ccm_iv,
        tag,
        magic.to_bytes(8, 'little'),
    ])

    return bundle
