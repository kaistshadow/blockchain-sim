#!/bin/bash

if [ $# -eq 1 ]; then
    n=`expr $1 + 1`
    python python-script/test-rc1-gossip-recovery.py --nodenum $1
    python python-script/print_log.py $n rc1-gossip-datadir PEER
    python python-script/network_graph.py $n
fi
