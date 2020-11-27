#!/bin/bash

# shadow setup: with -pg
cd ../../../../shadow/
./setup build -o
./setup install
cd - 1> /dev/null
# system info modification
sudo sysctl -w kernel.perf_event_paranoid=-1
sudo sh -c " echo 0 > /proc/sys/kernel/kptr_restrict"

XMLROOT="../../../../tests/regtest"
CORECNT=$(nproc)

xmls=(
	/shadow-bitcoin/5_storage-share/target_example.xml
	)
for (( i=0; i<${#xmls[@]}; i++ )); do
	FILE_DEST="$XMLROOT"${xmls[i]}
	DIR=${FILE_DEST%/*}
	XML_TARGET=${FILE_DEST##*/}
	#echo $FILE_DEST
	echo $DIR
	echo $XML_TARGET
	cd $DIR
    perf record -a -g shadow -d datadir -h 100000 $XML_TARGET
    rm -r ./datadir
    if test -f gmon.out; then
	    rm gmon.out
	fi
    cd - 1> /dev/null
    if [ ! -d ./perf_results ]; then
    	mkdir perf_results
    fi
    mv $DIR/perf.data ./perf_results/perf$i.data
    cp ./perf_results/perf$i.data ../FlameGraph/perf.data
    cd ../FlameGraph
    perf script | ./stackcollapse-perf.pl > out.perf-folded
    ./flamegraph.pl -width 2400 out.perf-folded > perf$i.svg
    rm perf.data
    rm out.perf-folded
    cd - 1> /dev/null
    mv ../FlameGraph/perf$i.svg ./perf_results/perf$i.svg
done