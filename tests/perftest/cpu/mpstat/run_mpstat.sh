#!/bin/bash
XMLROOT="../../../../tests/regtest"
CORECNT=$(nproc)
xmls=(
	/BLEEP-POW-consensus/Consensus\[pow-tree\]-Gossip\[SP\]-200node-2sec.xml
    /BLEEP-gossip-10node/10node-txgossip.xml
	)
for (( i=0; i<${#xmls[@]}; i++ )); do
	FILE_DEST="$XMLROOT"${xmls[i]}
	DIR=${FILE_DEST%/*}
	XML_TARGET=${FILE_DEST##*/}
	# run mpstat
	mpstat -P ALL 1 > mpstat.log & RUNPID=$!

	cd $DIR
	# run taskset shadow with one core
	taskset -c `expr $CORECNT - 1` shadow -d datadir -h 100000 $XML_TARGET
	kill -SIGINT $RUNPID
    rm -r ./datadir
    if test -f gmon.out; then
	    rm gmon.out
	fi
    if test -f perf.data; then
	    rm perf.data
	fi
    cd -
    if [ ! -d ./mpstat_results ]; then
    	mkdir mpstat_results
    fi
    mv ./mpstat.log ./mpstat_results/mpstat$i.log
done