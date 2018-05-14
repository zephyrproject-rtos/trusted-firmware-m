# TF-M Audit Logging Service Integration Guide

## Introduction

TF-M Audit logging service allows secure services in the system to log critical
system events and information that have security implications. This is required
to post analyse the system behaviour, system events and triage system issues
offline. This offers a mitigation against the repudiation threat.

The types of information that can be logged are the ID of the entity that
originated a secure service request, or the relevant output or data
associated to the authentication mechanism that the requesting service
has performed on the entity that originated the request. The possible types of
information that can be logged can be easily extended to accomodate various
requirements from other secure services.

## Current service limitations

**Policy manager** - Currently, there is no policy manager implemented, which
means that there are no restrictions on the entities which can add or remove
items from the log. Also, the item replacement in the log is just replacing
older elements first.

**Encryption** - Support for encryption and authentication is not available yet.

**Permanent storage** - Currently the Audit Logging service supports only a RAM
based storage of the log, permanent storage is not supported yet.

## Code structure

Audit logging service code is located in `secure_fw/services/audit_logging/` and
is divided as follows:

 - Core files

The TF-M public interfaces for Audit logging service are located in
`interface/include`.

### TF-M public interfaces

The Audit logging service exposes the following public interfaces:

 - `enum tfm_log_err tfm_log_retrieve(uint32_t size, int32_t start, uint8_t *buffer, struct tfm_log_info *info)`
 - `enum tfm_log_err tfm_log_get_info(struct tfm_log_info *info)`
 - `enum tfm_log_err tfm_log_delete_items(uint32_t num_items, uint32_t *rem_items)`

### TF-M secure-only interface

In addition to the public interface of the previous section, the Audit logging
service exposes the following secure-only interface:

 - `enum tfm_log_err tfm_log_veneer_add_line(struct tfm_log_line *line)`

### Core files

 `log_core.c` - This file implements the core logic for the audit logging
 service, which includes the log management, item replacement, log retrieval and
 log item addition.

## Audit logging service integration guide

 In this section, a brief description of each field of a log entry is given,
 with an example on how to perform a logging request from a requesting service.
 The secure service that requests the addition of an item to the log, has to
 provide the the following data type as defined in
`interface\include\tfm_log_defs.h`:

 ```
 /*!
 * \struct tfm_log_line
 *
 * \brief The part of the log line which has to be
 *        provided by the secure service that wants
 *        to add information to the log
 */
struct tfm_log_line {
    uint32_t size;        /*!< Size in bytes of the three following fields */
    uint32_t function_id; /*!< ID of the function requested */
    uint32_t arg[4];      /*!< [r0,r1,r2,r3] arguments to the function */
    uint8_t  payload[];   /*!< Flexible array member for payload */
};
```

Each field is described as follows:

- `size` - This is the size, in bytes, of the `function_id`, `arg[4]` and
`payload[]` fields that follow. Given that the `payload[]` field is optional,
in the current implementation the minimum value to be provided in `size` is 20
bytes.
- `function_id` - This field is meant to be used to store an ID of the function
that is performing the logging request.
- `arg[4]` - This array is meant to be used to store the content of the
register arguments passed to the function performing the logging request.
- `payload[]` - The payload is an optional content which can be made of one or
more Type-Length-Value entries as described by the following type:

 ```
 /*!
  * \struct log_tlv_entry
  *
  * \brief TLV entry structure with a flexible
  *        array member
  */
 struct log_tlv_entry {
     enum log_tlv_type type;
     uint32_t length;
     uint8_t value[];
 };
 ```
The possible TLV types described by `enum log_tlv_type` can be extended as
needed by system integrators modifying `log_core.h` as needed to guarantee the
desired flexibility.

 --------------

*Copyright (c) 2018, Arm Limited. All rights reserved.*
