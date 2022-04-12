# -----------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------

# This module is providing the default parameters for manual Documentation
# building. ( Without relying on CMake to determine the enviroment )
#
# It will however be able to communicate parameters when populated
# by CMake (using the tfm_env.py file), and use a best-effort approach
# to determine them when the interface file is not preset.

import os
import re
import json
from subprocess import check_output
from platform import system

# When called after cmake an evniroment variables file will be present
try:
    from tfm_env import cmake_env
except Exception as E:
    print("ERROR: Configuration Exception:", E)
    cmake_env = None

tfm_def_render_cmake = True
tfm_def_copy_files = True
tfm_def_build_doxygen = True


def find_tfm_root(start_dir=os.path.dirname(os.path.abspath(__file__)),
                  target_files=["license.rst",
                                "dco.txt",
                                "toolchain_GNUARM.cmake"],
                  max_depth=5):
    """ Method which attempts to find the root of the project
    by traversing parent directoried and attempts to located each of the
    files included in target_files list"""

    tfm_root = start_dir

    for i in range(max_depth):
        tfm_root = os.path.dirname(tfm_root)
        if set(target_files).issubset(set(os.listdir(tfm_root))):
            return tfm_root
    return None


def find_package(binary_name):
    """ Attempts to resolve the abolute path for a given application or return
    empty string is nothing is found"""

    sys_det = system()

    if sys_det == "Windows":
        cmd = "where"
        # Window's where requires the extension
        binary_name = binary_name + ".exe"
    elif sys_det in ["Darwin", "Linux"]:
        cmd = "which"
    try:
        return check_output([cmd, binary_name]).decode('UTF-8').strip()
    except Exception as E:
        return ""


def render_cmake_file(config_map, in_file, out_file):
    """ Read an input file containing CMAKE variables and try to
    render them based on a configuration map. Variables not listed
    on the map will be cleared """

    # Read the input file
    with open(in_file, "r", encoding='utf-8') as F:
        _data = F.read()

    # Render all config entires included in the map
    for k, v in config_map.items():
        v = v.replace("\\", "\\\\")
        _data = re.sub(r'@%s@' % k, r'%s' % v, _data)

    # Set all remaining entries to blank
    _data = re.sub(r'@[A-Z\_]+@', "", _data)

    # Create output file
    with open(out_file, "w", encoding='utf-8') as F:
        F.write(_data)


# Default output director for reference_manual. It should not be empty
tfm_def_doxy_output_dir = "reference_manual"


if cmake_env is None:
    # #################### Automatic Defaults ( Standalone )################# #

    # Resolve  ../../
    tfm_def_root_dir = find_tfm_root()

    # Set the copy files directory to whatever will be passed to sphynx-build
    tfm_def_copy_dir = os.path.abspath(os.getcwd())

    # Documentation base path
    tfm_def_doc_root = os.path.join(tfm_def_root_dir, "docs")
    tfm_def_copy_doc_root = os.path.join(tfm_def_copy_dir, "docs")

    tfm_def_doxy_root = os.path.join(tfm_def_doc_root, "doxygen")

    tfm_def_doxy_output_dir = os.path.join(tfm_def_copy_dir,
                                           tfm_def_doxy_output_dir)

    # Input files ( Files containing CMAKE variables )
    tfm_def_conf_in_file = os.path.join(tfm_def_doc_root, "conf.py.in")
    tfm_def_doxygen_in_file = os.path.join(tfm_def_doxy_root, "Doxyfile.in")

    # Attempt to detect plantUML
    _ubuntu_plantum_loc = "/usr/share/plantuml/plantuml.jar"
    if "PLANTUML_JAR_PATH" in os.environ.keys():
        tfm_def_plantum_loc = os.environ["PLANTUML_JAR_PATH"]
    elif os.path.isfile(_ubuntu_plantum_loc):
        tfm_def_plantum_loc = _ubuntu_plantum_loc
    else:
        tfm_def_plantum_loc = ""

    # Attempt to detect the java interpreter location
    tfm_def_java_binary = find_package("java")
    tfm_def_doxygen_loc = find_package("doxygen")
    tfm_def_doxygen_dot_loc = find_package("dot")

    try:
        vrex = re.compile(r'TF-M(?P<GIT_VERSION>v.+?)'
                          r'(-[0-9]+-g)?(?P<GIT_SHA>[a-f0-9]{7,})?$')

        tfm_def_tfm_version = check_output("git describe --tags --always",
                                           shell = True, encoding = 'UTF-8')

        _v = vrex.match(tfm_def_tfm_version)

        tfm_def_tfm_version = _v.group("GIT_VERSION")
        if _v.group("GIT_SHA"):
            tfm_def_tfm_version += "+" + _v.group("GIT_SHA")[:7]

    except Exception as E:
        try:
            tfm_def_tfm_version
        except NameError:
            tfm_def_tfm_version = "Unknown"

