# -----------------------------------------------------------------------------
# Copyright (c) 2019-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

from collections.abc import Iterable
from copy import deepcopy

import base64
import cbor2
import yaml
from ecdsa import SigningKey, VerifyingKey
from pycose.sign1message import Sign1Message
from pycose.mac0message import Mac0Message
from iatverifier.verifiers import AttestationTokenVerifier

def sign_eat(token, key=None):
    signed_msg = Sign1Message()
    signed_msg.payload = token
    if key:
        signed_msg.key = key
        signed_msg.signature = signed_msg.compute_signature()
    return signed_msg.encode()


def hmac_eat(token, verifier, key=None):
    hmac_msg = Mac0Message(payload=token, key=key)
    hmac_msg.compute_auth_tag(verifier.cose_alg)
    return hmac_msg.encode()


def convert_map_to_token_files(mapfile, keyfile, verifier, outfile):
    token_map = read_token_map(mapfile)

    if verifier.method == 'sign':
        with open(keyfile) as fh:
            signing_key = SigningKey.from_pem(fh.read())
    else:
        with open(keyfile, 'rb') as fh:
            signing_key = fh.read()

    with open(outfile, 'wb') as wfh:
        convert_map_to_token(token_map, signing_key, verifier, wfh,)


def convert_map_to_token(token_map, signing_key, verifier, wfh):
    token = cbor2.dumps(token_map)

    if verifier.method == AttestationTokenVerifier.SIGN_METHOD_RAW:
        signed_token = token
    elif verifier.method == AttestationTokenVerifier.SIGN_METHOD_SIGN1:
        signed_token = sign_eat(token, signing_key)
    elif verifier.method == AttestationTokenVerifier.SIGN_METHOD_MAC0:
        signed_token = hmac_eat(token, verifier, signing_key)
    else:
        err_msg = 'Unexpected method "{}"; must be one of: raw, sign, mac'
        raise ValueError(err_msg.format(method))

    wfh.write(signed_token)


def convert_token_to_map(raw_data, verifier):
    payload = get_cose_payload(raw_data, verifier)
    token_map = cbor2.loads(payload)
    return _relabel_keys(token_map)


def read_token_map(f):
    if hasattr(f, 'read'):
        raw = yaml.safe_load(f)
    else:
        with open(f) as fh:
            raw = yaml.safe_load(fh)

    return _parse_raw_token(raw)


def extract_iat_from_cose(keyfile, tokenfile, verifier):
    key = read_keyfile(keyfile, verifier.method)

    try:
        with open(tokenfile, 'rb') as wfh:
            return get_cose_payload(wfh.read(), verifier, key)
    except Exception as e:
        msg = 'Bad COSE file "{}": {}'
        raise ValueError(msg.format(tokenfile, e))


def get_cose_payload(cose, verifier, key=None):
    if verifier.method == AttestationTokenVerifier.SIGN_METHOD_SIGN1:
        return get_cose_sign1_payload(cose, verifier, key)
    if verifier.method == AttestationTokenVerifier.SIGN_METHOD_MAC0:
        return get_cose_mac0_pyload(cose, verifier, key)
    err_msg = 'Unexpected method "{}"; must be one of: sign, mac'
    raise ValueError(err_msg.format(method))


def get_cose_sign1_payload(cose, verifier, key=None):
    msg = Sign1Message.decode(cose)
    if key:
        msg.key = key
        msg.signature = msg.signers
        try:
            msg.verify_signature(alg=verifier.cose_alg)
        except Exception as e:
            raise ValueError('Bad signature ({})'.format(e))
    return msg.payload


def get_cose_mac0_pyload(cose, verifier, key=None):
    msg = Mac0Message.decode(cose)
    if key:
        msg.key = key
        try:
            msg.verify_auth_tag(alg=verifier.cose_alg)
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
        result = str(base64.b16encode(result))

    return result


def read_keyfile(keyfile, method=AttestationTokenVerifier.SIGN_METHOD_SIGN1):
    if keyfile:
        if method == AttestationTokenVerifier.SIGN_METHOD_SIGN1:
            return read_sign1_key(keyfile)
        if method == AttestationTokenVerifier.SIGN_METHOD_MAC0:
            return read_hmac_key(keyfile)
        err_msg = 'Unexpected method "{}"; must be one of: sign, mac'
        raise ValueError(err_msg.format(method))

    return None


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
    return key


def read_hmac_key(keyfile):
    return open(keyfile, 'rb').read()

def _get_known_claims():
    for _, claim_class in AttestationTokenVerifier.all_known_claims.items():
        yield claim_class

def _parse_raw_token(raw):
    result = {}
    field_names = {cc.get_claim_name(): cc for cc in _get_known_claims()}
    for raw_key, raw_value in raw.items():
        if isinstance(raw_key, int):
            key = raw_key
        else:
            field_name = raw_key.upper()
            try:
                claim_class = field_names[field_name]
                key = claim_class.get_claim_key()
            except KeyError:
                msg = 'Unknown field "{}" in token.'.format(field_name)
                raise ValueError(msg)

        if hasattr(raw_value, 'items'):
            value = _parse_raw_token(raw_value)
        elif (isinstance(raw_value, Iterable) and
                not isinstance(raw_value, (str, bytes))):
            # TODO  -- asumes dict elements
            value = [_parse_raw_token(v) for v in raw_value]
        else:
            value = claim_class.parse_raw(raw_value)

        result[key] = value

    return result

def _format_value(names, key, value):
    if key in names:
        value = names[key].get_formatted_value(value)
    return value

def _relabel_keys(token_map):
    result = {}
    while not hasattr(token_map, 'items'):
        # TODO: token map is not a map. We are assuming that it is a tag
        token_map = token_map.value
    names = {v.get_claim_key(): v for v in _get_known_claims()}
    for key, value in token_map.items():
        if hasattr(value, 'items'):
            value = _relabel_keys(value)
        elif (isinstance(value, Iterable) and
                not isinstance(value, (str, bytes))):
            new_value = []
            for item in value:
                if hasattr(item, 'items'):
                    new_value.append(_relabel_keys(item))
                else:
                    new_value.append(_format_value(names, key, item))
            value = new_value
        else:
            value = _format_value(names, key, value)

        if key in names:
            new_key = names[key].get_claim_name().lower()
        else:
            new_key = key
        result[new_key] = value
    return result
