#!/bin/bash

# if [ $# -ne 1 ]; then
#  echo "Usage: $0 worker_count"
#  exit -1
# fi
# re='^[0-9]+$'
# if ! [[ $1 =~ $re ]] ; then
#    echo "error: Worker_count is not a positive integer" >&2; exit -1
# fi

XMLROOT="../../tests/perftest"
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
	/memory_sharing/output.xml
	)
for (( i=0; i<${#xmls[@]}; i++ )); do
	FILE_DEST="$XMLROOT"${xmls[i]}
	DIR=${FILE_DEST%/*}
	XML_TARGET=${FILE_DEST##*/}


	cd $DIR
	# run shadow
    if [ $3 -eq "1" ]; then 
        shadow -d datadir -h 10000 -w $WORKER_CNT -m $4 & RUNPID=$! 

    else
        shadow -d datadir -h 10000 -w $WORKER_CNT $4 & RUNPID=$!
    fi

	ps u -p $RUNPID &> ps.log
	while :
	do
		sleep 0.1
		if [ ! -f /proc/$RUNPID/cmdline ]; then
			break
		fi
		PID_CHECK=$(tr -d '\0' < /proc/$RUNPID/cmdline )
		if [[ ! $PID_CHECK == *"shadow"* ]]; then
			break
		fi
		ps u --no-headers -p $RUNPID >> ps.log
	done
    rm -r ./datadir
    if test -f gmon.out; then
	    rm gmon.out
	fi
    if test -f perf.data; then
	    rm perf.data
	fi
    cd - 1> /dev/null
    if [ ! -d ./ps_results ]; then
    	mkdir ps_results
    fi

    if [ $3 -eq "1" ]; then 
        mv $DIR/ps.log ./ps_results/dedup_$2s.log
    else
        mv $DIR/ps.log ./ps_results/non_dedup_$2s.log
    fi
done
