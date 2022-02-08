# -----------------------------------------------------------------------------
# Copyright (c) 2019-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

import logging

import cbor2

logging.basicConfig(level=logging.INFO, format='%(levelname)8s: %(message)s')
logger = logging.getLogger('iat-verify')

seen_errors = False

# IAT custom claims
ARM_RANGE = -75000

# SW component IDs
SW_COMPONENT_RANGE = 0

class Verifier:
    def __init__(self, configuration, mandatory=True):
        self.config = configuration
        self.mandatory = mandatory
        self.verify_count = 0

    def verify(self, value):
        raise NotImplementedError

    def get_claim_key(self):
        raise NotImplementedError

    def get_claim_name(self):
        raise NotImplementedError

    def get_claim_key_list(self):
        return {}

    def error(self, message):
        global seen_errors
        seen_errors = True
        if self.config.keep_going:
            logger.error(message)
        else:
            raise ValueError(message)

    def decode(self, value):
        if self.is_utf_8():
            try:
                return value.decode()
            except UnicodeDecodeError as e:
                msg = 'Error decodeing value for "{}": {}'
                self.error(msg.format(self.get_claim_name(), e))
                return str(value)[2:-1]
        else:  # not a UTF-8 value, i.e. a bytestring
            return value

    def add_tokens_to_dict(self, token, value):
        entry_name = self.get_claim_name()
        if isinstance(value, bytes):
            value = self.decode(value)
        token[entry_name] = value

    def all_mandatory_found(self):
        return (not self.mandatory) or self.verify_count>0

    def _validate_bytestring_length(self, value, name, expected_len):
        if not isinstance(value, bytes):
            msg = 'Invalid {}: must be a bytes string: found {}'
            self.error(msg.format(name, type(value)))

        value_len = len(value)
        if value_len != expected_len:
            msg = 'Invalid {} length: must be exactly {} bytes, found {} bytes'
            self.error(msg.format(name, expected_len, value_len))

    def parse_raw(self, raw_value):
        return raw_value

    def get_formatted_value(self, value):
        return value

    def is_utf_8(self):
        return False


# ----------------------------------------------------------------------------
# Validation classes
#
class InstanceIdVerifier(Verifier):
    def get_claim_key(self):
        return ARM_RANGE - 9  # UEID

    def get_claim_name(self):
        return 'INSTANCE_ID'

    def verify(self, value):
        self._validate_bytestring_length(value, 'INSTANCE_ID', 33)
        if value[0] != 0x01:
            msg = 'Invalid INSTANCE_ID: first byte must be 0x01, found: 0x{}'
            self.error(msg.format(value[0]))
        self.verify_count += 1


class ChallengeVerifier(Verifier):

    HASH_SIZES = [32, 48, 64]

    def get_claim_key(self):
        return ARM_RANGE - 8  # nonce

    def get_claim_name(self):
        return 'CHALLENGE'

    def verify(self, value):
        if not isinstance(value, bytes):
            msg = 'Invalid CHALLENGE; must be a bytes string.'
            self.error(msg)

        value_len = len(value)
        if value_len not in ChallengeVerifier.HASH_SIZES:
            msg = 'Invalid CHALLENGE length; must one of {}, found {} bytes'
            self.error(msg.format(ChallengeVerifier.HASH_SIZES, value_len))
        self.verify_count += 1


class AlwaysPassVerifier(Verifier):
    def verify(self, value):
        self.verify_count += 1

    def get_claim_key(self):
        raise NotImplementedError

    def get_claim_name(self):
        raise NotImplementedError


class ImplementationIdVerifier(AlwaysPassVerifier):
    def get_claim_key(self):
        return ARM_RANGE - 3

    def get_claim_name(self):
        return 'IMPLEMENTATION_ID'


class HardwareIdVerifier(AlwaysPassVerifier):
    def get_claim_key(self):
        return ARM_RANGE - 5

    def get_claim_name(self):
        return 'HARDWARE_ID'


