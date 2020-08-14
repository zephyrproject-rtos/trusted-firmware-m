######################
'demo'-directory guide
######################

************
Introduction
************

This directory contains some example usages of TF-Fuzz, in a form that makes it
easy to run each case and quickly see the results.

An example way to use this directory:

- Add ``./`` to your ``$PATH`` variable, for convenience (see below).
- Using any text editor that provides for tabbed editing (e.g., Nedit) pop
  up all files in this directory, each in a separate tab.
- In a shell window, run each demo test, and view a file called
  ``tossThis2`` (that name to suggest we delete that file after we're done)
  to view the generated C code.

The ``purpose`` line in each test describes what each demo illustrates.

Bear in mind that these are not intended as practical use cases, but just for
illustration.  In the first few cases, the TF-Fuzz tool is not very "smart";
that is, TF-Fuzz only provides a very-compact test-specification format.  As
the test number increases, the TF-Fuzz tool infers for you more about test.

*********************************************
``.../tools/tf_fuzz/demo`` directory contents
*********************************************
.. code-block:: bash

    1  10  11  12  13  14  15  16  17  18  19  2  3  4  5  6  7  8  9  r

The numbered files are test-template files -- input to TF-Fuzz -- for
demonstrating what it can do.

The ``r`` shell script is designed to make it quick and easy to run the demo
tests one by one:  just type ``r 5`` for example.  The very short script name
``r`` and the very-short, numbered demo-test file names were chosen to make it
easy to quickly see example usages.  To see sequentially more-involved usages
of the tool, go through these demos in order.

--------------

*Copyright (c) 2020, Arm Limited. All rights reserved.*
