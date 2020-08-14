#####################
'lib'-directory guide
#####################

************
Introduction
************

This directory contains the customizable "boilerplate" code snippets used to
write out C source code.  The file in this directory currently, is that for
TF-M, but other comparable frameworks could be addressed with equivalent files
taylored for the required syntax of that other framework.

Which file to use for this process is selected by setting two environment
variables pointing to that file:  One for the ``lib`` directory and one for the
file itself within that directory.  In ``bash`` syntax,

.. code-block:: bash

    export TF_FUZZ_LIB_DIR=<path to this TF-M installation>/tools/tf_fuzz/lib
    export TF_FUZZ_BPLATE=tfm_boilerplate.txt


********************************************
``.../tools/tf_fuzz/lib`` directory contents
********************************************
.. code-block:: bash

    tfm_boilerplate.txt

--------------

*Copyright (c) 2020, Arm Limited. All rights reserved.*