class OriginatorVerifier(AlwaysPassVerifier):
    def get_claim_key(self):
        return ARM_RANGE - 10  # originator

    def get_claim_name(self):
        return 'ORIGINATOR'

    def is_utf_8(self):
        return True



class SWComponentsVerifier(Verifier):

    def get_claim_key(self):
        return ARM_RANGE - 6

    def get_claim_name(self):
        return 'SW_COMPONENTS'

    def get_sw_component_verifiers(self):
        return [
            SWComponentTypeVerifier(self.config, mandatory=False),
            SwComponentVersionVerifier(self.config, mandatory=False),
            MeasurementValueVerifier(self.config, mandatory=True),
            MeasurementDescriptionVerifier(self.config, mandatory=False),
            SignerIdVerifier(self.config, mandatory=False),
        ]

    def get_claim_key_list(self):
        ret = {}
        for verifier in self.get_sw_component_verifiers():
            ret[verifier.get_claim_key()] = verifier.__class__
        return ret

    def verify(self, value):
        if not isinstance(value, list):
            msg = 'Invalid SW_COMPONENTS value (must be an array): {}'
            self.error(msg.format(value))
            return

        for entry_number, sw_component in enumerate(value):
            if not isinstance(sw_component, dict):
                msg = 'Invalid SW_COMPONENTS array entry (must be a map): {}'
                self.error(msg.format(sw_component))
                return

            verifiers = {v.get_claim_key(): v for v in self.get_sw_component_verifiers()}
            for k, v in sw_component.items():
                if k not in verifiers.keys():
                    if self.config.strict:
                        msg = 'Unexpected SW_COMPONENT claim: {}'
                        self.error(msg.format(k))
                    else:
                        continue
                try:
                    verifiers[k].verify(v)
                except Exception:
                    if not self.config.keep_going:
                        raise
            for verifier in verifiers.values():
                if not verifier.all_mandatory_found():
                    msg = ('Invalid IAT: missing MANDATORY claim "{}" '
                           'from sw_componentule at index {}')
                    self.error(msg.format(verifier.get_claim_name(),
                                     entry_number))
        self.verify_count += 1

    def decode_sw_component(self, raw_sw_component):
        sw_component = {}
        names = {verifier.get_claim_key(): verifier.get_claim_name() for verifier in self.get_sw_component_verifiers()}
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
            self.error('Invalid SW_COMPONENT value: {}'.format(value))

class SWComponentTypeVerifier(AlwaysPassVerifier):
    def get_claim_key(self):
        return SW_COMPONENT_RANGE + 1

    def get_claim_name(self):
        return 'SW_COMPONENT_TYPE'

    def is_utf_8(self):
        return True


class NoMeasurementsVerifier(AlwaysPassVerifier):
    def get_claim_key(self):
        return ARM_RANGE - 7

    def get_claim_name(self):
        return 'NO_MEASUREMENTS'


class ClientIdVerifier(Verifier):
    def get_claim_key(self):
        return ARM_RANGE - 1

    def get_claim_name(self):
        return 'CLIENT_ID'

    def verify(self, value):
        if not isinstance(value, int):
            msg = 'Invalid CLIENT_ID, must be an int: {}'
            self.error(msg.format(value))
        self.verify_count += 1

class SecurityLifecycleVerifier(Verifier):

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

    def get_claim_key(self):
        return ARM_RANGE - 2

    def get_claim_name(self):
        return 'SECURITY_LIFECYCLE'

    def verify(self, value):
        if not isinstance(value, int):
            msg = 'Invalid SECURITY_LIFECYCLE, must be an int: {}'
            self.error(msg.format(value))
        self.verify_count += 1

    def add_tokens_to_dict(self, token, value):
        entry_name = self.get_claim_name()
        try:
            name_idx = (value >> SecurityLifecycleVerifier.SL_SHIFT) - 1
            token[entry_name] = SecurityLifecycleVerifier.SL_NAMES[name_idx]
        except IndexError:
            token[entry_name] = 'CUSTOM({})'.format(value)

    def parse_raw(self, raw_value):
        name_idx = SecurityLifecycleVerifier.SL_NAMES.index(raw_value.upper())
        return (name_idx + 1) << SecurityLifecycleVerifier.SL_SHIFT

    def get_formatted_value(self, value):
        return (SecurityLifecycleVerifier.SL_NAMES[(value >> SecurityLifecycleVerifier.SL_SHIFT) - 1])


