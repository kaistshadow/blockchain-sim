# Setup

# How to run

add target xml on the xmls set in mpstat_run.sh. There is already two example in the mpstat_run.sh:
```
xmls=(
    /BLEEP-POW-consensus/Consensus\[pow-tree\]-Gossip\[SP\]-200node-2sec.xml
    /BLEEP-gossip-10node/10node-txgossip.xml
	)
```
Currently, all test tartget should be in the regtest directory.
After modify xmls, just run the script.
```
bash mpstat_run.sh
```

The script will automatically generate mpstatX.log in mpstat_results directory after running mpstat for each xml target in the xmls.
