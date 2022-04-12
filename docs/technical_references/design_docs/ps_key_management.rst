========================================
Protected Storage service key management
========================================

:Author: Jamie Fox
:Organization: Arm Limited
:Contact: Jamie Fox <jamie.fox@arm.com>

Background
==========
The PSA Protected Storage API requires confidentiality for external storage to
be provided by:

    **cryptographic ciphers using device-bound keys**, a tamper resistant
    enclosure, or an inaccessible deployment location, depending on the threat
    model of the deployed system.

A TBSA-M-compliant device must embed a Hardware Unique Key (HUK), which provides
the root of trust (RoT) for confidentiality in the system. It must have at least
128 bits of entropy (and a 128 bit data size), and be accessible only to Trusted
code or Trusted hardware that acts on behalf of Trusted code. [TBSA-M]_

Design description
==================
Each time the system boots, PS will request that the Crypto service uses a key
derivation function (KDF) to derive a storage key from the HUK. The storage key
could be kept in on-chip volatile memory private to the Crypto partition, or it
could remain inside a secure element. Either way it will not be returned to PS.

For each call to the PSA Protected Storage APIs, PS will make requests to the
Crypto service to perform AEAD encryption and/or decryption operations using the
storage key (providing a fresh nonce for each encryption).

At no point will PS access the key material itself, only referring to the HUK
and storage key by their handles in the Crypto service.

Key derivation
==============
PS will make key derivation requests to the Crypto service with calls to the
PSA Crypto APIs. In order to derive the storage key, the following calls are
required:

.. code-block:: c

    static psa_key_id_t ps_key;

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_derivation_operation_t op = PSA_KEY_DERIVATION_OPERATION_INIT;

    /* Set the key attributes for the storage key */
    psa_set_key_usage_flags(&attributes, PS_KEY_USAGE);
    psa_set_key_algorithm(&attributes,
                          PSA_ALG_AEAD_WITH_SHORTENED_TAG(PS_CRYPTO_AEAD_ALG,
                                                          PS_TAG_LEN_BYTES));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, PSA_BYTES_TO_BITS(PS_KEY_LEN_BYTES));

    /* Set up a key derivation operation with HUK derivation as the alg */
    psa_key_derivation_setup(&op,
                             TFM_CRYPTO_ALG_HUK_DERIVATION);

    /* Supply the PS key label as an input to the key derivation */
    status = psa_key_derivation_input_bytes(&op,
                                            PSA_KEY_DERIVATION_INPUT_LABEL,
                                            key_label,
                                            key_label_len);

    /* Create the storage key from the key derivation operation */
    status = psa_key_derivation_output_key(&attributes,
                                           &op,
                                           &ps_key);

.. note::
    ``key_label`` is combined with client ID and UID in the PS crypto ref
    structure.

In the call to ``psa_key_derivation_setup()``, ``TFM_CRYPTO_ALG_HUK_DERIVATION``
is supplied as the key derivation algorithm argument. The algorithm identifier
refers to key derivation from the HUK and it can be implemented in a
platform-defined way (e.g. using a crypto accelerator). The system integrator
should choose the most optimal algorithm for the platform, or fall back to the
software implementation if none is available.

When implemented in software, the key derivation function used by the crypto
service to derive the storage key will be HKDF, with SHA-256 as the underlying
hash function. HKDF is suitable because:

- It is simple and efficient, requiring only two HMAC operations when the length
  of the output key material is less than or equal to the hash length (as is the
  case here).
- The trade-off is that HKDF is only suitable when the input key material has at
  least as much entropy as required for the output key material. But this is the
  case here, as the HUK has 128 bits of entropy, the same as required by PS.
- HKDF is standardised in RFC 5869 [RFC5869]_ and its security has been formally
  analysed. [HKDF]_
- It is supported by the TF-M Crypto service.

The choice of underlying hash function is fairly straightforward: it needs to be
a modern standardised algorithm, considered to be secure and supported by TF-M
Crypto. This narrows it down to just the SHA-2 family. Of the hash functions in
the family, SHA-256 is the simplest and provides more than enough output length.

Keeping the storage key private to PS
-------------------------------------
The salt and label fields are not generally secret, so an Application RoT
service could request the Crypto service to derive the same storage key from the
HUK, which violates isolation between Application RoT partitions to some extent.
This could be fixed in a number of ways:

- Only PSA RoT partitions can request Crypto to derive keys from the HUK.

  - But then either PS has to be in the PSA RoT or request a service in the PSA
    RoT to do the derivation on its behalf.

- PS has a secret (pseudo)random salt, accessible only to it, that it uses to
  derive the storage key.

  - Where would this salt be stored? It cannot be generated fresh each boot
    because the storage key must stay the same across reboots.

- The Crypto service appends the partition ID to the label, so that no two
  partitions can derive the same key.

  - Still need to make sure only PSA RoT partitions can directly access the HUK
    or Secure Enclave. The label is not secret, so any actor that can access the
    HUK could simply perform the derivation itself, rather than making a request
    to the Crypto service.

The third option would solve the issue with the fewest drawbacks, so this option
is the one that is proposed.

Key use
=======
To encrypt and decrypt data, PS will call the PSA Crypto AEAD APIs in the same
way as the current implementation, but ``ps_key`` will refer to the storage key,
rather than the imported HUK. For each encryption operation, the following call
is made (and analogously for decryption):

.. code-block:: c

    psa_aead_encrypt(ps_key, PS_CRYPTO_ALG,
                     crypto->ref.iv, PS_IV_LEN_BYTES,
                     add, add_len,
                     in, in_len,
                     out, out_size, out_len);

References
==========
.. [TBSA-M] Arm Platform Security Architecture Trusted Base System Architecture
   for Armv6-M, Armv7-M and Armv8-M, version 1.0
.. [HKDF] Hugo Krawczyk. 2010. Cryptographic extraction and key derivation: the
   HKDF scheme. In Proceedings of the 30th annual conference on Advances in
   cryptology (CRYPTO'10)
.. [RFC5869] IETF RFC 5869: HMAC-based Extract-and-Expand Key Derivation
   Function (HKDF)

--------------

*Copyright (c) 2019-2022, Arm Limited. All rights reserved.*
