# Setup

The vmstat requires that:
* target plugin shared object should be built before the anaylsis. This can be done by just running python blockchain-sim/setup.py --install on the cloned project base directory.
* installing Shadow without profile flag(-o). Profiling timer expiration may occur on -pg option.

Before running the vmstat tests, run the following script
```
bash vmstat_setup.sh
```
This script automatically install sysstat tools including vmstat, reinstall Shadow without profiling flags.

# How to run

add target xml on the xmls set in vmstat_run.sh. There is already two example in the vmstat_run.sh:
```
xmls=(
    /BLEEP-POW-consensus/Consensus\[pow-tree\]-Gossip\[SP\]-200node-2sec.xml
    /BLEEP-gossip-10node/10node-txgossip.xml
	)
```
Currently, all test tartget should be in the regtest directory.
After modify xmls, just run the script.
```
bash vmstat_run.sh <worker count>
```
Worker count should be positve interger. Worker thread in Shadow is assigned with -w option on Shadow.
The taskset is also given based on the worker count. It assigns Shadow worker thread from the last cpu core.
For example, when we run the script with 5 worker count in 8 core system, taskset assigns the Shadow to use 3-7 cores from 0-7 cores.

The script will automatically generate vmstatX.log in vmstat_results directory after running vmstat for each xml target in the xmls.
