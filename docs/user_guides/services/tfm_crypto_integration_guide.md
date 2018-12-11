# TF-M Crypto Service Integration Guide

## Introduction

TF-M Crypto service allows application to use cryptography primitives such as
symmetric and asymmetric ciphers, hash, message authentication codes (MACs) and
authenticated encryption with associated data (AEAD).

## Code structure

The PSA interfaces for the Crypto service are located in `interface\include`.
The only header to be included by applications that want to use functions from
the PSA API is `psa_crypto.h`.
The TF-M Crypto service source files are located in `secure_fw\services\crypto`.

### PSA interfaces

The TF-M Crypto service exposes the PSA interfaces detailed in the header
`psa_crypto.h`. There are two additional header files, named
`psa_crypto_extra.h` and `psa_crypto_platform.h`, which are meant to be included
only by the `psa_crypto.h` header itself, that specify, respectively, extensions
to the API that are vendor specific, and provide definitions and types which are
platform specific. For a detailed description of the PSA API interface, please
refer to the comments in the `psa_crypto.h` header itself.

### Service source files

 - `crypto_cipher.c` : This file implements functionalities related to the
 ciphering module;
 - `crypto_hash.c` : This file implements functionalities related to the
 hashing module;
 - `crypto_init.c` :  This file provides basic functions to initialise the
 secure service during TF-M boot;
 - `crypto_key.c` : This file implements functionalities related to the key
 management module. The `TFM_CRYPTO_KEY_STORAGE_NUM` determines how many key
 stores are available, while the `TFM_CRYPTO_MAX_KEY_LENGTH` defines the
 maximum allowed key length in bytes supported in a key storage. These two
 items can be modfied at the build configuration step by defining the
 following variables, `-DCRYPTO_KEY_STORAGE_NUM=<value>` and the
 `-DCRYPTO_KEY_MAX_KEY_LENGTH=<value>`;
 - `crypto_alloc.c` : This file implements extensions to the PSA interface
 which are specifically required by the TF-M Crypto service, in particular
 related to the allocation and deallocation of crypto operation contexts in
 the secure world. The `TFM_CRYPTO_CONC_OPER_NUM`, defined in this file,
 determines how many concurrent contexts are supported (8 for the current
 implementation). For multipart cipher/hash/MAC operations, a context is
 associated to the handle provided during the setup phase, and is explicitly
 cleared only following a successful termination or an abort;
 - `crypto_wrappers.c` : This file implements TF-M compatible wrappers in
 case they are needed by the functions exported by other modules;
 - `crypto_utils.c` : This file implements utility functions that can be
 used by other modules of the TF-M Crypto service;
 - `crypto_engine.c` : This file implements the layer which the other modules
 use to interact with the cryptography primitives available (in SW or HW). The
 `TFM_CRYPTO_ENGINE_BUF_SIZE` determines the size in bytes of the static scratch
 buffer used by this layer for its internal allocations. This item can be
 modified at the build configuration step by defining
 `-DCRYPTO_ENGINE_BUF_SIZE=<value>`. The current implementation provides only SW
 primitives based on Mbed TLS functions;
 - `crypto_mac.c` : This file implements functionalities related to the
 MAC (Message Authentication Code) module;
 - `crypto_aead.c` : This file implements functionalities related to the AEAD
 (Authenticated Encryption with Associated Data) module.

## Crypto service integration guide

In this section, a brief description of the required flow of operation for the
functionalities exported by the PSA Crypto interface is given, with particular
focus on the TF-M Crypto service specific operations. For the details of the
generic PSA Crypto interface operations, please refer directly to the header
`psa_crypto.h`.

Most of the PSA Crypto APIs require an operation context to be allocated by the
application and then to be passed as a pointer during the following API calls.
These operation contexts are of four main types describes below:

 - `psa_key_policy_t` - Operation context to be used when setting key policies;
 - `psa_hash_operation_t` - Operation context for multipart hash operations;
 - `psa_mac_operation_t` - Operation context for multipart MAC operations;
 - `psa_cipher_operation_t` - Operation context for multipart cipher operations.

The user applications are not allowed to make any assumption about the original
types behind these typedefs, which are defined inside `psa_crypto.h`.
In the scope of the TF-M Crypto service, these types are regarded as
handles to the corresponding implementation defined structures which are stored
in the Secure world.

 --------------

*Copyright (c) 2018-2019, Arm Limited. All rights reserved.*
