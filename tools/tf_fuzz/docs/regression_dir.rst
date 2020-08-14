############################
'regression'-directory guide
############################

************
Introduction
************

This is a regression suite for the TF-Fuzz tool.  That is, tests to make sure
that TF-Fuzz is still functioning properly after making changes.  Note that
this regression implementation tests the most basic aspects of TF-Fuzz's
implementation, but is not yet complete.  Most notably, it does not yet test
``*active`` and ``*deleted``, nor ``shuffle`` and ``2 to 5 of {...}``
constructs.

***************************************************
``.../tools/tf_fuzz/regression`` directory contents
***************************************************
.. code-block:: bash

    000001_set_sst_uid_data_expect_pass
    000002_set_sst_name_data_expect_nothing
    000003_set_sst_name_data
    000004_set_sst_name_rand_data
    000005_set_sst_rand_name_rand_data
    000006_set_sst_multi_name_rand_data
    000007_set_sst_multi_uid_rand_data
    000008_set_sst_name_rand_data_read_check_wrong
    000009_set_sst_name_rand_data_read_check_var_read_print
    000010_read_nonexistent_sst_check_string
    000011_read_nonexistent_sst_check_string_expect_pass
    000012_read_nonexistent_sst_check_string_expect_other
    000013_set_sst_name_rand_data_remove_twice
    000014_set_sst_name_rand_data_remove_other
    000015_set_sst_name_only
    000016_set_sst_single_asset_set_multiple_times
    000017_read_sst_check_single_asset_multiple_times
    000018_000016_and_000017
    000019_read_asset_to_variable_set_other_asset
    add_these_tests
    function2OpenFiles
    README
    regress
    regress_lib

******************************
Files for Each Regression Test
******************************

Here's the overall regression scheme:

- ``bash regress`` from this directory runs regression.  It will fail with an
  error if a problem is found.  If it runs to completion, then regression has
  passed.

- Each test is in its own sub-directory containing these files, by name (always
  same name):

  - ``template``:  The test-template file to be run though the TF-Fuzz under
    test, called "the DUT TF-Fuzz" here.

  - ``exp_stdout_stderr``:  The *expected*, combined ``stdout`` and ``stderr``
    from running TF-Fuzz in verbose mode ``-v``.  This file contains wildcard
    expressions to be checked (more on that below).

  - ``exp_test.c``:  The *expected* output C code.  This file also contains
    wildcard expressions to be resolved against the DUT TF-Fuzz output (again,
    more on that below).

  - ``stdout_stderr`` (if present):  The *actual* combined ``stdout`` and
    ``stderr`` from running the DUT TF-Fuzz in verbose mode ``-v``, during
    regression testing.

  - ``test.c`` (if present):  The output C code generated from running the DUT
    TF-Fuzz in verbose mode ``-v``, during regression testing.

  - ``check.py``:  This Python 3 script compares expected to actual
    ``stdout``/``stderr`` and C-test code, resolving wildcard references in
    ``exp_stdout_stderr`` and ``exp_test.c``.  Each test directory has its own
    script customized to the needs of that particular test, but they mostly
    just runs TF-Fuzz, opens files, then invokes functions in the
    ``regress_lib`` directory, which do the majority of the actual work.

********************************
How ``check.py`` Assesses a Test
********************************

To illustrate how ``check.py`` checks a regression test, below is a ``diff`` of
``test.c`` and ``exp_test.c`` file files, from
``./000005_set_sst_rand_name_rand_data/``, at the time of writing this:

.. code-block:: bash

    47,48c47,48
    <     static uint8_t koxjis_data[] = "Gaa wuqnoe xoq uhoz qof er uaycuuf?";
    <     static int koxjis_data_size = 35;
    ---
    >     static uint8_t @@@003@@@_data[] = "@@002@10@@[a-z\ ]*[\.\?\!]";
    >     static int @@@003@@@_data_size = \d+;
    53,55c53,55
    <     /* Creating SST asset "koxjis," with data "Gaa wuqnoe...". */
    <     sst_status = psa_ps_set(2110, koxjis_data_size, koxjis_data,
    <                             PSA_STORAGE_FLAG_NONE);
    ---
    >     /* Creating SST asset "@@@003@@@," with data "@@002@10@@...". */
    >     sst_status = psa_ps_set(@@@001@@@, @@@003@@@_data_size, @@@003@@@_data,
    >                             PSA_STORAGE_FLAG_[A-Z_]+);
    63c63
    <     psa_ps_remove(2110);
    ---
    >     psa_ps_remove(@@@001@@@);

``check.py``, short summary, performs a Python ``re.match()`` line-by-line the
generated ``test.c`` against the ``exp_test.c`` file.  However, ``exp_test.c``,
in addition to Python regular expressions, also contains "special" wildcards,
described below.

*********
Wildcards
*********

The wildcards in the ``exp_stdout_stderr`` and ``exp_test.c`` files are of
three basic natures, using the examples shown above (please reference them
above to clearly understand the ideas here):

.. list-table::
   :widths: 20 80

   * - ``[a-z\ ]*[\.\?\!]`` or ``[A-Z_]+``
     - | These are Python regex pattern matches for what characters are expected
       | at those places.  The data consist of quasi-sentences, capitalized at
       | the beginning.  The capitalized character is covered by the
       | ``@@002@10@@`` (see below) before it.  The ``[a-z\ ]*[\.\?\!]`` is a
       | Python-regex match for all remaining characters of the sentence:  A
       | sequence of zero or more lower-case letters or blanks followed by
       | sentence-ending punctuation.

   * - ``@@@001@@@`` (``@@@``, a pattern number, ``@@@``)
     - | This denotes a particular pattern of characters, until the expected and
       | actual character streams re-sync again.  The important thing, however,
       | is that what this wildcard stands for *must be consistent* throughout
       | the comparison!  In this case above, ``@@@001@@@`` in the ``exp_test.c``
       | must consistently match ``8617`` everywhere throughout the ``test.c``
       | file.  Of course, the ``8617`` is different for different random-seed
       | values.  The number between the two ``@@@`` occurrences in the wildcard
       | designates which pattern must consistently match.

   * - ``@@002@10@@`` (``@@``, a pattern number, ``@``, a pattern size, ``@@``)
     - | This is a slight variant upon the previous wildcard, in which a specific
       | match length is required.  In lines 47 and 48 above, random data
       | generated consists of 10 characters (thus the ... ``@10@@`` in the
       | wildcard) ``Gaa wuqnoe`` followed by other characters we don't care
       | about;  they can be anything.  Thus ``@@002@10@@[a-z\ ]*[\.\?\!]`` in
       | line 47:  The ``@@002@10@@`` denotes a pattern number 002 for a length
       | of 10 characters that must match ``Gaa wuqnoe`` in this case, followed
       | by some arbitrary number of characters we don't care about, thus
       | ``[a-z\ ]*[\.\?\!]`` -- a sequence of lower-case letters or spaces,
       | capped off with normal sentence-ending punctuation.

After the ``check.py`` capability -- resolving these wildcards -- for this
purpose is fleshed out, we shall have to figure out how to address
``shuffle {}`` and ``5 to 8 of {}`` randomizations.

The ``add_these_tests`` directory contains regression tests of the above nature
that the regression framework is not currently able to address.

--------------

*Copyright (c) 2020, Arm Limited. All rights reserved.*
