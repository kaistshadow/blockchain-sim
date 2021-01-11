#addexam.sh [node no.] [ simultime] [storageshare(disable/disable] [algorithm(pow/coinflip)]
#executeTest.sh [node no.] [ simultime] [storageshare(disable/disable] [algorithm(pow/coinflip)] [executeNo]

#!/bin/bash

#python make_approximate_setmining_test.py normal $1 $2 $3 $4
#mv base$1N$2T.xml target_example.xml
cd test_result
rm -r *
cd ..


python make_approximate_setmining_test.py normal $1 $2 $3 coinflip
mv base$1N$2T.xml target_example.xml

for i in $(seq 1 $5);
do
  mkdir -p test_result/coinflip/test$i/
  mkdir -p test_result/pow/test$i/
done

python make_approximate_setmining_test.py normal $1 $2 $3 pow
mv base$1N$2T.xml target_example.xml

for i in $(seq 1 $5);
do
  shadow -d datadir -h 100000 -w 8 target_example.xml >test_result/pow_$i.txt
  cp -r data/ test_result/coinflip/test$i/
  cp -r datadir/ test_result/coinflip/test$i/
done
python fileparsing.py $5 pow

for i in $(seq 1 $5);
do
  shadow -d datadir -h 100000 -w 8 target_example.xml >test_result/coinflip_$i.txt
  cp -r data/ test_result/coinflip/test$i/
  cp -r datadir/ test_result/coinflip/test$i/
done
python fileparsing.py $5 coinflip