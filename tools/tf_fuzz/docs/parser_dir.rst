########################
'parser'-directory guide
########################

************
Introduction
************

Users' test-template files are parsed using a Lex/YACC grammar, to make the
test-template language easily extensible.  Much of the actual work is done in
helper functions, but the Lex/YACC grammar manages the process.

***********************************************
``.../tools/tf_fuzz/parser`` directory contents
***********************************************
.. code-block:: bash

    tf_fuzz_grammar.l  tf_fuzz_grammar.y

This directory contains the Lex and YACC grammars for parsing the TF-Fuzz
command "language," if it can be called that.  The
``tf_fuzz_grammar.tab.cpp/.hpp`` files generated also form the executive for
the entire parsing process.

--------------

*Copyright (c) 2020, Arm Limited. All rights reserved.*
