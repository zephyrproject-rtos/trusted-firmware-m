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


THIS_DIR = os.path.dirname(__file__)

DATA_DIR = os.path.join(THIS_DIR, 'data')
KEYFILE = os.path.join(DATA_DIR, 'key.pem')
KEYFILE_ALT = os.path.join(DATA_DIR, 'key-alt.pem')


def create_token(source_name, keyfile):
    source_path = os.path.join(DATA_DIR, source_name)
    fd, dest_path = tempfile.mkstemp()
    os.close(fd)
    convert_map_to_token_files(source_path, keyfile, dest_path)
    return dest_path


def read_iat(filename, keyfile, verifier):
    filepath = os.path.join(DATA_DIR, filename)
    raw_iat = extract_iat_from_cose(keyfile, filepath)
    return verifier.decode_and_validate_iat(raw_iat)


def create_and_read_iat(source_name, keyfile, verifier):
    token_file = create_token(source_name, keyfile)
    return read_iat(token_file, keyfile, verifier)


class TestIatVerifier(unittest.TestCase):

    def setUp(self):
        class Configuration:
            pass
        config = Configuration()
        config.keep_going = False
        config.strict = False
        self.config = config

    def test_validate_signature(self):
        good_sig = create_token('valid-iat.yaml', KEYFILE)
        bad_sig = create_token('valid-iat.yaml', KEYFILE_ALT)

        raw_iat = extract_iat_from_cose(KEYFILE, good_sig)

        with self.assertRaises(ValueError) as cm:
            raw_iat = extract_iat_from_cose(KEYFILE, bad_sig)

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

    def test_binary_string_decoding(self):
        iat = create_and_read_iat('valid-iat.yaml', KEYFILE, PSAIoTProfile1TokenVerifier.get_verifier(self.config))
        self.assertEqual(iat['SECURITY_LIFECYCLE'], 'SL_SECURED')

    def test_security_lifecycle_decoding(self):
        iat = create_and_read_iat('valid-iat.yaml', KEYFILE, PSAIoTProfile1TokenVerifier.get_verifier(self.config))
        self.assertEqual(iat['SECURITY_LIFECYCLE'], 'SL_SECURED')
