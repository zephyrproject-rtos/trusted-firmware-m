##############
Issue tracking
##############

To trace TF-M issues and to maintain LTS versions transparently, all issues
found after an official release and breaking TF-M functionality, i.e. bugs in
either common code or platform code, or security vulnerability post public
disclosure shall be registered in TF-M Github `Issue tracker`_ .
The identified issues shall be addressed in the **main** development branch to
ensure normal progress. Additionally, these fixes will be backported to the
active Long-Term Support (LTS) branches and will be included in the upcoming
LTS release.

.. warning:: Security-related issues should be addressed through the
    :doc:`Security Disclosure </security/index>` and recorded in the `Issue tracker`_
    **only** after public disclosure.

When reporting a new issue please cover:

 1. **Summary:**
    Provide a concise overview of the issue.
    What problem are you encountering?
 2. **Technical Description:**
    Explain the issue thoroughly. Include relevant logs or screenshots.
 3. **Build and Execution Environment:**
       - The hardware platform
       - Build toolchain with versions
 4. **Reproduction Steps:**
    Describe how to reproduce the issue step by step.
    If possible, provide sample code or configuration settings.
 5. **Proposed Fix (Optional):**
    If you have ideas on how to address the issue, share them.
    It’s not mandatory, but it can be helpful.

Is it a bug or security vulnerability?
--------------------------------------

A security vulnerability refers to a flaw that an attacker can exploit to gain
unauthorized access to system secrets, manipulate data, or perform actions
beyond the intended functionality. However, it’s important to note that defects
that cause system crashes or lead to a Denial of Service (DoS) state are
considered bugs rather than security vulnerabilities.
When faced with uncertainty in classifying a new defect, it is wise to use
caution and consider it as a potential safety issue.

As implied in the :doc:`/contributing/contributing_process`
maintainers reserve the right to decide on what's acceptable to be backported
to LTS branches in case of any divergence.

.. _Issue tracker: https://github.com/TrustedFirmware-M/trusted-firmware-m/issues

--------------

*Copyright (c) 2024, Arm Limited. All rights reserved.*
