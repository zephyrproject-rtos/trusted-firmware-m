##############################
'visualStudio'-directory guide
##############################

************
Introduction
************

This is just things to support running and debug under Microsoft Visual Studio
(Visual Studio is a trademark of Microsoft Corporation).  So far, it contains
a "stand-in" for the ``unistd.h`` library, and more specifically, the
``isatty()`` call in that library, which is used by Lex and YACC.  Other
functions may be added later as needed.  At some time in the future, an
up-coming "interactive mode" for interactively creating test templates may
need an actual working ``isatty()``, but that's not ready yet.

*************************************************
.../tools/tf_fuzz/visualStudio directory contents
*************************************************
.. code-block:: bash

    unistd.c  unistd.h

--------------

*Copyright (c) 2020, Arm Limited. All rights reserved.*
