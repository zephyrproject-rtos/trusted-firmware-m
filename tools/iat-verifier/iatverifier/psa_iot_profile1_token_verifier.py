# -----------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

from iatverifier.verifiers import AttestationTokenVerifier
from iatverifier.verifiers import ProfileIdClaim, ClientIdClaim, SecurityLifecycleClaim
from iatverifier.verifiers import ImplementationIdClaim, BootSeedClaim, HardwareIdClaim
from iatverifier.verifiers import NoMeasurementsClaim, ChallengeClaim
from iatverifier.verifiers import InstanceIdClaim, OriginatorClaim, SWComponentsClaim

class PSAIoTProfile1TokenVerifier(AttestationTokenVerifier):
    @staticmethod
    def get_verifier(configuration):
        verifier = PSAIoTProfile1TokenVerifier(configuration)
        verifier.add_claims([
            ProfileIdClaim(verifier, False),
            ClientIdClaim(verifier, True),
            SecurityLifecycleClaim(verifier, True),
            ImplementationIdClaim(verifier, True),
            BootSeedClaim(verifier, True),
            HardwareIdClaim(verifier, False),
            SWComponentsClaim(verifier, False),
            NoMeasurementsClaim(verifier, False),
            ChallengeClaim(verifier, True),
            InstanceIdClaim(verifier, True),
            OriginatorClaim(verifier, False),
        ])
        return verifier
