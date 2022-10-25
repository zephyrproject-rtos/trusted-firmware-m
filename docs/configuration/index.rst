.. _tf-m_configuration:

#############
Configuration
#############

.. Warning::

    This is a temporay design document for review.
    Once the design is settled down.
    It will be converted to user/integration guides.

.. toctree::
    :maxdepth: 1
    :glob:

    build_configuration.rst
    Component configuration <header_file_system>
    Kconfig <kconfig_system>
    Profiles <profiles/index>
    test_configuration.rst

TF-M is highly configurable project with many configuration options to meet
a user needs. A user can select the desired set of services and fine-tune
them to their requirements. There are two types of configuration options

Building configuration
  Specifies which file or component to include into compilation and build.
  These are options, usually used by a build system to enable/disable
  modules, specify location of external dependency or other selection, global to
  a project. Please check the corresponded section :ref:`tfm_cmake_configuration`

Component tuning
   To adjust a particular parameter to a desired value. Those options are
   local to a component or externally referenced when components are coupled.
   Options are in C header file. The :ref:`Header_configuration` has more
   details about it.

.. Note::
   Originally, TF-M used CMake variables for both building and component tuning
   purposes. It was convenient to have a single system for both building and
   component's configurations. To simplify and improve configurability and
   better support build systems other than a CMake, TF-M introduced a
   :ref:`Header_configuration` and moved component options into a dedicated
   config headers.

****************
How to configure
****************

TF-M Project provides a base build, defined in config_base.cmake. It includes
SPM and platform code only. Starting from the base, users can enable required
services and features using several independent methods to configure TF-M.

Use :ref:`tf-m_profiles`.
   There are 4 sets of predefined configurations for a elected
   use cases, called profiles. A user can select a profile by providing
   -DTFM_PROFILE=<profile file name>.
   Each profiles represented by a pair of configuration files for
   Building (CMake) options and Component options (.h file)

Use a custom profile.
   Another method is to take existing TF-M profile and adjust the desired
   options manually editing CMake and config header files. This is for users
   familiar with TF-M.

Use :ref:`Kconfig_system`.
   This method is recommended for beginners. Starting from the
   *base configuration* a user can enable necessary services and options.
   KConfig ensurers that all selected options are consistent and valid.
   This is new in v1.7.0 and it covers only SPM and PSA services. As an output
   KConfig produces a pair of configuration files, similar to a profile.

.. Note::
   In contrast, before TF-M v1.7.0, the default build includes all possible
   features. With growing functionality, such rich default build became
   unpractical by not fitting into every platform and confusing of big
   memory requirements.

**********
Priorities
**********

A project configueration performed in multiple steps with priorities.
The list below explains the process but for the details specific to
:ref:`tfm_cmake_configuration` or :ref:`Header_configuration` please
check the corresponded document.

#. The base configuration with default values is used as a starting point
#. A profile options applied on top of the base
#. A platform can check the selected configuration and apply restrictions
#. Finally, command line options can modify the composed set

.. Note::
   To ensure a clear intention and conscious choice, all options must be
   providede explicitly via a project configuration file. Default values
   on step 1 will generate warnings which expected to break a build.

--------------

*Copyright (c) 2022, Arm Limited. All rights reserved.*