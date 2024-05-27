#-------------------------------------------------------------------------------
# Copyright (c) 2021-2024, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

import argparse
import networkx as nx

import sys
import os
sys.path.append(os.path.join(sys.path[0],'..'))
from provisioning_common_utils import *

def load_graph(filename):
    assert(filename[-4:] == ".tgf")

    with open(filename, 'rt') as graph_file:
        lines = graph_file.readlines()

    graph = nx.DiGraph()

    for line in lines:
        line = line.rstrip()
        if line == '':
            continue

        # First see if it is a valid line
        try:
            edge = line.split(" ", 2)
            edge[0] = int(edge[0])
            edge[1] = int(edge[1])

            assert("Send" in edge[2])
            assert("Receive" in edge[2])
            edge[2] = edge[2].replace("Send ", "").rstrip()
            edge[2] = edge[2].replace("Receive ", "").rstrip()

            send, receive = [int(x) for x in edge[2].split()]

            graph.add_edge(edge[0], edge[1], send=send, receive=receive)
        except ValueError as e:
            # Then see if it is a valid node
            node = line.split(" ", 1)
            node[0] = int(node[0])
            graph.add_node(node[0])
    return graph

def routing_tables_from_graph(graph, rse_id, rse_amount):
    send_table = [0] * rse_amount
    receive_table = [0] * rse_amount

    for destination in range(rse_amount):
        if destination is rse_id:
            continue

        path = nx.shortest_path(graph, rse_id, destination)
        nexthop = path[1]
        send_table[destination] = graph[rse_id][nexthop]['send']
        receive_table[destination] = graph[rse_id][nexthop]['receive']

    send_table_bytes = bytes(0)
    for table_entry in send_table:
        send_table_bytes += table_entry.to_bytes(4, byteorder='little')
    assert(len(send_table_bytes) == 4 * rse_amount)

    receive_table_bytes = bytes(0)
    for table_entry in receive_table:
        receive_table_bytes += table_entry.to_bytes(4, byteorder='little')
    assert(len(receive_table_bytes) == 4 * rse_amount)

    return send_table_bytes, receive_table_bytes

parser = argparse.ArgumentParser()
parser.add_argument("--provisioning_code", help="the input provisioning code", required=True)
parser.add_argument("--provisioning_data", help="the input provisioning data", required=True)
parser.add_argument("--provisioning_values", help="the input provisioning values", required=True)
parser.add_argument("--dm_provisioning_key_file", help="the AES-CCM key file", required=True)
parser.add_argument("--rse_id", help="the ID of the RSE", required=False)
parser.add_argument("--rse_amount", help="the amount of RSEes in the system", required=False)
parser.add_argument("--multi_rse_topology_graph_file", help="The topology graph of a multi-rse system", required=False)
parser.add_argument("--bundle_output_file", help="bundle output file", required=True)
args = parser.parse_args()

with open(args.provisioning_code, "rb") as in_file:
    code = in_file.read()

# It's technically possible to have provisioning code that requires no DATA
# section, so this is optional.
try:
    with open(args.provisioning_data, "rb") as in_file:
        data = in_file.read()
except FileNotFoundError:
    data = bytes(0);

with open(args.provisioning_values, "rb") as in_file:
    values = in_file.read()

with open(args.dm_provisioning_key_file, "rb") as in_file:
    input_key = in_file.read()

if args.rse_id:
    rse_id = int(args.rse_id, 0).to_bytes(4, 'little')
else:
    rse_id = bytes(0)

if args.rse_amount:
    rse_amount = int(args.rse_id, 0).to_bytes(4, 'little')
else:
    rse_amount = int(1).to_bytes(4, 'little')

if args.multi_rse_topology_graph_file:
    assert(args.rse_id)
    assert(args.rse_amount)
    assert(int(args.rse_amount) != 1)

    graph = load_graph(args.multi_rse_topology_graph_file);
    send_table, receive_table = routing_tables_from_graph(graph,
                                                          int(args.rse_id),
                                                          int(args.rse_amount))
else:
    send_table = bytes(4)
    receive_table = bytes(4)

with open(args.dm_provisioning_key_file, "rb") as in_file:
    input_key = in_file.read()

# These are always required
patch_bundle = struct_pack([
    rse_id,
    send_table,
    receive_table,
])

values = patch_binary(values, patch_bundle, 0)

bundle = encrypt_bundle(code, 0xB000, values, 0x3800, data, 0x3D00, 0xAAAABEEFFEEDAAAA,
                        input_key)

with open(args.bundle_output_file, "wb") as out_file:
    out_file.write(bundle)