class ProfileIdVerifier(Verifier):
    def get_claim_key(self):
        return ARM_RANGE

    def get_claim_name(self):
        return 'PROFILE_ID'

    def verify(self, value):
        if not isinstance(value, str):
            msg = 'Invalid PROFILE_ID (must be a string): {}'.format(value)
            self.error(msg.format(value))
        self.verify_count += 1

    def is_utf_8(self):
        return True


class BootSeedVerifier(Verifier):
    def get_claim_key(self):
        return ARM_RANGE - 4

    def get_claim_name(self):
        return 'BOOT_SEED'

    def verify(self, value):
        self._validate_bytestring_length(value, 'BOOT_SEED', 32)
        self.verify_count += 1


class SignerIdVerifier(Verifier):
    def get_claim_key(self):
        return SW_COMPONENT_RANGE + 5

    def get_claim_name(self):
        return 'SIGNER_ID'

    def verify(self, value):
        self._validate_bytestring_length(value, 'SIGNER_ID', 32)
        self.verify_count += 1


class SwComponentVersionVerifier(AlwaysPassVerifier):
    def get_claim_key(self):
        return SW_COMPONENT_RANGE + 4

    def get_claim_name(self):
        return 'SW_COMPONENT_VERSION'

    def is_utf_8(self):
        return True


class MeasurementValueVerifier(Verifier):
    def get_claim_key(self):
        return SW_COMPONENT_RANGE + 2

    def get_claim_name(self):
        return 'MEASUREMENT_VALUE'

    def verify(self, value):
        self._validate_bytestring_length(value, 'MEASUREMENT', 32)
        self.verify_count += 1


class MeasurementDescriptionVerifier(AlwaysPassVerifier):
    def get_claim_key(self):
        return SW_COMPONENT_RANGE + 6

    def get_claim_name(self):
        return 'MEASUREMENT_DESCRIPTION'

    def is_utf_8(self):
        return True


# ----------------------------------------------------------------------------

class AttestationTokenVerifier:

    all_known_verifiers = {}

    def __init__(self, verifiers):
        for verifier in verifiers:
            key = verifier.get_claim_key()
            if key not in AttestationTokenVerifier.all_known_verifiers:
                AttestationTokenVerifier.all_known_verifiers[key] = verifier.__class__

            AttestationTokenVerifier.all_known_verifiers.update(verifier.get_claim_key_list())
        self.verifiers = verifiers

    def decode_and_validate_iat(self, encoded_iat):
        try:
            raw_token = cbor2.loads(encoded_iat)
        except Exception as e:
            msg = 'Invalid CBOR: {}'
            raise ValueError(msg.format(e))

        verifiers = {v.get_claim_key(): v for v in self.verifiers}

        token = {}
        for entry in raw_token.keys():
            value = raw_token[entry]

            try:
                verifier = verifiers[entry]
            except KeyError:
                if self.config.strict:
                    self.error('Invalid IAT claim: {}'.format(entry))
                token[entry] = value
                continue

            verifier.verify(value)
            verifier.add_tokens_to_dict(token, value)

            config = verifier.config

        sw_component_present = False
        no_measurement_present = False
        for verifier in verifiers.values():
            if isinstance(verifier, SWComponentsVerifier) and verifier.verify_count>0:
                sw_component_present = True
            if isinstance(verifier, NoMeasurementsVerifier) and verifier.verify_count>0:
                no_measurement_present = True
            if not verifier.all_mandatory_found():
                msg = 'Invalid IAT: missing MANDATORY claim "{}"'
                verifier.error(msg.format(verifier.get_claim_name()))

        if not sw_component_present and not no_measurement_present:
            Verifier(config).error('Invalid IAT: no software measurements defined and '
                  'NO_MEASUREMENTS claim is not present.')

        return token
