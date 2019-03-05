################################
TF-M Core Test integration guide
################################

************
Introduction
************

The core test suites test some of the features of the TF-M core. There are
multiple services that are used by the core test suites. The services are
defined in the ``test/test_services`` folder.

Currently there are two test suites, *interactive* and *positive*. The positive
test suite can be run, by building using ``ConfigCoreTest.cmake`` as cmake
config file. The interactive test suite is only available by manually setting
CORE_TEST_INTERACTIVE to ON in the following block in ``CommonConfig.cmake``:

.. code-block:: cmake

	if (CORE_TEST)
		set(CORE_TEST_POSITIVE ON)
		set(CORE_TEST_INTERACTIVE OFF)
		set(TFM_PARTITION_TEST_SECURE_SERVICES ON)
		add_definitions(-DTFM_PARTITION_TEST_SECURE_SERVICES)
	endif()

After making the change, the tests can be run by building using
``ConfigCoreTest.cmake`` as cmake config file.

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
without actually executing the action expected by the test.

******************
IRQ handling tests
******************

The IRQ handling test currently tests the following scenarios:

- NS code execution is interrupted by a secure IRQ (``IRQ_TEST_SCENARIO_1``)
- S code execution is interrupted by a secure IRQ, The handler is not the
  interrupted service (``IRQ_TEST_SCENARIO_2``)
- S code execution is interrupted by a secure IRQ, The handler is the
  interrupted service (``IRQ_TEST_SCENARIO_3``)
- S code waits for an interrupt (calling ``psa_wait()``), the handler is in
  the service that is waiting, ``psa_eoi()`` is called after ``psa_wait()`` returns
  (``IRQ_TEST_SCENARIO_4``)

The following test services participate in the test execution:

- ``TFM_IRQ_TEST_1`` has the role of the interrupted partition with the IRQ
  handler
- ``TFM_SP_CORE_TEST_2`` has the role of the interrupted partition without the
  IRQ handler

All the test executions are initiated from the NS positive test suite. For each
scenario the non-secure testcase calls the following secure functions in order:

1. prepare_test_scenario for ``TFM_IRQ_TEST_1``
2. prepare_test_scenario for ``TFM_SP_CORE_TEST_2``
3. execute_test_scenario for ``TFM_IRQ_TEST_1``
4. execute_test_scenario for ``TFM_SP_CORE_TEST_2``

During these steps, the ``TFM_IRQ_TEST_1`` sets up a timer with a convenient
init value, and depending on the scenario, one of the services, or the NS code
enters a busy wait waiting for the timer interrupt to be raised. In case of
``IRQ_TEST_SCENARIO_3``, when ``PSA API`` is used, the execute_test_scenario
request of the NS code is only replied when the IRQ is handled, so no explicit
busy wait is required. In all the other cases, handling of the irq is signalled
to the waiting party by setting a variable in a non-secure memory location.

--------------

*Copyright (c) 2019, Arm Limited. All rights reserved.*