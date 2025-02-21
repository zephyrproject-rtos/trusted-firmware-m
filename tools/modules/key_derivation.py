#!/usr/bin/env python3
# -----------------------------------------------------------------------------
# SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

from arg_utils import *
from cryptography.hazmat.primitives import cmac
from cryptography.hazmat.primitives.ciphers import algorithms

from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.hkdf import HKDF

import struct
import argparse
import logging
logger = logging.getLogger("TF-M")


def symmetric_kdf_sp800_108_cmac(input_key: bytes,
                                 context: bytes,
                                 label: bytes,
                                 length: int,
                                 **kwargs: dict,
                                 ):
    """
    This assumes r == 32 bits, i.e. the size in bits over which
    the counter [i]2 is encoded as a binary number. For simplicity
    we use 4-bytes to encode the length as well, [L]2

    Args:
        input_key (bytes): Ki from which to derive a Ko
        context (bytes): Context bytes
        label (bytes): label bytes
        length (int): Size in bytes of the Ko to be derived
    """
    def prf_cmac(x: bytes, key: bytes):
        c = cmac.CMAC(algorithms.AES(key))
        c.update(x)
        return c.finalize()

    # K(0) = PRF(K_in, Label || 0x00 || Context || [L]2)
    state = label + bytes(1) + context + struct.pack(">I", 8 * length)
    K0 = prf_cmac(state, input_key)

    output_key = bytes(0)
    # K(i) = PRF(K_in, [i]2 || Label || 0x00 || Context || [L]2 || K(0))
    for i in range((length // 16) + 1):
        state = struct.pack(">I", i + 1) + label + bytes(1) + \
            context + struct.pack(">I", 8 * length) + K0
        output_key += prf_cmac(state, input_key)

    return output_key[:length]


def symmetric_kdf_hkdf(input_key: bytes,
                       context: bytes,
                       label: bytes,
                       length: int,
                       hkdf_hash_alg: str = "SHA256",
                       **kwargs: dict,
                       ):
    state = label + context

    hkdf = HKDF(
        algorithm=hkdf_hashes[hkdf_hash_alg](),
        length=length,
        salt=None,
        info=state)

    return hkdf.derive(input_key)


kdf_funcs = {
    "sp800-108_cmac": symmetric_kdf_sp800_108_cmac,
    "hkdf": symmetric_kdf_hkdf,
}

hkdf_hashes = {
    "SHA256": hashes.SHA256,
    "SHA384": hashes.SHA384,
    "SHA512": hashes.SHA512,
}


def add_arguments(parser: argparse.ArgumentParser,
                  prefix: str = "",
                  required: bool = True,
                  ) -> None:
    add_prefixed_argument(parser, "kdf_alg", prefix,
                          help="key deriation algorithm",
                          choices=kdf_funcs, required=required)
    add_prefixed_argument(parser, "hkdf_hash_alg", prefix,
                          help="hkdf hash function",
                          choices=hkdf_hashes, required=False)


def parse_args(args: argparse.Namespace,
               prefix: str = "",
               ) -> dict:
    out = parse_args_automatically(args, ["kdf_alg", "hkdf_hash_alg"], prefix)
    return out


def derive_symmetric_key(input_key: bytes,
                         context: bytes,
                         label: str,
                         length: int,
                         kdf_alg: str = "sp800-108_cmac",
                         **kdf_args: dict
                         ):
    # C keeps the null byte, python removes it, so we add it back manually.
    label_bytes = label.encode('ascii') + bytes(1) if label else bytes(0)

    output_key = kdf_funcs[kdf_alg](
        input_key, context, label_bytes, length, **kdf_args)

    return output_key
