#!/bin/bash

if [ $# -lt 1 ]; then
	echo "Fail: please given node number"
	exit
fi

echo "cleanup data start!"

rm -rf data/*
i=0
while [ $i -lt $1 ]; do
	mkdir -p data/bcdnode$i
	cp -r ../../../../testlibs/dump/difficulty_3/bcdnode0/* ./data/bcdnode$i
	i=$(($i+1))
done
