#-------------------------------------------------------------------------------
# Copyright (c) 2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

import re
try:
    from enum import Enum
except ImportError as e:
    print e, "To install it, type:"
    print "pip install enum34"
    exit(1)

class Verbosity(Enum):
    debug = 3
    info = 2
    warning = 1
    error = 0

# Set default value
VERBOSITY = Verbosity.info

REkeychain = "@@\w+[\.\w+]*@@"
REfirstkeyword = "@@\w+\.?"
emptychain = "@@@@"

MISSING_KEYS_ACTION = 'halt'

def log_print(level, *args, **kwargs):
    global VERBOSITY
    if kwargs.get("verbosity"):
        VERBOSITY = kwargs["verbosity"]
    if level.value <= VERBOSITY.value:
        string = ' '.join(map(str, args))
        print string

def leaftype(x):
    return isinstance(x, str) or isinstance(x, unicode) or \
        isinstance(x, type(1)) or isinstance(x, type(True))

def substitute(templist, chains, db, depth):
    depth += 1
    log_print(Verbosity.info, "substitute(",templist, chains, db, depth,")")
    if isinstance(db, type([])):
        # db is list
        outlist = []
        for instance in db:
            log_print(Verbosity.info, "Going deeper at", depth, "for db list instance", instance)
            outlist.extend(substitute(templist, chains, instance, depth))
        log_print(Verbosity.info, "substitute", depth, "returning from list with", outlist)
        return outlist

    transientlist = list(templist)
    if leaftype(db):
        # db is leaf
        for chain in chains:
            if templist[chain] == emptychain:
                transientlist[chain] = str(db)
            else:
                print "keychain not empty but db is"
                transientlist[chain] = str(db) + templist[chain]
                continue
        chains = []
        log_print(Verbosity.info, "substitute", depth, "returning from leaf with", transientlist)
        return transientlist

    # db is dict
    # find chain groups with same key
    chaingroups = {"chains": [], "keys": []}
    for chain in chains:
        key = re.search('\w+', templist[chain])
        if not key:
            # chain does not define a leaf
            print "chain", chain, "out of keys before reaching leaf"
            continue
        key = key.group(0) # convert MatchObj to string
        for idx, groupkey in enumerate(chaingroups["keys"]):
            if key == groupkey:
                # insert chain in group
                chaingroups["chains"][idx].append(chain)
                break
        else:
            # key not yet in the list
            chaingroups["keys"].append(key)
            chaingroups["chains"].append([chain])

    log_print(Verbosity.debug, chaingroups)

    for groupidx, key in enumerate(chaingroups["keys"]):
        log_print(Verbosity.info, "key lookup in", db, "for", key)
        if key in db.keys():
            if leaftype(db[key]):
                # db entry value is leaf
                for chain in chaingroups["chains"][groupidx]:
                    transientlist[chain] = str(db[key])
                chaingroups["chains"][groupidx] = []
            else:
                # db node is branch
                for chain in chaingroups["chains"][groupidx]:
                    transientlist[chain] = re.sub(REfirstkeyword, "@@", templist[chain])
        else:
            # key not found in database
            if MISSING_KEYS_ACTION == 'report' or MISSING_KEYS_ACTION == 'halt':
                print "key", key, "not found, invalid chains", chaingroups["chains"][groupidx]
            placeholder = False
            if MISSING_KEYS_ACTION == 'hide':
                placeholder = ""
            if 'replace' in MISSING_KEYS_ACTION:
                placeholder = MISSING_KEYS_ACTION.replace("replace ", "")
                for chain in chaingroups["chains"][groupidx]:
                    transientlist[chain] = placeholder
            chaingroups["chains"][groupidx] = []
            if MISSING_KEYS_ACTION == 'halt':
                exit(1)
    log_print(Verbosity.debug, "**** after selection chaingroups:", chaingroups, transientlist)
    forked = False
    for chidx, chains in enumerate(chaingroups["chains"]):
        if chains:
            key = chaingroups["keys"][chidx]
            log_print(Verbosity.info, "Going deeper at", depth, "for chains", chains, "in dict", db[key])
            if not forked:
                outlist = substitute(transientlist, chains, db[key], depth)
                forked = True
            else:
                transientlist = list(outlist)
                outlist = list()
                for item in transientlist:
                    outlist.extend(substitute(item, chains, db[key], depth))

    if not forked:
        outlist = [transientlist]

    log_print(Verbosity.info, "substitute", depth, "returning from dict/leaf with", outlist)
    return outlist

def keyword_substitute(db, line, missing_keys):
    global MISSING_KEYS_ACTION
    chains = []
    MISSING_KEYS_ACTION = missing_keys
    log_print(Verbosity.info, line)
    templist = re.split("(" + REkeychain + ")", line)
    for idx, item in enumerate(templist):
        if re.match(REkeychain, item):
            chains.append(idx)
            log_print(Verbosity.info, "Keychain:", item)
    outlist = substitute(templist, chains, db, 0)
    outstring = ""
    for outline in outlist:
        outstring += ''.join(outline) + "\n"
    log_print(Verbosity.info, "generator returns with:")
    log_print(Verbosity.info, outstring + "<<")
    return outlist
