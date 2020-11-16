# executeTest_TPS.sh [node no.] [ simultime] [storageshare(disable/disable] [algorithm(pow/coinflip)]

#cleanup datadir
bash cleanup_data.sh $1

# make xml 
python make_approximate_setmining_test.py normal $1 $2 $3 $4
mv base$1N$2T.xml target_example.xml

# execute test
shadow -d datadir -w 8 -h 100000 target_example.xml
python calcResult.py $1 $2