#!/bin/bash

rm -r ./data/*
for i in $(seq 0 1 `expr $1 - 1`)
do
	mkdir -p ./data/bcdnode$i
done