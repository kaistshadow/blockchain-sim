#!/bin/sh
sudo apt install -y clang python3 python3-pip libclang-10-dev
pip3 install clang
cd drshadow
rm -rf build
mkdir build
cd build
cmake ..
make
cd ../..
ln -sf ../drshadow/build/bin64/drshadow ./bin/drshadow
ln -sf ../drshadow/build/bin64/libdrshadow.so ./bin/libdrshadow.so
cd ./shadow-dynamorio-runner
gcc -o ../bin/drshadow-runner drshadow-runner.c