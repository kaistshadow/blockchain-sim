#!/bin/bash

if [ $# -eq 1 ]; then
    python test-rc1-eclipse.py --nodenum $1
    #python print_log.py 1 eclipse-datadir PEER
    python invariant_eclipse_graph.py $1
else
    echo "Invalid Arguments"
    exit 0
fi
