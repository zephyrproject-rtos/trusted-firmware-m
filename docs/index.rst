:Page authors: Gyorgy Szing <gyorgy.szing@arm.com>

..
   The build-system will copy all documents into a temporary directory tree
   before the documentation is built.
   This fill will be copied to the top level and thus please use relative paths
   as if this file would be in <TFM_ROOT_DIR>.

Welcome to TF-M's documentation!
================================

.. toctree::
    :maxdepth: 2
    :glob:
    :hidden:

    contributing
    docs/dco
    glossary
    maintainers
    docs/user_guides/tfm_sw_requirement
    docs/user_guides/tfm_build_instruction
    docs/coding_guide
    docs/user_guides/tfm_user_guide
    docs/user_guides/os_migration_guide_armv8m
    docs/user_guides/tfm_integration_guide
    docs/user_guides/tfm_ns_client_identification
    docs/user_guides/tfm_secure_boot

.. toctree::
    :maxdepth: 2
    :caption: Secure services
    :glob:
    :hidden:

    docs/user_guides/services/*

.. toctree::
    :maxdepth: 2
    :caption: Components
    :glob:
    :hidden:

    lib/**

.. toctree::
    :maxdepth: 2
    :caption: Target platforms
    :glob:
    :hidden:

    platform/**

.. toctree::
    :caption: Design documents
    :maxdepth: 2
    :glob:
    :hidden:

    docs/design_documents/*

.. toctree::
    :caption: Draft design documents
    :maxdepth: 2
    :glob:
    :hidden:

    docs/design_documents/drafts/*

.. toctree::
    :caption: Rejected design documents
    :maxdepth: 2
    :glob:
    :hidden:

    docs/design_documents/rejected/*

.. include:: readme.rst

-----------

*Copyright (c) 2019, Arm Limited. All rights reserved.*
