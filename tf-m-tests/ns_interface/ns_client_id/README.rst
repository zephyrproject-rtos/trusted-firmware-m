##################################
TF-M test NS Client Identification
##################################

TF-M test implements NS client identification to support multiple NS clients
test, such as in Protected Storage (PS) service test.
This implementation is based on the design of non-secure client management [1]_.

NS test cases calls the ``tfm_nspm_register_client_id()`` to register the client
ID of the currently running NS client.

.. code-block:: c

    uint32_t tfm_nspm_register_client_id(void);

``tfm_nspm_register_client_id()`` fetches the client ID and eventually invokes
``tfm_register_client_id()`` to register the ID from an SVC handler.

.. code-block:: c

    enum tfm_status_e tfm_register_client_id(int32_t ns_client_id);

See ``tfm_api.h`` [2]_ for the details of ``tfm_register_client_id()``.
``tfm_api.h`` will be exported by TF-M during TF-M secure firmware build.

The SVC handler which calls ``tfm_register_client_id()`` is implemented via the
following macro

.. code-block:: c

    SVC(SVC_TFM_NSPM_REGISTER_CLIENT_ID);

The SVC call handler of the above SVC maps the name of the current thread to a
hard-coded client ID, and sends it to the TF-M core via the earlier discussed
API.
The mapping is implemented in ``ns_interface/ns_client_id/tfm_nspm_api.c``.

The system integrators **may** implement the non-secure ID mapping based on
their application/threat model.

*********
Reference
*********

.. [1] `Non-secure Client Management <https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git/tree/docs/design_documents/tfm_non_secure_client_management.rst>`_

.. [2] `tfm_api.h <https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git/tree/interface/include/tfm_api.h>`_

--------------------

*Copyright (c) 2018-2021, Arm Limited. All rights reserved.*
