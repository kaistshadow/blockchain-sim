#!/bin/bash

# check shadow installation, if not, proceed installation
if ! command -v ../../../Install/bin/shadow &> /dev/null
then
	echo "Shadow is not installed yet. Install main project first."
	exit
fi

# generator output.xml
python ./xmlGenerator.py 1000 3600

# install plugin
mkdir -p build
rm -r ./build/*
cd build
cmake ..
make
cd -
rm -r build

# test without memshare
sleep 0.2
../../../Install/bin/shadow -d datadir -h 10000 -w 8 output.xml
# test with memshare
sleep 0.2
../../../Install/bin/shadow -d datadir -h 10000 -w 8 --memshare=ON output.xml

