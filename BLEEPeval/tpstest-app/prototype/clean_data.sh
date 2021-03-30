#!/bin/bash

if [ $# -lt 1 ]; then
	echo "Fail: please given node number"
	exit
fi

echo "cleanup data start!"

rm -rf data/*

for  i in $(seq 0 $1); do
	mkdir -p data/bcdnode$i
	cp -r ../../../testlibs/dump/difficulty_3/bcdnode0/* ./data/bcdnode$i
done

cp -r ../../../testlibs/dump/difficulty_3/coinflip_hash.txt ./data
echo "Datadir Bootstrapping success!"
