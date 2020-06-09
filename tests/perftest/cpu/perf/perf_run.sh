#!/bin/bash
XMLROOT="../../../../tests/regtest"
CORECNT=$(nproc)

xmls=(
	/BLEEP-POW-consensus/Consensus\[pow-tree\]-Gossip\[SP\]-200node-2sec.xml
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
    cd -
    if [ ! -d ./perf_results ]; then
    	mkdir perf_results
    fi
    mv $DIR/perf.data ./perf_results/perf$i.data
done