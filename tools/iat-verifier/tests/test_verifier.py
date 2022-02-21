# -----------------------------------------------------------------------------
# Copyright (c) 2019-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

import os
import tempfile
import unittest

from iatverifier.util import convert_map_to_token_files
from iatverifier.verify import extract_iat_from_cose, PSAIoTProfile1TokenVerifier
from iatverifier.verifiers import VerifierConfiguration


THIS_DIR = os.path.dirname(__file__)

DATA_DIR = os.path.join(THIS_DIR, 'data')
KEYFILE = os.path.join(DATA_DIR, 'key.pem')
KEYFILE_ALT = os.path.join(DATA_DIR, 'key-alt.pem')


def create_token(source_name, keyfile, verifier):
    source_path = os.path.join(DATA_DIR, source_name)
    fd, dest_path = tempfile.mkstemp()
    os.close(fd)
    convert_map_to_token_files(source_path, keyfile, verifier, dest_path)
    return dest_path


def read_iat(filename, keyfile, verifier):
    filepath = os.path.join(DATA_DIR, filename)
    raw_iat = extract_iat_from_cose(keyfile, filepath, verifier)
    return verifier.decode_and_validate_iat(raw_iat)


def create_and_read_iat(source_name, keyfile, verifier):
    token_file = create_token(source_name, keyfile, verifier)
    return read_iat(token_file, keyfile, verifier)


class TestIatVerifier(unittest.TestCase):

    def setUp(self):
        self.config = VerifierConfiguration()

    def test_validate_signature(self):
        verifier = PSAIoTProfile1TokenVerifier.get_verifier(self.config)
        good_sig = create_token('valid-iat.yaml', KEYFILE, verifier)
        bad_sig = create_token('valid-iat.yaml', KEYFILE_ALT, verifier)

        raw_iat = extract_iat_from_cose(KEYFILE, good_sig, verifier)

        with self.assertRaises(ValueError) as cm:
            raw_iat = extract_iat_from_cose(KEYFILE, bad_sig, verifier)

        self.assertIn('Bad signature', cm.exception.args[0])

    def test_validate_iat_structure(self):
        iat = create_and_read_iat('valid-iat.yaml', KEYFILE, PSAIoTProfile1TokenVerifier.get_verifier(self.config))

        with self.assertRaises(ValueError) as cm:
            iat = create_and_read_iat('invalid-profile-id.yaml', KEYFILE, PSAIoTProfile1TokenVerifier.get_verifier(self.config))
        self.assertIn('Invalid PROFILE_ID', cm.exception.args[0])

        with self.assertRaises(ValueError) as cm:
            iat = read_iat('malformed.cbor', KEYFILE, PSAIoTProfile1TokenVerifier.get_verifier(self.config))
        self.assertIn('Bad COSE', cm.exception.args[0])

        with self.assertRaises(ValueError) as cm:
            iat = create_and_read_iat('missing-claim.yaml', KEYFILE, PSAIoTProfile1TokenVerifier.get_verifier(self.config))
        self.assertIn('missing MANDATORY claim', cm.exception.args[0])

        with self.assertRaises(ValueError) as cm:
            iat = create_and_read_iat('submod-missing-claim.yaml', KEYFILE, PSAIoTProfile1TokenVerifier.get_verifier(self.config))
        self.assertIn('missing MANDATORY claim', cm.exception.args[0])

        with self.assertRaises(ValueError) as cm:
            iat = create_and_read_iat('missing-sw-comps.yaml', KEYFILE, PSAIoTProfile1TokenVerifier.get_verifier(self.config))
        self.assertIn('NO_MEASUREMENTS claim is not present',
                      cm.exception.args[0])

        with self.assertLogs() as cm:
            iat = create_and_read_iat('missing-signer-id.yaml', KEYFILE, PSAIoTProfile1TokenVerifier.get_verifier(self.config))
            self.assertIn('Missing RECOMMENDED claim "SIGNER_ID" from sw_component',
                         cm.records[0].getMessage())

        with self.assertLogs() as cm:
            keep_going_conf = VerifierConfiguration(keep_going=True)
            iat = create_and_read_iat('invalid-type-length.yaml', KEYFILE, PSAIoTProfile1TokenVerifier.get_verifier(keep_going_conf))
            self.assertIn("Invalid PROFILE_ID: must be a(n) <class 'str'>: found <class 'int'>",
                         cm.records[0].getMessage())
            self.assertIn("Invalid SIGNER_ID: must be a(n) <class 'bytes'>: found <class 'str'>",
                         cm.records[1].getMessage())
            self.assertIn("Invalid SIGNER_ID length: must be at least 32 bytes, found 12 bytes",
                         cm.records[2].getMessage())
            self.assertIn("Invalid MEASUREMENT length: must be at least 32 bytes, found 28 bytes",
                         cm.records[3].getMessage())


    def test_binary_string_decoding(self):
        iat = create_and_read_iat('valid-iat.yaml', KEYFILE, PSAIoTProfile1TokenVerifier.get_verifier(self.config))
        self.assertEqual(iat['SECURITY_LIFECYCLE'], 'SL_SECURED')

    def test_security_lifecycle_decoding(self):
        iat = create_and_read_iat('valid-iat.yaml', KEYFILE, PSAIoTProfile1TokenVerifier.get_verifier(self.config))
        self.assertEqual(iat['SECURITY_LIFECYCLE'], 'SL_SECURED')
