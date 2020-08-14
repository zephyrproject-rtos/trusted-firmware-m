#############################
'boilerplate'-directory guide
#############################

************
Introduction
************

This directory contains the C++ header and code files to work with the
customizable, "boilerplate" code snippets in the ``...tools/tf_fuzz/lib``
directory.  Mostly what this code does is read these code snippets into an
STL ``vector`` of, essentially, named strings.

****************************************************
``.../tools/tf_fuzz/boilerplate`` directory contents
****************************************************
.. code-block:: bash

    boilerplate.cpp  boilerplate.hpp

This is currently implemented as a vector of strings, with a parallel vector
of ``const int`` array-index names.  (In the future, it may make sense to
manage the boilerplate code in a C++ STL ``map``-type container.)

--------------

*Copyright (c) 2020, Arm Limited. All rights reserved.*
