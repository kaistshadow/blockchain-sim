#!/bin/bash

if [ $# -ne 1 ]; then
 echo "Usage: $0 worker_count"
 exit -1
fi
re='^[0-9]+$'
if ! [[ $1 =~ $re ]] ; then
   echo "error: Worker_count is not a positive integer" >&2; exit -1
fi

XMLROOT="../../../../tests/regtest"
CORECNT=$(nproc)
WORKER_CNT=$1
if [ $1 -le 1 ]; then
	TASKSET_PARAM=1
	WORKER_CNT=1
elif [ $1 -ge $CORECNT ]; then
	TASKSET_PARAM="0-`expr $CORECNT - 1`"
else
	TASKSET_PARAM_END="`expr $CORECNT - 1`"
	SUBS_PARAM=`expr $1 - 1`
	TASKSET_PARAM="`expr $TASKSET_PARAM_END - $SUBS_PARAM`-$TASKSET_PARAM_END"
fi
xmls=(
	/BLEEP-POW-consensus/example.xml
    /BLEEP-gossip-10node/10node-txgossip.xml
	)
for (( i=0; i<${#xmls[@]}; i++ )); do
	FILE_DEST="$XMLROOT"${xmls[i]}
	DIR=${FILE_DEST%/*}
	XML_TARGET=${FILE_DEST##*/}
	# run vmstat
	vmstat -a 1 -Sm > vmstat.log & RUNPID=$!

	cd $DIR
	# run taskset shadow
	taskset -c $TASKSET_PARAM shadow -d datadir -h 100000 -w $WORKER_CNT $XML_TARGET
	kill -SIGINT $RUNPID

	sleep 1

	PID_CHECK=$(tr -d '\0' < /proc/$RUNPID/cmdline )
	if [[ $PID_CHECK == *"vmstat"* ]]; then
		kill -9 $RUNPID
	fi
    rm -r ./datadir
    if test -f gmon.out; then
	    rm gmon.out
	fi
    if test -f perf.data; then
	    rm perf.data
	fi
    cd -
    if [ ! -d ./vmstat_results ]; then
    	mkdir vmstat_results
    fi
    mv ./vmstat.log ./vmstat_results/vmstat$i.log
done
