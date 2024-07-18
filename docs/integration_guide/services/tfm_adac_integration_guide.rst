#########################################
ADAC (Authenticated Debug Access Control)
#########################################

************
Introduction
************

Background
==========
In order to make sure debug capabilities of any system do not become attack
vectors, PSA provides reference ADAC system architecture. It specifies
functional layers that reside above actual physical link.

Authenticated Debug Access Control (ADAC), also referred to as Secure Debug, is
a protocol that provides a way to use strong authentication to restrict device
debug access to only authorized entities. Across various life cycle states of
target device, it permits appropriate access to finely configured domains.

Requirements
============
Debug capabilities must achieve several safety goals to be fully resilient.
It requires

1. Strong authentication
2. Finely grained hardware and firmware domains
3. Enforcing debug limitations

ADAC protocol is fully flexible to meet the above goals.  But depending on the
system design and topology, it must be implemented so to meet varying attack
surface.

ADAC Components (Host Side)
===========================

- ``Secure Debug Manager (SDM)``:  SDM is the host side component adac protocol
  implementation. Its API is implemented in below external github repository.

  ``git@github.com:ARM-software/sdm-api.git``.

An example implementation for SDM library is in below external github repository.

  ``git@github.com:ARM-software/secure-debug-manager.git``.

ADAC Components (Target Side)
=============================

- ``Secure Debug Authenticator (SDA)``:  SDA is the target side component that
  implements ADAC protocol. It is implemented in below external repository.
  It authenticates all input credentials and applies specific debug permissions.

  ``https://git.trustedfirmware.org/shared/psa-adac.git``.

SDA integration
===============

The above mentioned SDA repository includes required HAL updates for a few
supported platforms along with transport layer protocol. Hence, the API to
initiate the connection with the host debugger and to perform
the authentication process is platform-specific.

For example for musca b1 platform, the API to integrate is:

- ``tfm_to_psa_adac_musca_b1_secure_debug()``

Once the device is Secured, opening up the debug ports poses a risk to
active confidential production secrets. So on debug request, a system reset is
required, and most of the time, SDA code is expected to reside in the
boot loader (BL2) or equivalent.

However RSE platform has some implemenentation constraints due to which SDA is
integrated in runtime service.
For more details, please refer to the Implementation Constraints section in the
:doc:`ADAC Implementation for RSE <TF-M-Extras:partitions/adac_impl_for_rse>`

Please follow the below link for further information on SDA implementation.

| `psa-adac read me`_

.. _psa-adac read me:
  https://developer.arm.com/documentation/den0101/latest

*********
Reference
*********

| `ADAC specification`_

.. _ADAC specification:
  https://developer.arm.com/documentation/den0101/latest

--------------

*Copyright (c) 2022-2024, Arm Limited. All rights reserved.*
