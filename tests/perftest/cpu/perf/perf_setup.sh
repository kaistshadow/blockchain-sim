#!/bin/bash
sudo apt-get install linux-tools-common linux-tools-generic linux-tools-`uname -r`
# system info modification
sudo sysctl -w kernel.perf_event_paranoid=-1
sudo sh -c " echo 0 > /proc/sys/kernel/kptr_restrict"
# shadow setup: with -pg
cd ../../../../shadow/
./setup build -c -o
./setup install
cd -
# FlameGraph setup
cd ..
if [ ! -d ./FlameGraph ]; then
	git clone https://github.com/brendangregg/FlameGraph
fi
cd -