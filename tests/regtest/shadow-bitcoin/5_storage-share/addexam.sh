#!/bin/bash

bash cleanup_data.sh $1
python make_approximate_setmining_test.py $1 $2
mv base$1N$2T.xml target_example.xml
