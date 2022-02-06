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


class Verifier:
    def __init__(self, configuration):
        self.config = configuration

    def verify(self):
        raise NotImplementedError

def error(message, keep_going=False):
    global seen_errors
    seen_errors = True
    if keep_going:
        logger.error(message)
    else:
        raise ValueError(message)


def decode(value, key, keep_going=False):
    if key in const.IS_UTF_8:
        try:
            return value.decode()
        except UnicodeDecodeError as e:
            msg = 'Error decodeing value for "{}": {}'
            error(msg.format(key, e), keep_going)
            return str(value)[2:-1]
    else:  # not a UTF-8 value, i.e. a bytestring
        return value


# ----------------------------------------------------------------------------
# Validation classes
#
class InstanceIdVerifier(Verifier):
    def verify(self, value):
        _validate_bytestring_length(value, 'INSTANCE_ID', 33, self.config.keep_going)
        if value[0] != 0x01:
            msg = 'Invalid INSTANCE_ID: first byte must be 0x01, found: 0x{}'
            error(msg.format(value[0]), self.config.keep_going)


class ChallengeVerifier(Verifier):
    def verify(self, value):
        if not isinstance(value, bytes):
            msg = 'Invalid CHALLENGE; must be a bytes string.'
            error(msg, self.config.keep_going)

        value_len = len(value)
        if value_len not in const.HASH_SIZES:
            msg = 'Invalid CHALLENGE length; must one of {}, found {} bytes'
            error(msg.format(const.HASH_SIZES, value_len), self.config.keep_going)


class AlwaysPassVerifier(Verifier):
    def verify(self, value):
        pass

class ImplementationIdVerifier(AlwaysPassVerifier):
    pass

class HardwareIdVerifier(AlwaysPassVerifier):
    pass

class OriginatorVerifier(AlwaysPassVerifier):
    pass

class SWComponentsVerifier(Verifier):
    def verify(self, value):
        if not isinstance(value, list):
            msg = 'Invalid SW_COMPONENTS value (must be an array): {}'
            error(msg.format(value), self.config.keep_going)
            return

        for sw_component in value:
            if not isinstance(sw_component, dict):
                msg = 'Invalid SW_COMPONENTS array entry (must be a map): {}'
                error(msg.format(sw_component), self.config.keep_going)
                return

            for k, v in sw_component.items():
                if k not in const.ALLOWED_SW_COMPONENT_CLAIMS:
                    if self.config.strict:
                        msg = 'Unexpected SW_COMPONENT claim: {}'
                        error(msg.format(k), self.config.keep_going)
                    else:
                        continue
                try:
                    class Configuration:
                        pass
                    validator_obj = validation_classes[k](self.config)
                    validator_obj.verify(v)
                except Exception:
                    if not self.config.keep_going:
                        raise

class SWComponentTypeVerifier(AlwaysPassVerifier):
    pass

class NoMeasurementsVerifier(AlwaysPassVerifier):
    pass

class ClientIdVerifier(Verifier):
    def verify(self, value):
        if not isinstance(value, int):
            msg = 'Invalid CLIENT_ID, must be an int: {}'
            error(msg.format(value), self.config.keep_going)

class SecurityLifecycleVerifier(Verifier):
    def verify(self, value):
        if not isinstance(value, int):
            msg = 'Invalid SECURITY_LIFECYCLE, must be an int: {}'
            error(msg.format(value), self.config.keep_going)

class ProfileIdVerifier(Verifier):
    def verify(self, value):
        if not isinstance(value, str):
            msg = 'Invalid PROFILE_ID (must be a string): {}'.format(value)
            error(msg.format(value), self.config.keep_going)


class BootSeedVerifier(Verifier):
    def verify(self, value):
        _validate_bytestring_length(value, 'BOOT_SEED', 32, self.config.keep_going)


class SignerIdVerifier(Verifier):
    def verify(self, value):
        _validate_bytestring_length(value, 'SIGNER_ID', 32, self.config.keep_going)


class SwComponentVersionVerifier(AlwaysPassVerifier):
    pass

class MeasurementValueVerifier(Verifier):
    def verify(self, value):
        _validate_bytestring_length(value, 'MEASUREMENT', 32, self.config.keep_going)

