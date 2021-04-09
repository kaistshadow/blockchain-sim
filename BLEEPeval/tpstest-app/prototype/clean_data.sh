#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Fail: please given node number"
  exit
fi

if [ ! -e rpc.so ]; then
  echo "start to dump!"
  cp ../../../BLEEPemul/emulation/rpc.so ./
fi

echo "cleanup data start!"
SHELL_PATH=`pwd -P`

path=../../../testlibs/dump/difficulty_$1
if [ ! -e $path/coinflip.txt ]; then
  echo "start to dump!"
  cd ../../../testlibs/datadirDump/
  sh startdump.sh $1
fi

cd $SHELL_PATH

CreateDIR=./data
if [ -d $CreateDIR ]; then
  rm -rf data/
fi

path="../../../testlibs/dump/difficulty_$1"

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
