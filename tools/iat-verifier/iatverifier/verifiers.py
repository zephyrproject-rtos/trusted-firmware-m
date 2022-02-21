# -----------------------------------------------------------------------------
# Copyright (c) 2019-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

import logging

import cbor2

logging.basicConfig(level=logging.INFO, format='%(levelname)8s: %(message)s')
logger = logging.getLogger('iat-verifiers')

# IAT custom claims
ARM_RANGE = -75000

# SW component IDs
SW_COMPONENT_RANGE = 0

class AttestationClaim:
    MANDATORY = 0
    RECOMMENDED = 1
    OPTIONAL = 2

    def __init__(self, verifier, necessity=MANDATORY):
        self.config = verifier.config
        self.verifier = verifier
        self.necessity = necessity
        self.verify_count = 0

    def verify(self, value):
        raise NotImplementedError

    def get_claim_key(self=None):
        raise NotImplementedError

    def get_claim_name(self=None):
        raise NotImplementedError

    def get_contained_claim_key_list(self):
        return {}

    def decode(self, value):
        if self.is_utf_8():
            try:
                return value.decode()
            except UnicodeDecodeError as e:
                msg = 'Error decodeing value for "{}": {}'
                self.verifier.error(msg.format(self.get_claim_name(), e))
                return str(value)[2:-1]
        else:  # not a UTF-8 value, i.e. a bytestring
            return value

    def add_tokens_to_dict(self, token, value):
        entry_name = self.get_claim_name()
        if isinstance(value, bytes):
            value = self.decode(value)
        token[entry_name] = value

    def claim_found(self):
        return self.verify_count>0

    def _validate_bytestring_length(self, value, name, expected_len):
        if not isinstance(value, bytes):
            msg = 'Invalid {}: must be a bytes string: found {}'
            self.verifier.error(msg.format(name, type(value)))

        value_len = len(value)
        if value_len != expected_len:
            msg = 'Invalid {} length: must be exactly {} bytes, found {} bytes'
            self.verifier.error(msg.format(name, expected_len, value_len))

    @staticmethod
    def parse_raw(raw_value):
        return raw_value

    @staticmethod
    def get_formatted_value(value):
        return value

    def is_utf_8(self):
        return False

    def check_cross_claim_requirements(self):
        pass


# ----------------------------------------------------------------------------
# Validation classes
#
class InstanceIdClaim(AttestationClaim):
    def __init__(self, verifier, expected_len, necessity=AttestationClaim.MANDATORY):
        super().__init__(verifier, necessity)
        self.expected_len = expected_len

    def get_claim_key(self=None):
        return ARM_RANGE - 9  # UEID

    def get_claim_name(self=None):
        return 'INSTANCE_ID'

    def verify(self, value):
        self._validate_bytestring_length(value, 'INSTANCE_ID', self.expected_len)
        if value[0] != 0x01:
            msg = 'Invalid INSTANCE_ID: first byte must be 0x01, found: 0x{}'
            self.verifier.error(msg.format(value[0]))
        self.verify_count += 1


class ChallengeClaim(AttestationClaim):

    HASH_SIZES = [32, 48, 64]

    def get_claim_key(self=None):
        return ARM_RANGE - 8  # nonce

    def get_claim_name(self=None):
        return 'CHALLENGE'

    def verify(self, value):
        if not isinstance(value, bytes):
            msg = 'Invalid CHALLENGE; must be a bytes string.'
            self.verifier.error(msg)

        value_len = len(value)
        if value_len not in ChallengeClaim.HASH_SIZES:
            msg = 'Invalid CHALLENGE length; must one of {}, found {} bytes'
            self.verifier.error(msg.format(ChallengeClaim.HASH_SIZES, value_len))
        self.verify_count += 1


class NonVerifiedClaim(AttestationClaim):
    def verify(self, value):
        self.verify_count += 1

    def get_claim_key(self=None):
        raise NotImplementedError

    def get_claim_name(self=None):
        raise NotImplementedError


