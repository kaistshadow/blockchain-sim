# sh addexam.sh [node no.] [ simultime] [storageshare(disable/disable] [algorithm(pow/coinflip)] [transatcion/normal]

#!/bin/bash

bash cleanup_data.sh $1
python make_approximate_setmining_test.py $5 $1 $2 $3 $4
mv base$1N$2T.xml target_example.xml
