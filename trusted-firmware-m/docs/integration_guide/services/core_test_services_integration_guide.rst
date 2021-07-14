###########################
Core Test integration guide
###########################

************
Introduction
************

The core test suites test some of the features of the TF-M core. There are
multiple services that are used by the core test suites. The services are
defined in the ``test/test_services`` folder.

Currently there are two test suites, *interactive* and *positive*. The positive
test suite can be run by building using non-secure regression tests using
``-DTEST_NS``.

The interactive test suite is only available by setting
``-DTFM_INTERACTIVE_TEST=ON`` while the non-secure regression testsuite is
enabled.

In order to enable the peripheral access test module of the positive testsuite,
the non-secure regression testsuite must be enabled and
``-DTFM_PERIPH_ACCESS_TEST=ON`` must be set.

**************************
Platform specific features
**************************
For certain test cases the core test services rely on platform functionality.
The required features are:

- Access to LEDs or registers that can be used as scratch registers for
  read/write access tests
- Get a button state that can be pressed by the user to simulate a secure
  service with an arbitrarily long blocking execution.
- Access to a timer that is able to interrupt the core running TF-M.

The functionality that have to be implemented by the platform is described in
``platform/include/tfm_plat_test.h``. For details see the documentation of the
functions.

It is the responsibility of the platform implementation to ensure that the
resources needed for the core test services are properly linked to the service.
This can be achieved by using the
``TFM_LINK_SET_<memory_type>_IN_PARTITION_SECTION(...)`` macros in
``platform/include/tfm_plat_defs.h``. More details regarding the usage of these
macros are available in the header file.

It is possible that a platform implementation mocks the implementation of some
or all of the functions, by returning the values expected by the test cases,
without actually executing the action expected by the test. A platform can also
set the corresponding control flag to ``OFF`` to skip a test case. For example,
A platform can skip peripheral access test case by setting
``TFM_PERIPH_ACCESS_TEST`` to ``OFF`` in its cmake configuration file.

--------------

*Copyright (c) 2019-2021, Arm Limited. All rights reserved.*
