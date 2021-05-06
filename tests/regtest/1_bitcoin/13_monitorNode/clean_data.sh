#!/bin/bash

if [ $# -lt 1 ]; then
	echo "Fail: please given node number"
	exit
fi

echo "cleanup data start!"

rm -rf data/*
SHELL_PATH=`pwd -P`

path=../../../../testlibs/dump/difficulty_3

if [ ! -e $path/coinflip_hash.txt ]; then
  echo "start to dump!"
  cd ../../../../testlibs/datadirDump/
  sh startdump.sh 3
fi

cd $SHELL_PATH
i=0
while [ $i -lt $1 ]; do
	mkdir -p data/bcdnode$i
	cp -r $path/bcdnode0/* ./data/bcdnode$i
	i=$(($i+1))
done

cp -r $path/coinflip_hash.txt ./data
cp -r $path/key.txt ./data
cp -r $path/state.txt ./data

 echo "Datadir Bootstrapping success!"
