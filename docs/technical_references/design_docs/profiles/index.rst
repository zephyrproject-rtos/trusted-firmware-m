TF-M Profiles
=============

The capabilities and resources may dramatically vary on different IoT devices.
Some IoT devices may have very limited memory resource. The program on those devices should keep
small memory footprint and basic functionalities.
On the other hand, some devices may consist of more memory and extended storage, to support stronger
software capabilities.

Diverse IoT use cases also require different levels of security and requirements on device resource.
For example, use cases require different cipher capabilities. Selecting cipher suites can be
sensitive to memory footprint on devices with constrained resource.

Trusted Firmware-M (TF-M) defines several general profiles, such as Profile Small, Profile Medium,
Profile Medium ARoT-less and Profile Large, to provide different levels of security to fit diverse
device capabilities and use cases.
Each profile specifies a predefined list of features, targeting typical use cases with specific
hardware constraints. Profiles can serve as reference designs, based on which developers can
continue further development and configurations, according to use case.

TF-M Profiles align with Platform Security Architecture specifications and certification
guidelines. It can help vendors to simplify security configuring for PSA certification.

TF-M Profiles details are discussed in the links below.

.. toctree::
    :maxdepth: 1
    :glob:

    *

--------------

*Copyright (c) 2020, Arm Limited. All rights reserved.*
