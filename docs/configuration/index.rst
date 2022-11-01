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

    profiles/index

TF-M is a complex project having many configuration options to adjust project
for a user needs. A user can select the desired set of services and fine-tune
them to user's requirements. There are 2 types of configuration options:

1. Building : to select which file or component to include into compilation.
   These are options, usually used by a build system to enable/disable
   modules, specify location of external dependency or other selection,
   global to a project. These options shall be considered while adopting TF-M
   to other build systems.
   In the Base configuration table theses options have *Build* type.

2. Component tuning : to adjust a particular parameter to a desired value.
   Those options are local to a component or externally referenced when
   components are coupled. Usually, such options are located in C header
   file. The Header File Config System has more details about it.
   In the Base configuration table theses options have *Component* type.

.. Note::
  Originally, TF-M used CMake variables for both building and component tuning
  purposes. It was convenient to have a single system for both building and
  component's configurations. To simplify and improve configurability and
  better support build systems other than a CMake, TF-M introduced a header
  file configuration and moved component options into a dedicated config headers.

****************
How to configure
****************

The default TF-M build includes the minimum set of components required in any
project: SPM and a selected platform. This is not very useful for any product
and desired services shall be enabled by a user. There are several independent
ways to configure TF-M.

1. Use profiles. There are 4 sets of predefined configurations for a elected
   use cases, called profiles. A user can select a profile by providing
   -DTFM_PROFILE=<profile file name>.
   Each profiles represented by a pair of configuration files for
   Building (CMake) options and Component options (.h file)

2. A custom profile. Another method is to take a profile as a base and manually
   modify desired options.

3. Use KConfig system. This is recommended method especially for beginners.
   KConfig ensurers that all selected options are consistent and valid. This
   is new in v1.7.0 and it covers only SPM and PSA ervices. As an output
   KConfig produces a pair of configuration files, similar to a profile.

.. Note::
   In contrast, before TF-M v1.7.0, the default build includes all possible
   features. With growing functionality, such rich default build became
   unpractical by not fitting into every platform and confusing of big
   memory requirements.

**********
Priorities
**********
Note::

   TODO: Need a secion on configuration Priorities


--------------

*Copyright (c) 2022, Arm Limited. All rights reserved.*