# -----------------------------------------------------------------------------
# Copyright (c) 2019-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

import logging

import cbor2

from iatverifier import const

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
        return {self.get_claim_name(): self.get_claim_key()}

    def error(self, message):
        global seen_errors
        seen_errors = True
        if self.config.keep_going:
            logger.error(message)
        else:
            raise ValueError(message)


    def decode(self, value, key):
        if key in const.IS_UTF_8:
            try:
                return value.decode()
            except UnicodeDecodeError as e:
                msg = 'Error decodeing value for "{}": {}'
                self.error(msg.format(key, e))
                return str(value)[2:-1]
        else:  # not a UTF-8 value, i.e. a bytestring
            return value

    def add_tokens_to_dict(self, token, value):
        entry_name = self.get_claim_name()
        if isinstance(value, bytes):
            value = self.decode(value, entry_name)
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
    def get_claim_key(self):
        return ARM_RANGE - 8  # nonce

    def get_claim_name(self):
        return 'CHALLENGE'

    def verify(self, value):
        if not isinstance(value, bytes):
            msg = 'Invalid CHALLENGE; must be a bytes string.'
            self.error(msg)

        value_len = len(value)
        if value_len not in const.HASH_SIZES:
            msg = 'Invalid CHALLENGE length; must one of {}, found {} bytes'
            self.error(msg.format(const.HASH_SIZES, value_len))
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
        ret = super().get_claim_key_list()
        for verifier in self.get_sw_component_verifiers():
            ret.update(verifier.get_claim_key_list())
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
                v = self.decode(v, k)
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
            name_idx = (value >> const.SL_SHIFT) - 1
            token[entry_name] = const.SL_NAMES[name_idx]
        except IndexError:
            token[entry_name] = 'CUSTOM({})'.format(value)


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

# ----------------------------------------------------------------------------

class PSAIoTProfile1TokenVerifier(Verifier):
    def __init__(self, configuration, mandatory=True):
        super().__init__(configuration, mandatory)
        self.claims = [
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

    def get_claim_key_list(self):
        ret = {}
        for verifier in self.claims:
            ret.update(verifier.get_claim_key_list())
        return ret

    def decode_and_validate_iat(self, encoded_iat):
        try:
            raw_token = cbor2.loads(encoded_iat)
        except Exception as e:
            msg = 'Invalid CBOR: {}'
            raise ValueError(msg.format(e))

        verifiers = {v.get_claim_key(): v for v in self.claims}

        token = {}
        for entry in raw_token.keys():
            value = raw_token[entry]

            try:
                verifier = verifiers[entry]
            except KeyError:
                if self.config.strict:
                    self.error('Invalid IAT claim: {}'.format(entry))
                if isinstance(value, bytes):
                    value = self.decode(value, entry)
                token[entry] = value
                continue

            verifier.verify(value)
            verifier.add_tokens_to_dict(token, value)

        sw_component_present = False
        no_measurement_present = False
        for verifier in verifiers.values():
            if isinstance(verifier, SWComponentsVerifier) and verifier.verify_count>0:
                sw_component_present = True
            if isinstance(verifier, NoMeasurementsVerifier) and verifier.verify_count>0:
                no_measurement_present = True
            if not verifier.all_mandatory_found():
                msg = 'Invalid IAT: missing MANDATORY claim "{}"'
                self.error(msg.format(verifier.get_claim_name()))

        if not sw_component_present and not no_measurement_present:
            self.error('Invalid IAT: no software measurements defined and '
                  'NO_MEASUREMENTS claim is not present.')

        return token

    def verify(self, value):
        return self.decode_and_validate_iat(value)


def get_field_names():

    field_names = {}
    for sym in globals().values():
        try:
            issubclass(sym, globals().get('Verifier'))
        except TypeError:
            continue

        verifier = sym(None)
        try:
            claim_key = verifier.get_claim_key()
            claim_name = verifier.get_claim_name()
        except NotImplementedError:
            continue

        field_names[claim_name] = claim_key

    return field_names
