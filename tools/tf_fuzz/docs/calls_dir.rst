#######################
'calls'-directory guide
#######################

************
Introduction
************

These classes define objects that describe everything needed to generate a PSA
call.  The Parse phase, which parses the test-template file, generates an C++
STL ``vector`` of ``psa_call``-class objects describing the series of PSA calls
chosen for the test to run, based upon the test template and the random seed.

More-specifically class psa_class is an abstract class, and this vector contains
objects of derived classes, each derived class defining a particular call type.
Each ``psa_call``-subclass object encapsulates the initial and expected data,
expected pass/fail results, and other information about the call.

**********************************************
``.../tools/tf_fuzz/calls`` directory contents
**********************************************
.. code-block:: bash

    crypto_call.cpp  psa_call.cpp  security_call.cpp  sst_call.cpp
    crypto_call.hpp  psa_call.hpp  security_call.hpp  sst_call.hpp

In the future, it's probably possible to, for target systems that can support
sufficiently large memory footprints, instead of writing these calls out to a
``.c`` file, to directly execute these PSA calls.

--------------

*Copyright (c) 2020, Arm Limited. All rights reserved.*
