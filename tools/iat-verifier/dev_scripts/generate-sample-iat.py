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

from iatverifier.verifiers import InstanceIdClaim, ImplementationIdClaim, ChallengeClaim
from iatverifier.verifiers import ClientIdClaim, SecurityLifecycleClaim, ProfileIdClaim
from iatverifier.verifiers import BootSeedClaim, SWComponentsClaim, SWComponentTypeClaim
from iatverifier.verifiers import SignerIdClaim, SwComponentVersionClaim
from iatverifier.verifiers import MeasurementValueClaim, MeasurementDescriptionClaim

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
  InstanceIdClaim.get_claim_key(): GUID,
  ImplementationIdClaim.get_claim_key(): ORIGIN,
  ChallengeClaim.get_claim_key(): NONCE,
  ClientIdClaim.get_claim_key(): 2,
  SecurityLifecycleClaim.get_claim_key(): SecurityLifecycleClaim.SL_SECURED,
  ProfileIdClaim.get_claim_key(): 'http://example.com',
  BootSeedClaim.get_claim_key(): BOOT_SEED,
  SWComponentsClaim.get_claim_key(): [
        {
            # bootloader
            SWComponentTypeClaim.get_claim_key(): 'BL',
            SignerIdClaim.get_claim_key(): SIGNER_ID,
            SwComponentVersionClaim.get_claim_key(): '3.4.2',
            MeasurementValueClaim.get_claim_key(): MEASUREMENT,
            MeasurementDescriptionClaim.get_claim_key(): 'TF-M_SHA256MemPreXIP',
        },
        {
            # mod1
            SWComponentTypeClaim.get_claim_key(): 'M1',
            SignerIdClaim.get_claim_key(): SIGNER_ID,
            SwComponentVersionClaim.get_claim_key(): '3.4.2',
            MeasurementValueClaim.get_claim_key(): MEASUREMENT,
        },
        {
            # mod2
            SWComponentTypeClaim.get_claim_key(): 'M2',
            SignerIdClaim.get_claim_key(): SIGNER_ID,
            SwComponentVersionClaim.get_claim_key(): '3.4.2',
            MeasurementValueClaim.get_claim_key(): MEASUREMENT,
        },
        {
            # mod3
            SWComponentTypeClaim.get_claim_key(): 'M3',
            SignerIdClaim.get_claim_key(): SIGNER_ID,
            SwComponentVersionClaim.get_claim_key(): '3.4.2',
            MeasurementValueClaim.get_claim_key(): MEASUREMENT,
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
