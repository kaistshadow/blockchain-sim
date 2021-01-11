#!/bin/bash

if [ $# -eq 1 ]; then
    python python-script/test-rc1-membership-stretch.py --nodenum $1
    python python-script/print_log.py $1 rc1-gossip-datadir PEER
    python python-script/network_graph.py $1
fi
