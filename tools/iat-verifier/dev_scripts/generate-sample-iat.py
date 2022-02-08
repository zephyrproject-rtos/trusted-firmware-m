#!/usr/bin/env python3
# -----------------------------------------------------------------------------
# Copyright (c) 2019-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

import base64
import struct

import cbor2
from ecdsa import SigningKey
from pycose.sign1message import Sign1Message

from iatverifier.util import sign_eat

from iatverifier.verifiers import InstanceIdVerifier, ImplementationIdVerifier, ChallengeVerifier
from iatverifier.verifiers import ClientIdVerifier, SecurityLifecycleVerifier, ProfileIdVerifier
from iatverifier.verifiers import BootSeedVerifier, SWComponentsVerifier, SWComponentTypeVerifier
from iatverifier.verifiers import SignerIdVerifier, SwComponentVersionVerifier
from iatverifier.verifiers import MeasurementValueVerifier, MeasurementDescriptionVerifier

# First byte indicates "GUID"
GUID = b'\x01' + struct.pack('QQQQ', 0x0001020304050607, 0x08090A0B0C0D0E0F,
                             0x1011121314151617, 0x18191A1B1C1D1E1F)
NONCE = struct.pack('QQQQ', 0X0001020304050607, 0X08090A0B0C0D0E0F,
                    0X1011121314151617, 0X18191A1B1C1D1E1F)
ORIGIN = struct.pack('QQQQ', 0X0001020304050607, 0X08090A0B0C0D0E0F,
                     0X1011121314151617, 0X18191A1B1C1D1E1F)
BOOT_SEED = struct.pack('QQQQ', 0X0001020304050607, 0X08090A0B0C0D0E0F,
                        0X1011121314151617, 0X18191A1B1C1D1E1F)
SIGNER_ID = struct.pack('QQQQ', 0X0001020304050607, 0X08090A0B0C0D0E0F,
                        0X1011121314151617, 0X18191A1B1C1D1E1F)
MEASUREMENT = struct.pack('QQQQ', 0X0001020304050607, 0X08090A0B0C0D0E0F,
                          0X1011121314151617, 0X18191A1B1C1D1E1F)

token_map = {
  InstanceIdVerifier.get_claim_key(None): GUID,
  ImplementationIdVerifier.get_claim_key(None): ORIGIN,
  ChallengeVerifier.get_claim_key(None): NONCE,
  ClientIdVerifier.get_claim_key(None): 2,
  SecurityLifecycleVerifier.get_claim_key(None): SecurityLifecycleVerifier.SL_SECURED,
  ProfileIdVerifier.get_claim_key(None): 'http://example.com',
  BootSeedVerifier.get_claim_key(None): BOOT_SEED,
  SWComponentsVerifier.get_claim_key(None): [
        {
            # bootloader
            SWComponentTypeVerifier.get_claim_key(None): 'BL',
            SignerIdVerifier.get_claim_key(None): SIGNER_ID,
            SwComponentVersionVerifier.get_claim_key(None): '3.4.2',
            MeasurementValueVerifier.get_claim_key(None): MEASUREMENT,
            MeasurementDescriptionVerifier.get_claim_key(None): 'TF-M_SHA256MemPreXIP',
        },
        {
            # mod1
            SWComponentTypeVerifier.get_claim_key(None): 'M1',
            SignerIdVerifier.get_claim_key(None): SIGNER_ID,
            SwComponentVersionVerifier.get_claim_key(None): '3.4.2',
            MeasurementValueVerifier.get_claim_key(None): MEASUREMENT,
        },
        {
            # mod2
            SWComponentTypeVerifier.get_claim_key(None): 'M2',
            SignerIdVerifier.get_claim_key(None): SIGNER_ID,
            SwComponentVersionVerifier.get_claim_key(None): '3.4.2',
            MeasurementValueVerifier.get_claim_key(None): MEASUREMENT,
        },
        {
            # mod3
            SWComponentTypeVerifier.get_claim_key(None): 'M3',
            SignerIdVerifier.get_claim_key(None): SIGNER_ID,
            SwComponentVersionVerifier.get_claim_key(None): '3.4.2',
            MeasurementValueVerifier.get_claim_key(None): MEASUREMENT,
        },
    ],
}


if __name__ == '__main__':
    import sys
    if len(sys.argv) != 3:
        print('Usage: {} KEYFILE OUTFILE'.format(sys.argv[0]))
        sys.exit(1)
    keyfile = sys.argv[1]
    outfile = sys.argv[2]

    sk = SigningKey.from_pem(open(keyfile, 'rb').read())
    token = cbor2.dumps(token_map)
    signed_token = sign_eat(token, sk)

    with open(outfile, 'wb') as wfh:
        wfh.write(signed_token)
