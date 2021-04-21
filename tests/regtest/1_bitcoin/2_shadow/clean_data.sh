#!/bin/bash

if [ $# -lt 1 ]; then
	echo "Fail: please given node number"
	exit
fi

echo "cleanup data start!"

rm -rf data/*

path=../../../../testlibs/dump/difficulty_3
i=0
while [ $i -lt $1 ]; do
	mkdir -p data/bcdnode$i
	cp -r $path/bcdnode0/* ./data/bcdnode$i
	i=$(($i+1))
done

cp -r $path/coinflip_hash.txt ./data

echo "Datadir Bootstrapping success!"