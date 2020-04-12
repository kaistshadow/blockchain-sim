#!/bin/bash

if [ $# -eq 1 ]; then
    python python-script/test-rc1-network-v1.py --nodenum $1
    python python-script/network_graph.py $1

elif [ $# -eq 2 ]; then
    n=`expr $1 + 1`
    python python-script/test-rc1-network-v1.py --nodenum $1 $2
    python python-script/network_graph.py $n
else
    echo "Invalid Arguments"
    exit 0
fi
