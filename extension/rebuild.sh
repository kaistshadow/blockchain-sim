#!/bin/sh
cd ./shadow-dynamorio-runner
gcc -o ../bin/drshadow-runner drshadow-runner.c
cd ..
cd ./drshadow/build
make