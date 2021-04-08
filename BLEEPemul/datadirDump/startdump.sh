#!bin/bash

if [ $# -ne 1 ]; then
  echo "script need 1 parameter about difficulty "
  exit 1
fi

#1. remove the  data file and make new bcdnode
CreateDIR=./data
if [ ! -d $CreateDIR ]; then
  rm data/
  mkdir -p data/bcdnode0
fi

#2-0. generate xml file
SHELL_PATH=`pwd -P`
echo $SHELL_PATH
python xmlGenerator.py 1 100 pow disable $1 $SHELL_PATH

#2. execute shadow
shadow output.xml

#3. making key.txt and state.txt
python parsing_txt.py

#4. move to testlibs/dumpdata
rm -rf ../../testlibs/dump/difficulty_$1/*
cp -r data/* ../../testlibs/dump/difficulty_$1/