else:
    # #################### Cmake Defaults ################################## #
    tfm_def_root_dir = os.path.abspath(cmake_env["TFM_ROOT_DIR"])
    tfm_def_copy_dir = os.path.abspath(cmake_env["SPHINX_TMP_DOC_DIR"])
    tfm_def_plantum_loc = os.path.abspath(cmake_env["PLANTUML_JAR_PATH"])
    tfm_def_java_binary = os.path.abspath(cmake_env["Java_JAVA_EXECUTABLE"])
    tfm_def_tfm_version = cmake_env["SPHINXCFG_TFM_VERSION"]
    tfm_def_conf_in_file = cmake_env["SPHINXCFG_TEMPLATE_FILE"]

    tfm_def_copy_files = True if cmake_env["SPHINXCFG_COPY_FILES"] == "True" \
        else False
    tfm_def_render_cmake = True \
        if cmake_env["SPHINXCFG_RENDER_CONF"] == "True" else False

    tfm_def_build_doxygen = True \
        if cmake_env["DOXYCFG_DOXYGEN_BUILD"] == "True" else False

    if tfm_def_build_doxygen:
        tfm_def_doxy_root = cmake_env["DOXYCFG_DOXYGEN_CFG_DIR"]
        tfm_def_doxygen_in_file = os.path.join(tfm_def_doxy_root,
                                               "Doxyfile.in")

    # Documentation base path
    tfm_def_doc_root = os.path.join(tfm_def_root_dir, "docs")
    tfm_def_copy_doc_root = os.path.join(tfm_def_copy_dir, "docs")

    # Disable copyfiles for next invocation
    cmake_env["SPHINXCFG_COPY_FILES"] = "False"
    with open("tfm_env.py", "w", encoding='utf-8') as F:
        F.write("cmake_env =" + json.dumps(cmake_env))

# #################### User Defaults ######################################## #

# Directories, referenced by TF-M root, which may contain releval documents
# which need to be broughtt over
document_scan_dirs = ["tools", "platform"]

document_scan_ext = [".rst", ".md", ".jpg", ".png"]

# Other documents that should be added to the root documentation folder
documents_extra = ["license.rst", "dco.txt"]

# Output files ( After CMAKE variables have been evaluated )
tfm_def_conf_out_file = os.path.join(tfm_def_copy_dir, "conf_rendered.py")
if tfm_def_build_doxygen:
    tfm_def_doxygen_out_file = os.path.join(tfm_def_doxy_root,
                                            "DoxyfileCfg.in")

# If env is none, the script is running as standalone. Generate it with the
# auto-detected values set above
if cmake_env is None:
    cmake_env = {"TFM_ROOT_DIR": tfm_def_root_dir,
                 "DOXYGEN_EXECUTABLE": tfm_def_doxygen_loc,
                 "DOXYGEN_DOT_EXECUTABLE": tfm_def_doxygen_dot_loc,
                 "PLANTUML_JAR_PATH": tfm_def_plantum_loc,
                 "SPHINXCFG_TFM_VERSION": tfm_def_tfm_version,
                 "Java_JAVA_EXECUTABLE": tfm_def_java_binary,
                 "DOXYCFG_OUTPUT_PATH": tfm_def_doxy_output_dir,
                 "DOXYCFG_TFM_VERSION": tfm_def_tfm_version,
                 }
# Only Override the version
else:
    cmake_env["SPHINXCFG_TFM_VERSION"] = tfm_def_tfm_version
