######################
TF-M Example Partition
######################
The example partition provides a secure SHA-256 hash service that computes the
SHA-256 hash of a given message and returns the hash. In addition, it also
contains a templated example which can be used as a starting point for
developing secure services. The templated example service shows how to
start a platform timer and implement a timer IRQ handler.

The example partition is only enabled when the non-secure application (Zephyr)
enables the config options ``CONFIG_TFM_PARTITION_EXAMPLE``.

***************************
Adding a new secure service
***************************

Adding a new secure service involves three steps:

* Adding the secure service definition in ``tfm_example_partition.yaml``
* Implementing secure service in ``tfm_example_partition.c``
* Provide non-secure (NS) APIs in ``tfm_example_partition_api.c/h`` for IPC
  mode or ``tfm_example_partition_secure_api.c/tfm_example_partition_api.h``
  for library mode

Please refer to `Adding secure partition`_ for more information.

.. _Adding secure partition:
  https://tf-m-user-guide.trustedfirmware.org/docs/integration_guide/services/tfm_secure_partition_addition.html
