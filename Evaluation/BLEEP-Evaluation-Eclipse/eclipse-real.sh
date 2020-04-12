#!/bin/bash

if [ $# -eq 1 ]; then
    python python-script/test-rc1-eclipse-real.py --nodenum $1
    python python-script/eclipse_graph.py $1
else
    echo "Invalid Arguments"
    exit 0
fi
