#!/bin/bash

# for ubuntu 18.04

cd ../
mkdir -p tool_set
INST_DIR=$(pwd)/install_set
TOOL_DIR=$(pwd)/tool_set
cd -

# bcc installation
sudo apt-get update
sudo apt-get -y install bison build-essential cmake flex git libedit-dev \
  libllvm6.0 llvm-6.0-dev libclang-6.0-dev python zlib1g-dev libelf-dev
git clone https://github.com/iovisor/bcc.git
mkdir bcc/build; cd bcc/build
cd $TOOL_DIR
mkdir -p bcc
cd -
cmake .. -DCMAKE_INSTALL_PREFIX="$TOOL_DIR/bcc"
make
make install
# bcc installation end