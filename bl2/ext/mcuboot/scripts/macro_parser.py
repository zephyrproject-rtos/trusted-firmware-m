#! /usr/bin/env python3
#
# -----------------------------------------------------------------------------
# Copyright (c) 2019-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
# -----------------------------------------------------------------------------


import re
import os

# Opens a file that contains the macro of interest, then finds the macro with
# a regular expression, parses the expression that is defined for the given
# macro. Lastly it evaluates the expression with the eval function
def evaluate_macro(file, regexp, matchGroupKey, matchGroupData, bracketless=False):
    regexp_compiled = re.compile(regexp)

    if os.path.isabs(file):
        configFile = file
    else:
        scriptsDir = os.path.dirname(os.path.abspath(__file__))
        configFile = os.path.join(scriptsDir, file)

    macroValue = {}
    with open(configFile, 'r') as macros_preprocessed_file:
        for line in macros_preprocessed_file:
            if bracketless:
                line=line.replace("(","")
                line=line.replace(")","")
            line=line.replace(",","")
            m = regexp_compiled.match(line)
            if m is not None:
                macroValue[m.group(matchGroupKey)] = \
                eval(m.group(matchGroupData))

    if (matchGroupKey == 0 and not macroValue):
        macroValue["None"] = None

    return list(macroValue.values())[0] if (matchGroupKey == 0) else macroValue
