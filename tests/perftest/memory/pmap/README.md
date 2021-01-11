# Setup

Before running the pmap tests, run the following script
```
bash pmap_setup.sh
```
This script automatically install procps tools including pmap, reinstall Shadow without profiling flags.

# How to run

add target xml on the xmls set in pmap_run.sh. There is already two example in the pmap_run.sh:
```
xmls=(
    /BLEEP-POW-consensus/Consensus\[pow-tree\]-Gossip\[SP\]-200node-2sec.xml
    /BLEEP-gossip-10node/10node-txgossip.xml
	)
```
Currently, all test tartget should be in the regtest directory.
After modify xmls, just run the script.
```
bash pmap_run.sh <worker count>
```
Worker count should be positve interger. Worker thread in Shadow is assigned with -w option on Shadow.
The taskset is also given based on the worker count. It assigns Shadow worker thread from the last cpu core.
For example, when we run the script with 5 worker count in 8 core system, taskset assigns the Shadow to use 3-7 cores from 0-7 cores.

The script will automatically generate pmapX.log in pmap_results directory after running pmap for each xml target in the xmls.
For each test case, The script runs pmap per each 1 second period.
