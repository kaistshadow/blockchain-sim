#!/bin/bash

python ./xmlGenerator.py 1000 90000
sleep 0.2
rm ../../interfaces/shadow_memshare_interface/shadow_memshare_interface.h
sleep 0.2
cp ../../../shadow_memshare_interface_enabled.h ../../interfaces/shadow_memshare_interface/shadow_memshare_interface.h
sleep 0.2
cd ../..
python ./setup.py --all
cd -
sleep 0.2
shadow -d datadir -h 10000 -w 8 output.xml
rm ../../interfaces/shadow_memshare_interface/shadow_memshare_interface.h
cp ../../../shadow_memshare_interface_disabled.h ../../interfaces/shadow_memshare_interface/shadow_memshare_interface.h
cd ../..
python ./setup.py --all
cd -
sleep 0.2
shadow -d datadir -h 10000 -w 8 output.xml
