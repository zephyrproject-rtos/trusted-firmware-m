# -----------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

from iatverifier.verifiers import AttestationTokenVerifier as Verifier
from iatverifier.verifiers import ProfileIdClaim, ClientIdClaim, SecurityLifecycleClaim
from iatverifier.verifiers import ImplementationIdClaim, BootSeedClaim, HardwareIdClaim
from iatverifier.verifiers import NoMeasurementsClaim, ChallengeClaim
from iatverifier.verifiers import InstanceIdClaim, OriginatorClaim, SWComponentsClaim
from iatverifier.verifiers import SWComponentTypeClaim, SwComponentVersionClaim, MeasurementValueClaim
from iatverifier.verifiers import MeasurementDescriptionClaim, SignerIdClaim

class PSAIoTProfile1TokenVerifier(Verifier):
    @staticmethod
    def get_verifier(configuration=None):
        verifier = PSAIoTProfile1TokenVerifier(method=Verifier.SIGN_METHOD_SIGN1, cose_alg=Verifier.COSE_ALG_ES256, configuration=configuration)

        sw_component_claims = [
            (SWComponentTypeClaim, (False, )),
            (SwComponentVersionClaim, (False, )),
            (MeasurementValueClaim, (True, )),
            (MeasurementDescriptionClaim, (False, )),
            (SignerIdClaim, (False, )),
        ]

        verifier.add_claims([
            ProfileIdClaim(verifier, False),
            ClientIdClaim(verifier, True),
            SecurityLifecycleClaim(verifier, True),
            ImplementationIdClaim(verifier, True),
            BootSeedClaim(verifier, True),
            HardwareIdClaim(verifier, False),
            SWComponentsClaim(verifier, sw_component_claims, False),
            NoMeasurementsClaim(verifier, False),
            ChallengeClaim(verifier, True),
            InstanceIdClaim(verifier, 33, True),
            OriginatorClaim(verifier, False),
        ])
        return verifier