class ImplementationIdClaim(NonVerifiedClaim):
    def get_claim_key(self=None):
        return ARM_RANGE - 3

    def get_claim_name(self=None):
        return 'IMPLEMENTATION_ID'


class HardwareIdClaim(NonVerifiedClaim):
    def get_claim_key(self=None):
        return ARM_RANGE - 5

    def get_claim_name(self=None):
        return 'HARDWARE_ID'


class OriginatorClaim(NonVerifiedClaim):
    def get_claim_key(self=None):
        return ARM_RANGE - 10  # originator

    def get_claim_name(self=None):
        return 'ORIGINATOR'

    def is_utf_8(self):
        return True



class SWComponentsClaim(AttestationClaim):

    def __init__(self, verifier, claims, necessity=AttestationClaim.MANDATORY):
        super().__init__(verifier, necessity)
        self.claims = claims


    def get_claim_key(self=None):
        return ARM_RANGE - 6

    def get_claim_name(self=None):
        return 'SW_COMPONENTS'

    def get_sw_component_claims(self):
        return [claim(self.verifier, *args) for claim, args in self.claims]

    def get_contained_claim_key_list(self):
        ret = {}
        for claim in self.get_sw_component_claims():
            ret[claim.get_claim_key()] = claim.__class__
        return ret

    def verify(self, value):
        if not isinstance(value, list):
            msg = 'Invalid SW_COMPONENTS value (must be an array): {}'
            self.verifier.error(msg.format(value))
            return

        for entry_number, sw_component in enumerate(value):
            if not isinstance(sw_component, dict):
                msg = 'Invalid SW_COMPONENTS array entry (must be a map): {}'
                self.verifier.error(msg.format(sw_component))
                return

            claims = {v.get_claim_key(): v for v in self.get_sw_component_claims()}
            for k, v in sw_component.items():
                if k not in claims.keys():
                    if self.config.strict:
                        msg = 'Unexpected SW_COMPONENT claim: {}'
                        self.verifier.error(msg.format(k))
                    else:
                        continue
                try:
                    claims[k].verify(v)
                except Exception:
                    if not self.config.keep_going:
                        raise

            # Check claims' necessity
            for claim in claims.values():
                if not claim.claim_found():
                    if claim.necessity==AttestationClaim.MANDATORY:
                        msg = ('Invalid IAT: missing MANDATORY claim "{}" '
                            'from sw_component at index {}')
                        self.verifier.error(msg.format(claim.get_claim_name(),
                                        entry_number))
                    elif claim.necessity==AttestationClaim.RECOMMENDED:
                        msg = ('Missing RECOMMENDED claim "{}" '
                            'from sw_component at index {}')
                        self.verifier.warning(msg.format(claim.get_claim_name(),
                                        entry_number))

        self.verify_count += 1

    def decode_sw_component(self, raw_sw_component):
        sw_component = {}
        names = {claim.get_claim_key(): claim.get_claim_name() for claim in self.get_sw_component_claims()}
        for k, v in raw_sw_component.items():
            if isinstance(v, bytes):
                v = self.decode(v)
            try:
                sw_component[names[k]] = v
            except KeyError:
                if self.config.strict:
                    if not self.config.keep_going:
                        raise
                else:
                    sw_component[k] = v
        return sw_component

    def add_tokens_to_dict(self, token, value):
        entry_name = self.get_claim_name()
        try:
            token[entry_name] = []
            for raw_sw_component in value:
                decoded_component = self.decode_sw_component(raw_sw_component)
                token[entry_name].append(decoded_component)
        except TypeError:
            self.verifier.error('Invalid SW_COMPONENT value: {}'.format(value))

class SWComponentTypeClaim(NonVerifiedClaim):
    def get_claim_key(self=None):
        return SW_COMPONENT_RANGE + 1

    def get_claim_name(self=None):
        return 'SW_COMPONENT_TYPE'

    def is_utf_8(self):
        return True


class NoMeasurementsClaim(NonVerifiedClaim):
    def get_claim_key(self=None):
        return ARM_RANGE - 7

    def get_claim_name(self=None):
        return 'NO_MEASUREMENTS'


