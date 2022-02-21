# -----------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

from iatverifier.verifiers import AttestationTokenVerifier as Verifier
from iatverifier.verifiers import AttestationClaim as Claim
from iatverifier.verifiers import ProfileIdClaim, ClientIdClaim, SecurityLifecycleClaim
from iatverifier.verifiers import ImplementationIdClaim, BootSeedClaim, HardwareIdClaim
from iatverifier.verifiers import NoMeasurementsClaim, ChallengeClaim
from iatverifier.verifiers import InstanceIdClaim, OriginatorClaim, SWComponentsClaim
from iatverifier.verifiers import SWComponentTypeClaim, SwComponentVersionClaim
from iatverifier.verifiers import MeasurementValueClaim, MeasurementDescriptionClaim, SignerIdClaim

class PSAIoTProfile1TokenVerifier(Verifier):
    @staticmethod
    def get_verifier(configuration=None):
        verifier = PSAIoTProfile1TokenVerifier(
            method=Verifier.SIGN_METHOD_SIGN1,
            cose_alg=Verifier.COSE_ALG_ES256,
            configuration=configuration)

        sw_component_claims = [
            (SWComponentTypeClaim, (Claim.OPTIONAL, )),
            (SwComponentVersionClaim, (Claim.OPTIONAL, )),
            (MeasurementValueClaim, (Claim.MANDATORY, )),
            (MeasurementDescriptionClaim, (Claim.OPTIONAL, )),
            (SignerIdClaim, (Claim.RECOMMENDED, )),
        ]

        verifier.add_claims([
            ProfileIdClaim(verifier, Claim.OPTIONAL),
            ClientIdClaim(verifier, Claim.MANDATORY),
            SecurityLifecycleClaim(verifier, Claim.MANDATORY),
            ImplementationIdClaim(verifier, Claim.MANDATORY),
            BootSeedClaim(verifier, Claim.MANDATORY),
            HardwareIdClaim(verifier, Claim.OPTIONAL),
            SWComponentsClaim(verifier, sw_component_claims, Claim.OPTIONAL),
            NoMeasurementsClaim(verifier, Claim.OPTIONAL),
            ChallengeClaim(verifier, Claim.MANDATORY),
            InstanceIdClaim(verifier, 33, Claim.MANDATORY),
            OriginatorClaim(verifier, Claim.OPTIONAL),
        ])
        return verifier
