#!/bin/bash
if ! $(hash mpstat); then
	sudo apt-get install sysstat
fi
# shadow setup: without -pg
cd ../../../../shadow/
./setup build -c
./setup install
cd -