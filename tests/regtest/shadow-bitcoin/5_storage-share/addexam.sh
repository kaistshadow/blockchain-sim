# sh addexam.sh [node no.] [ simultime] [storageshare(disable/enable] [algorithm(pow/coinflip)] [transaction/normal]

#!/bin/bash

if [ $# -ne 5 ]; then
 echo "Usage: $0 <node #> <simul. time> <storageshare(disable/enable)> <PoW abs. (pow/coinflip)> <tx inject(transaction/normal)>"
 exit -1
fi

bash cleanup_data.sh $1
python make_approximate_setmining_test.py $5 $1 $2 $3 $4
mv base$1N$2T.xml target_example.xml
