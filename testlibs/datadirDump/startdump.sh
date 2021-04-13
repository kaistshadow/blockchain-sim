#!/bin/bash

if [ $# -ne 1 ]; then
  echo "script need 1 parameter about difficulty "
  exit 1
fi
if [ $1 -gt 3 ]; then
  echo "difficulty is smaller than 3"
  exit
fi

#1. remove the  data file and make new bcdnode
CreateDIR=./data
if [ -d $CreateDIR ]; then
  rm -rf data/
fi
mkdir -p data/bcdnode0

#2-0. generate xml file
SHELL_PATH=`pwd -P`
python xmlGenerator.py 1 100 pow disable $1 $SHELL_PATH

#2. execute shadow
SHELL_PATH=`pwd -P`
echo $SHELL_PATH
shadow output.xml

#3. making key.txt and state.txt
python parsing_txt.py

#4. move to testlibs/dumpdata
DUMPPATH=../dump/difficulty_$1
echo $dumppath
if [ -d $DUMPPATH ]; then
  rm -rf $DUMPPATH/*
fi
mkdir -p  $DUMPPATH/bcdnode0/blocks
cp -r ./data/bcdnode0/blocks  $DUMPPATH/bcdnode0/
cp -r ./data/*.txt $DUMPPATH