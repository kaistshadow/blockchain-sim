#!/bin/sh

if [ 1 -eq $# ]; then
    echo "number of peers : " $1
else 
    echo "Wrong: have to set correct input"
    exit
fi

python setup_enviorment.py $1
#python print_log.py $1
python print_err_log.py $1
python draw_network.py $1
