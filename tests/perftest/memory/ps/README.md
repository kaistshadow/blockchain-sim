# Setup

Before running the ps tests, run the following script
```
bash ps_setup.sh
```
This script automatically install procps tools including ps, reinstall Shadow without profiling flags.

# How to run

add target xml on the xmls set in ps_run.sh. There is already two example in the ps_run.sh:
```
xmls=(
    /BLEEP-POW-consensus/Consensus\[pow-tree\]-Gossip\[SP\]-200node-2sec.xml
    /BLEEP-gossip-10node/10node-txgossip.xml
	)
```
Currently, all test tartget should be in the regtest directory.
After modify xmls, just run the script.
```
bash ps_run.sh <worker count>
```
Worker count should be positve interger. Worker thread in Shadow is assigned with -w option on Shadow.
The taskset is also given based on the worker count. It assigns Shadow worker thread from the last cpu core.
For example, when we run the script with 5 worker count in 8 core system, taskset assigns the Shadow to use 3-7 cores from 0-7 cores.

The script will automatically generate psX.log in ps_results directory after running ps for each xml target in the xmls.
For each test case, The script runs ps per each 0.1 second period.