class ClientIdClaim(AttestationClaim):
    def get_claim_key(self=None):
        return ARM_RANGE - 1

    def get_claim_name(self=None):
        return 'CLIENT_ID'

    def verify(self, value):
        if not isinstance(value, int):
            msg = 'Invalid CLIENT_ID, must be an int: {}'
            self.verifier.error(msg.format(value))
        self.verify_count += 1

class SecurityLifecycleClaim(AttestationClaim):

    SL_SHIFT = 12

    SL_NAMES = [
        'SL_UNKNOWN',
        'SL_PSA_ROT_PROVISIONING',
        'SL_SECURED',
        'SL_NON_PSA_ROT_DEBUG',
        'SL_RECOVERABLE_PSA_ROT_DEBUG',
        'SL_PSA_LIFECYCLE_DECOMMISSIONED',
    ]

    # Security Lifecycle claims
    SL_UNKNOWN = 0x1000
    SL_PSA_ROT_PROVISIONING = 0x2000
    SL_SECURED = 0x3000
    SL_NON_PSA_ROT_DEBUG = 0x4000
    SL_RECOVERABLE_PSA_ROT_DEBUG = 0x5000
    SL_PSA_LIFECYCLE_DECOMMISSIONED = 0x6000

    def get_claim_key(self=None):
        return ARM_RANGE - 2

    def get_claim_name(self=None):
        return 'SECURITY_LIFECYCLE'

    def verify(self, value):
        if not isinstance(value, int):
            msg = 'Invalid SECURITY_LIFECYCLE, must be an int: {}'
            self.verifier.error(msg.format(value))
        self.verify_count += 1

    def add_tokens_to_dict(self, token, value):
        entry_name = self.get_claim_name()
        try:
            name_idx = (value >> SecurityLifecycleClaim.SL_SHIFT) - 1
            token[entry_name] = SecurityLifecycleClaim.SL_NAMES[name_idx]
        except IndexError:
            token[entry_name] = 'CUSTOM({})'.format(value)

    @staticmethod
    def parse_raw(raw_value):
        name_idx = SecurityLifecycleClaim.SL_NAMES.index(raw_value.upper())
        return (name_idx + 1) << SecurityLifecycleClaim.SL_SHIFT

    @staticmethod
    def get_formatted_value(value):
        return SecurityLifecycleClaim.SL_NAMES[(value >> SecurityLifecycleClaim.SL_SHIFT) - 1]


class ProfileIdClaim(AttestationClaim):
    def get_claim_key(self=None):
        return ARM_RANGE

    def get_claim_name(self=None):
        return 'PROFILE_ID'

    def verify(self, value):
        if not isinstance(value, str):
            msg = 'Invalid PROFILE_ID (must be a string): {}'.format(value)
            self.verifier.error(msg.format(value))
        self.verify_count += 1

    def is_utf_8(self):
        return True


class BootSeedClaim(AttestationClaim):
    def get_claim_key(self=None):
        return ARM_RANGE - 4

    def get_claim_name(self=None):
        return 'BOOT_SEED'

    def verify(self, value):
        self._validate_bytestring_length(value, 'BOOT_SEED', 32)
        self.verify_count += 1


class VerificationServiceClaim(NonVerifiedClaim):
    def get_claim_key(self=None):
        return ARM_RANGE - 10

    def get_claim_name(self=None):
        return 'VERIFICATION_SERVICE'

    def is_utf_8(self):
        return True


class SignerIdClaim(AttestationClaim):
    def get_claim_key(self=None):
        return SW_COMPONENT_RANGE + 5

    def get_claim_name(self=None):
        return 'SIGNER_ID'

    def verify(self, value):
        self._validate_bytestring_length(value, 'SIGNER_ID', 32)
        self.verify_count += 1


class SwComponentVersionClaim(NonVerifiedClaim):
    def get_claim_key(self=None):
        return SW_COMPONENT_RANGE + 4

    def get_claim_name(self=None):
        return 'SW_COMPONENT_VERSION'

    def is_utf_8(self):
        return True


