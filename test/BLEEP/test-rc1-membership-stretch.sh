#!/bin/bash

if [ $# -eq 1 ]; then
    python test-rc1-membership-stretch.py --nodenum $1
    python print_log.py $1 rc1-gossip-datadir PEER
    python invariant_network_graph.py $1
fi
