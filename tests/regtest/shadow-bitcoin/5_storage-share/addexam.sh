# sh addexam.sh [node no.] [ simultime] [algorithm(pow/coinflip)] [transaction/normal]

#!/bin/bash

if [ $# -ne 4 ]; then
 echo "Usage: $0 <node #> <simul. time>  <PoW abs. (pow/coinflip)> <tx inject(transaction/normal)>"
 exit -1
fi

bash cleanup_data.sh $1
python make_approximate_setmining_test.py $5 $1 $2 $3 $4
mv base$1N$2T.xml target_example.xml
