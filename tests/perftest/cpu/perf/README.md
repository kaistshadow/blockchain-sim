# Setup

The perf tools requires that:
* each shared object plugin for Shadow should be complied with -g flag to enable extra debug symbol
* target plugin shared object should be built before the analysis. This can be done by just running python blockchain-sim/setup.py --install on the cloned project base directory.

Before running the perf required tests, run the following script
```
bash perf_setup.sh
```
This script automatically install perf tool, reinstall Shadow with profiling flags, clone Flame Graph from https://github.com/brendangregg/FlameGraph, and setting system(kernel.perf_event_paranoid and /proc/sys/kernel/kptr_restrict settings) appropriate to the perf tool usage. 

# How to run

add target xml on the xmls set in perf_run.sh. There is already two example in the perf_run.sh:
```
xmls=(
    /BLEEP-POW-consensus/Consensus\[pow-tree\]-Gossip\[SP\]-200node-2sec.xml
    /BLEEP-gossip-10node/10node-txgossip.xml
	)
```
Currently, all test tartget should be in the regtest directory.
After modify xmls, just run the script.
```
bash perf_run.sh
```

The script will automatically generate perf_results directory after running perf record for each xml target in the xmls.
The result data (perfX.data) can be used to anaylze with the command perf report -i perfX.data.
The flame graph for each result data is also generated as a result of the bash script.
