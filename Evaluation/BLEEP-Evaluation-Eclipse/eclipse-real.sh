#!/bin/bash

if [ $# -eq 1 ]; then
    python python-script/test-rc1-eclipse-real.py --nodenum $1
    #python print_log.py 1 eclipse-datadir PEER
    python python-script/invariant_eclipse_graph.py $1
else
    echo "Invalid Arguments"
    exit 0
fi
