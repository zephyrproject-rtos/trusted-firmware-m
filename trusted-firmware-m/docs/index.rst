:Page authors: Gyorgy Szing <gyorgy.szing@arm.com>

..
   The build-system will copy all documents into a temporary directory tree
   before the documentation is built.
   This file will be copied to the top level and thus please use relative paths
   as if this file would be in <TFM_ROOT_DIR>.

   The values between @ characters will be filled in by CMake.

Trusted Firmware-M Documentation
================================

.. raw:: html

    <ul class="grid">
        <li class="grid-item">
            <a href="docs/introduction/readme.html">
                <img alt="" src="_static/images/tfm-introduction.png"/>
                <h2>Introduction</h2>
            </a>
            <p>Introducing the Trusted Firmware-M Project: overview,
               architecture, features, and licensing.</p>
        </li>
        <li class="grid-item">
            <a href="docs/getting_started/index.html">
               <img alt="" src="_static/images/tfm-documentation.png"/>
               <h2>Getting Started</h2>
            </a>
            <p>Follow this guide to set up a development environment on your
               system, and then build and run a sample application.</p>
        </li>
        <li class="grid-item">
            <a href="platform/ext/index.html">
                <img alt="" src="_static/images/tfm-platform.png"/>
                <h2>Supported Platforms</h2>
            </a>
            <p>List of supported boards and platforms.</p>
        </li>
        <li class="grid-item">
            <a href="docs/contributing/index.html">
                <img alt="" src="_static/images/tfm-contribution.png"/>
                <h2>Contribution</h2>
            </a>
            <p>As an open-source project, we welcome and encourage the community
               to submit patches directly to the project.</p>
        </li>
        <li class="grid-item">
            <a href="docs/integration_guide/index.html">
                <img alt="" src="_static/images/tfm-integration.png"/>
                <h2>Integration</h2>
            </a>
            <p>Guidelines for integration with TF-M.</p>
        </li>
        <li class="grid-item">
            <a href="docs/technical_references/index.html">
                <img alt="" src="_static/images/tfm-reference.png"/>
                <h2>References</h2>
            </a>
            <p>Design documents.</p>
        </li>
        <li class="grid-item">
            <a href="docs/security/index.html">
                <img alt="" src="_static/images/tfm.png"/>
                <h2>Security</h2>
            </a>
            <p>Requirements, processes, and thread models for ensuring security
               is addressed within the TF-M project.</p>
        </li>
        <li class="grid-item">
            <a href="docs/releases/index.html">
                <img alt="" src="_static/images/tfm-release.png"/>
                <h2>Releases</h2>
            </a>
            <p>Release notes.</p>
        </li>
    </ul>

.. toctree::
    :caption: Table of Contents
    :name: mastertoc
    :titlesonly:
    :maxdepth: 1
    :hidden:

    Home<self>
    docs/introduction/index
    docs/getting_started/index
    platform/ext/index
    docs/contributing/index
    docs/integration_guide/index
    docs/technical_references/index
    docs/security/index
    docs/releases/index


.. toctree::
    :maxdepth: 2
    :hidden:
    :caption: Quick Links

    API Reference <https://ci.trustedfirmware.org/view/TF-M/job/tf-m-build-docs-nightly/lastSuccessfulBuild/artifact/trusted-firmware-m/build/docs/reference_manual/html/index.html>
    Security Center <https://developer.trustedfirmware.org/w/collaboration/security_center>
    Platform Security Architecture <https://developer.arm.com/architectures/security-architectures/platform-security-architecture>
    docs/glossary

--------------

*Copyright (c) 2017-2021, Arm Limited. All rights reserved.*
