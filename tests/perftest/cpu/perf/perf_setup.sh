#!/bin/bash
sudo apt-get install linux-tools-common linux-tools-generic linux-tools-`uname -r`
# FlameGraph setup
cd ..
if [ ! -d ./FlameGraph ]; then
	git clone https://github.com/brendangregg/FlameGraph
fi
cd -