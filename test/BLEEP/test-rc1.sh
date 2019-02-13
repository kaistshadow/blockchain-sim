#!/bin/bash

if [ $# -eq 1 ]; then
     n=`expr $1 + 1`
    python test-rc1.py --nodenum $1
    python invariant_network_graph_proxy.py $n
    python invariant_blockchain_graph.py $n rc1-datadir
else
    echo "Invalid Arguments"
    exit 0
fi
