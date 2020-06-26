# -----------------------------------------------------------------------------
# Copyright (c) 2019-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

from collections.abc import Iterable
from copy import deepcopy

import cbor
import yaml
from ecdsa import SigningKey, VerifyingKey
from pycose.sign1message import Sign1Message
from pycose.mac0message import Mac0Message

from iatverifier import const


FIELD_NAMES = {v: k for k, v in const.NAMES.items()}


def sign_eat(token, key=None):
    signed_msg = Sign1Message()
    signed_msg.payload = token
    if key:
        signed_msg.key = key
        signed_msg.signature = signed_msg.compute_signature()
    return signed_msg.encode()


def hmac_eat(token, key=None):
    hmac_msg = Mac0Message(payload=token, key=key)
    hmac_msg.compute_auth_tag('HS256')
    return hmac_msg.encode()


def convert_map_to_token_files(mapfile, keyfile, outfile, method='sign'):
    token_map = read_token_map(mapfile)

    if method == 'sign':
        with open(keyfile) as fh:
            signing_key = SigningKey.from_pem(fh.read())
    else:
        with open(keyfile, 'rb') as fh:
            signing_key = fh.read()

    with open(outfile, 'wb') as wfh:
        convert_map_to_token(token_map, signing_key, wfh, raw)


def convert_map_to_token(token_map, signing_key, wfh, method='sign'):
    token = cbor.dumps(token_map)

    if method == 'raw':
        signed_token = token
    elif method == 'sign':
        signed_token = sign_eat(token, signing_key)
    elif method == 'mac':
        signed_token = hmac_eat(token, signing_key)
    else:
        err_msg = 'Unexpected method "{}"; must be one of: raw, sign, mac'
        raise ValueError(err_msg.format(method))

    wfh.write(signed_token)


def convert_token_to_map(raw_data):
    payload = get_cose_payload(raw_data)
    token_map = cbor.loads(payload)
    return _relabel_keys(token_map)


def read_token_map(f):
    if hasattr(f, 'read'):
        raw = yaml.safe_load(f)
    else:
        with open(f) as fh:
            raw = yaml.safe_load(fh)

    return _parse_raw_token(raw)


def extract_iat_from_cose(keyfile, tokenfile, keep_going=False, method='sign'):
    key = read_keyfile(keyfile, method)

    try:
        with open(tokenfile, 'rb') as wfh:
            return get_cose_payload(wfh.read(), key, method)
    except Exception as e:
        msg = 'Bad COSE file "{}": {}'
        raise ValueError(msg.format(tokenfile, e))


def get_cose_payload(cose, key=None, method='sign'):
    if method == 'sign':
        return get_cose_sign1_payload(cose, key)
    elif method == 'mac':
        return get_cose_mac0_pyload(cose, key)
    else:
        err_msg = 'Unexpected method "{}"; must be one of: sign, mac'
        raise ValueError(err_msg.format(method))


def get_cose_sign1_payload(cose, key=None):
    msg = Sign1Message.decode(cose)
    if key:
        msg.key = key
        msg.signature = msg.signers
        try:
            msg.verify_signature(alg='ES256')
        except Exception as e:
            raise ValueError('Bad signature ({})'.format(e))
    return msg.payload


def get_cose_mac0_pyload(cose, key=None):
    msg = Mac0Message.decode(cose)
    if key:
        msg.key = key
        try:
            msg.verify_auth_tag(alg='HS256')
        except Exception as e:
            raise ValueError('Bad signature ({})'.format(e))
    return msg.payload

def recursive_bytes_to_strings(d, in_place=False):
    if in_place:
        result = d
    else:
        result = deepcopy(d)

    if hasattr(result, 'items'):
        for k, v in result.items():
            result[k] = recursive_bytes_to_strings(v, in_place=True)
    elif (isinstance(result, Iterable) and
            not isinstance(result, (str, bytes))):
        result = [recursive_bytes_to_strings(r, in_place=True)
                  for r in result]
    elif isinstance(result, bytes):
        result = str(result)[2:-1]

    return result


def read_keyfile(keyfile, method='sign'):
    if keyfile:
        if method == 'sign':
            return read_sign1_key(keyfile)
        elif method == 'mac':
            return read_hmac_key(keyfile)
        else:
            err_msg = 'Unexpected method "{}"; must be one of: sign, mac'
            raise ValueError(err_msg.format(method))
    else:  # no keyfile
        key = None

    return key


def read_sign1_key(keyfile):
    try:
        key = SigningKey.from_pem(open(keyfile, 'rb').read())
    except Exception as e:
        signing_key_error = str(e)

        try:
            key = VerifyingKey.from_pem(open(keyfile, 'rb').read())
        except Exception as e:
            verifying_key_error = str(e)

            msg = 'Bad key file "{}":\n\tpubkey error: {}\n\tprikey error: {}'
            raise ValueError(msg.format(keyfile, verifying_key_error, signing_key_error))


def read_hmac_key(keyfile):
    return open(keyfile, 'rb').read()


def _parse_raw_token(raw):
    result = {}
    for raw_key, raw_value in raw.items():
        if isinstance(raw_key, int):
            key = raw_key
        else:
            field_name = raw_key.upper()
            try:
                key = FIELD_NAMES[field_name]
            except KeyError:
                msg = 'Unknown field "{}" in token.'.format(field_name)
                raise ValueError(msg)

        if key == const.SECURITY_LIFECYCLE:
            name_idx = const.SL_NAMES.index(raw_value.upper())
            value = (name_idx + 1) << const.SL_SHIFT
        elif hasattr(raw_value, 'items'):
            value = _parse_raw_token(raw_value)
        elif (isinstance(raw_value, Iterable) and
                not isinstance(raw_value, (str, bytes))):
            # TODO  -- asumes dict elements
            value = [_parse_raw_token(v) for v in raw_value]
        else:
            value = raw_value

        result[key] = value

    return result


def _relabel_keys(token_map):
    result = {}
    for key, value in token_map.items():
        if hasattr(value, 'items'):
            value = _relabel_keys(value)
        elif (isinstance(value, Iterable) and
                not isinstance(value, (str, bytes))):
            # TODO  -- asumes dict elements
            value = [_relabel_keys(v) for v in value]

        if key == const.SECURITY_LIFECYCLE:
            value = (const.SL_NAMES[(value >> const.SL_SHIFT) - 1])

        if key in const.NAMES:
            new_key = const.NAMES[key].lower()
        else:
            new_key = key
        result[new_key] = value
    return result
