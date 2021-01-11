#!/bin/bash
if ! $(hash pmap); then
	sudo apt-get install procps
fi
# shadow setup: without -pg
cd ../../../../shadow/
./setup build -c
./setup install
cd -