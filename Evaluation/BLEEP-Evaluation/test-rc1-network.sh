#!/bin/bash

if [ $# -eq 1 ]; then
    python test-rc1-network.py --nodenum $1
    #python print_log.py $1 rc1-datadir PEER
    python invariant_network_graph.py $1

elif [ $# -eq 2 ]; then
    n=`expr $1 + 1`
    m=`expr $1 + 11`
    python test-rc1-network.py --nodenum $1 $2
    #python print_log.py $m rc1-datadir PEER
    python invariant_network_graph.py $m
    #python invariant_blockchain_graph.py $1 rc1-datadir

else
    echo "Invalid Arguments"
    exit 0
fi
