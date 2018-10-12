# TF-M Initial Attestation Service Integration Guide

## Introduction
TF-M Initial Attestation Service allows the application to prove the device
identity during an authentication process to a verification entity. The initial
attestation service can create a token on request, which contains a fix set of
device specific data. Device must contain an attestation key pair, which is
unique per device. The token is signed with the private part of attestation key
pair. The public part of the key pair is known by the verification entity. The
public key is used to verify the token authenticity. The data items in the token
used to verify the device integrity and assess its trustworthiness. Attestation
key provisioning is out of scope for the attestation service and is expected to
take part during manufacturing of the device.

## Current service limitations

**CBOR encoding** - The final data encoding of initial attestation token
is going to follow the Concise Binary Object Representation, aka.
[CBOR](http://cbor.io/).
Currently TLV (Type-Length-Value) encoding is used.

**Signing of token** - In the current implementation the token is not signed.
Claims are concatenated in raw format and there is no signature applied. The
final implementation is going to use signed token. Signatures are going to be
generated according to the [COSE](https://datatracker.ietf.org/doc/rfc8152/)
format.

## Claims in the initial attestation token
The initial attestation token is formed of claims. A claim is a data entry,
which is represented in a key - value structure. The following fixed set of
claims are included in the token:
  - **Challenge** - Input object from caller. It can be a nonce value from a
  server, or a hash value of attested data.
  - **Client ID** - The ID of that secure partition or non-secure thread who
  called the Initial Attestation Service.
  - **Boot status** - Measurements(hash) of firmware components by secure
  bootloader.
  - **Boot seed** - Random number, which is non-volatile during the same boot
  cycle.
  - **Device ID** - Universally and globally unique ID of the device.

## Claims encoding
Currently claims are encoded in TLV format. The definition of TLV structure is
described in `bl2\include\tfm_boot_status.h`. The Initial Attestation Service
related type identifiers are described in
`secure_fw\services\initial_attestation\attestation.h`

Claim structure:
```
---------------------------------------------------------------
| Major_type(uint8_t) | Minor_type(uint8_t) | Length(uit16_t) |
---------------------------------------------------------------
|                         Raw data                            |
---------------------------------------------------------------
```

## Code structure
The PSA interface for the Initial Attestation Service is located in
`interface\include`.
The only header to be included by applications that want to use functions from
the PSA API is `psa_initial_attestation.h`.
The TF-M Initial Attestation Service source files are located in
`secure_fw\services\initial_attestation`.

### PSA interface

The TF-M Initial Attestation Service exposes the following PSA interface:

 ``` c
 enum psa_attest_err_t
 psa_initial_attest_get_token(const uint8_t *challenge_obj,
                              uint32_t       challenge_size,
                              uint8_t       *token,
                              uint32_t      *token_size);
```

### Service source files

 - `attestation_core.c` : This file implements core functionalities such as
 retrieval of claims and token creation.

### SPM interface
The Secure Partition Manager provides an interface to retrieve data from secure
bootloader which is relevant to runtime services. The attestation token must
contain the measurements of firmware components. These measurements are done by
the bootloader and stored in a shared data region for runtime firmware. Only SPM
has direct access to the shared data region. Services can retrieve relevant data
with the following interface:
  - `int32_t tfm_core_get_boot_data(uint8_t major_type, void *ptr,
     uint32_t len)`

This function must be called during service initialization phase, because the
shared data region is deliberately overlapping with secure main stack to spare
some memory and reuse this area during execution. After calling the function SPM
copies the service relevant data, identified by `major_type`, to service's
memory space.

### Shared data structure
The format of shared data is TLV encoded. The definition of TLV structure is
described in `bl2\include\tfm_boot_status.h`.

Structure of shared data:
```
---------------------------------------------------------------
| Magic number(uint16_t) | Shared data total length(uint16_t) |
---------------------------------------------------------------
| Major_type(uint8_t) | Minor_type(uint8_t) | Length(uit16_t) |
---------------------------------------------------------------
|                         Raw data                            |
---------------------------------------------------------------
|                              .                              |
|                              .                              |
|                              .                              |
---------------------------------------------------------------
| Major_type(uint8_t) | Minor_type(uint8_t) | Length(uit16_t) |
---------------------------------------------------------------
|                         Raw data                            |
---------------------------------------------------------------
```

Structure of TLV entry's header:
  - **Major_type** - Identifies the secure service which the data is sent to.
  - **Minor_type** - Identifies the type of the data.
  - **Length** - The length of one TLV entry including the header.

## Initial Attestation Service integration guide
In order to interact with Initial Attestation Service and retrieve attestation
token, the described PSA interface in `psa_initial_attestation.h` must be
called. It can be called from secure and non-secure context as well.

The caller must allocate a big enough buffer, where the token is going to be
created by Initial Attestation Service. Detailed description of the interface
can be found in the above mentioned header file.

### Measured boot
It **must** have a secure bootloader in the boot chain, which is capable to
measure the runtime firmware components (calculates hash value of theirs).
The data must be stored in the above described TLV format in a shared data
region in secure internal RAM. The address and format of shared data region is
a contract between the bootloader and the secure firmware.

 --------------

*Copyright (c) 2018, Arm Limited. All rights reserved.*
