# -----------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

from iatverifier.verifiers import AttestationTokenVerifier
from iatverifier.verifiers import ProfileIdVerifier, ClientIdVerifier, SecurityLifecycleVerifier
from iatverifier.verifiers import ImplementationIdVerifier, BootSeedVerifier, HardwareIdVerifier
from iatverifier.verifiers import SwComponentVersionVerifier, NoMeasurementsVerifier, ChallengeVerifier
from iatverifier.verifiers import InstanceIdVerifier, OriginatorVerifier, SWComponentsVerifier

class PSAIoTProfile1TokenVerifier(AttestationTokenVerifier):
    def __init__(self, configuration):
        self.config = configuration
        claims = [
            ProfileIdVerifier(self.config, False),
            ClientIdVerifier(self.config, True),
            SecurityLifecycleVerifier(self.config, True),
            ImplementationIdVerifier(self.config, True),
            BootSeedVerifier(self.config, True),
            HardwareIdVerifier(self.config, False),
            SWComponentsVerifier(self.config, False),
            NoMeasurementsVerifier(self.config, False),
            ChallengeVerifier(self.config, True),
            InstanceIdVerifier(self.config, True),
            OriginatorVerifier(self.config, False),
        ]
        super().__init__(claims)
