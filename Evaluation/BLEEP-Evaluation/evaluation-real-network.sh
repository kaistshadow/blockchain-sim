#!/bin/bash

# [NOTE] 
# arg 1 = # of node, arg 2 = "--injector" option
# Other argument can be passed check real-network.py -> check it
if [ $# -eq 1 ]; then
    python python-script/real-network.py --nodenum $1
    python python-script/network_graph.py $1
elif [ $# -eq 2 ]; then
    n=`expr $1 + 1`
    m=`expr $1 + 11`
    python python-script/real-network.py --nodenum $1 $2
    python python-script/network_graph.py $m
    python python-script/crawling_msg_info.py $1
    python python-script/process-latency.py
else
    echo "Invalid Arguments"
    exit 0
fi