class MeasurementDescriptionVerifier(AlwaysPassVerifier):
    pass

def get_verifier_classes():
    lowercase_globals = {name.replace('_', '').lower():globals().get(name)
                    for name in globals().keys()}
    validation_classes = {v: lowercase_globals["{}verifier".format(n.replace('_', '').lower())]
                    for v, n in const.NAMES.items()}
    return validation_classes

validation_classes = get_verifier_classes()

def validate_manadatory_claims(token, keep_going=False):
    for mand_claim in const.MANDATORY_CLAIMS:
        if mand_claim not in token:
            msg = 'Invalid IAT: missing MANDATORY claim "{}"'
            error(msg.format(const.NAMES[mand_claim]), keep_going)

    if const.SW_COMPONENTS in token:
        if (not token[const.SW_COMPONENTS] and
                const.NO_MEASUREMENTS not in token):
            error('Invalid IAT: no software measurements defined and '
                  'NO_MEASUREMENTS claim is not present.')

        for entry_number, sw_component_entry in \
                enumerate(token[const.SW_COMPONENTS]):
            for mand_claim in const.MANDATORY_SW_COMPONENT_CLAIMS:
                if mand_claim not in sw_component_entry:
                    msg = ('Invalid IAT: missing MANDATORY claim "{}" '
                           'from sw_componentule at index {}')
                    error(msg.format(const.NAMES[mand_claim],
                                     entry_number),
                          keep_going)

    elif const.NO_MEASUREMENTS not in token:
        error('Invalid IAT: no software measurements defined and '
              'NO_MEASUREMENTS claim is not present.')


def _validate_bytestring_length(value, name, expected_len, keep_going=False):
    if not isinstance(value, bytes):
        msg = 'Invalid {}: must be a bytes string: found {}'
        error(msg.format(name, type(value)), keep_going)

    value_len = len(value)
    if value_len != expected_len:
        msg = 'Invalid {} length: must be exactly {} bytes, found {} bytes'
        error(msg.format(name, expected_len, value_len), keep_going)
# ----------------------------------------------------------------------------


def decode_sw_component(raw_sw_component, keep_going=True, strict=False):
    sw_component = {}
    for k, v in raw_sw_component.items():
        if isinstance(v, bytes):
            v = decode(v, k, keep_going)
        try:
            sw_component[const.NAMES[k]] = v
        except KeyError:
            if strict:
                if not keep_going:
                    raise
            else:
                sw_component[k] = v
    return sw_component


def decode_and_validate_iat(encoded_iat, keep_going=False, strict=False):
    try:
        raw_token = cbor2.loads(encoded_iat)
    except Exception as e:
        msg = 'Invalid CBOR: {}'
        raise ValueError(msg.format(e))

    validate_manadatory_claims(raw_token, keep_going)

    token = {}
    for entry in raw_token.keys():
        try:
            entry_name = const.NAMES[entry]
        except KeyError:
            if strict:
                error('Invalid IAT claim: {}'.format(entry), keep_going)
            if isinstance(value, bytes):
                value = decode(value, entry, keep_going)
            token[entry] = value
            continue

        value = raw_token[entry]
        class Configuration:
            pass
        config = Configuration()
        config.keep_going = keep_going
        config.strict = strict
        validator_obj = validation_classes[entry](config)
        validator_obj.verify(value)
        if entry_name == 'SW_COMPONENTS':
            try:
                token[entry_name] = []
                for raw_sw_component in value:
                    decoded_component = decode_sw_component(raw_sw_component,
                                                            keep_going)
                    token[entry_name].append(decoded_component)
            except TypeError:
                error('Invalid SW_COMPONENT value: {}'.format(value),
                      keep_going)
        elif entry_name == 'SECURITY_LIFECYCLE':
            try:
                name_idx = (value >> const.SL_SHIFT) - 1
                token[entry_name] = const.SL_NAMES[name_idx]
            except IndexError:
                token[entry_name] = 'CUSTOM({})'.format(value)
        else:  # not SW_COMPONENT or SECURITY_LIFECYCLE
            if isinstance(value, bytes):
                value = decode(value, entry_name, keep_going)
            token[entry_name] = value

    return token