class MeasurementValueClaim(AttestationClaim):
    def get_claim_key(self=None):
        return SW_COMPONENT_RANGE + 2

    def get_claim_name(self=None):
        return 'MEASUREMENT_VALUE'

    def verify(self, value):
        self._validate_bytestring_length(value, 'MEASUREMENT', 32)
        self.verify_count += 1


class MeasurementDescriptionClaim(NonVerifiedClaim):
    def get_claim_key(self=None):
        return SW_COMPONENT_RANGE + 6

    def get_claim_name(self=None):
        return 'MEASUREMENT_DESCRIPTION'

    def is_utf_8(self):
        return True


# ----------------------------------------------------------------------------

class VerifierConfiguration:
    def __init__(self, keep_going=False, strict=False):
        self.keep_going=keep_going
        self.strict=strict

class AttestationTokenVerifier:

    all_known_claims = {}

    SIGN_METHOD_SIGN1 = "sign"
    SIGN_METHOD_MAC0 = "mac"
    SIGN_METHOD_RAW = "raw"

    COSE_ALG_ES256="ES256"
    COSE_ALG_ES384="ES384"
    COSE_ALG_ES512="ES512"
    COSE_ALG_HS256_64="HS256/64"
    COSE_ALG_HS256="HS256"
    COSE_ALG_HS384="HS384"
    COSE_ALG_HS512="HS512"

    def __init__(self, method, cose_alg, configuration=None, ):
        self.method = method
        self.cose_alg = cose_alg
        self.config = configuration if configuration is not None else VerifierConfiguration()
        self.claims = []

        self.seen_errors = False

    def add_claims(self, claims):
        for claim in claims:
            key = claim.get_claim_key()
            if key not in AttestationTokenVerifier.all_known_claims:
                AttestationTokenVerifier.all_known_claims[key] = claim.__class__

            AttestationTokenVerifier.all_known_claims.update(claim.get_contained_claim_key_list())
        self.claims.extend(claims)

    def check_cross_claim_requirements(self):
        claims = {v.get_claim_key(): v for v in self.claims}

        if SWComponentsClaim.get_claim_key() in claims:
            sw_component_present = claims[SWComponentsClaim.get_claim_key()].verify_count > 0
        else:
            sw_component_present = False

        if NoMeasurementsClaim.get_claim_key() in claims:
            no_measurement_present = claims[NoMeasurementsClaim.get_claim_key()].verify_count > 0
        else:
            no_measurement_present = False

        if not sw_component_present and not no_measurement_present:
            self.error('Invalid IAT: no software measurements defined and '
                  'NO_MEASUREMENTS claim is not present.')

    def decode_and_validate_iat(self, encoded_iat):
        try:
            raw_token = cbor2.loads(encoded_iat)
        except Exception as e:
            msg = 'Invalid CBOR: {}'
            raise ValueError(msg.format(e))

        claims = {v.get_claim_key(): v for v in self.claims}

        token = {}
        while not hasattr(raw_token, 'items'):
            # TODO: token map is not a map. We are assuming that it is a tag
            raw_token = raw_token.value
        for entry in raw_token.keys():
            value = raw_token[entry]

            try:
                claim = claims[entry]
            except KeyError:
                if self.config.strict:
                    self.error('Invalid IAT claim: {}'.format(entry))
                token[entry] = value
                continue

            claim.verify(value)
            claim.add_tokens_to_dict(token, value)

        # Check claims' necessity
        for claim in claims.values():
            if not claim.claim_found():
                if claim.necessity==AttestationClaim.MANDATORY:
                    msg = 'Invalid IAT: missing MANDATORY claim "{}"'
                    self.error(msg.format(claim.get_claim_name()))
                elif claim.necessity==AttestationClaim.RECOMMENDED:
                    msg = 'Missing RECOMMENDED claim "{}"'
                    self.warning(msg.format(claim.get_claim_name()))

            claim.check_cross_claim_requirements()

        self.check_cross_claim_requirements()

        return token


    def error(self, message):
        self.seen_errors = True
        if self.config.keep_going:
            logger.error(message)
        else:
            raise ValueError(message)

    def warning(self, message):
        logger.warning(message)
