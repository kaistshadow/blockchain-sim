#!/bin/bash
if ! $(hash mpstat); then
	sudo apt-get install sysstat
fi
pip install numpy
pip install matplotlib
pip install pandas
pip install seaborn