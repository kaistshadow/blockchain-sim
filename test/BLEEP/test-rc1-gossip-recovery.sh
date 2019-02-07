#!/bin/bash

if [ $# -eq 1 ]; then
    n=`expr $1 + 1`
    python test-rc1-gossip-recovery.py --nodenum $1
    python print_log.py $n rc1-gossip-datadir PEER
    python invariant_network_graph.py $n
fi
