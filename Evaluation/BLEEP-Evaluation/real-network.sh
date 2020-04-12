#!/bin/bash

if [ $# -eq 1 ]; then
    python python-script/real-network.py --nodenum $1
    python python-script/invariant_network_graph.py $1
elif [ $# -eq 2 ]; then
    n=`expr $1 + 1`
    m=`expr $1 + 11`
    python python-script/real-network.py --nodenum $1 $2
    python python-script/invariant_network_graph.py $m
else
    echo "Invalid Arguments"
    exit 0
fi
