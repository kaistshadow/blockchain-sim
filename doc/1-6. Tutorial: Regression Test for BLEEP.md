In order to maintain the integrity of the BLEEP source code and to make better collaboration between developers, it is highly recommended to use regression testing.

The goal of using regression tests for BLEEP are as follows. 

1. To provide (uniform) experiment configurations for multiple developers.
2. Using the provided experimental settings, regression testing can provide the fair comparisons among different commits.
3. To automate the process of evaluating the performance improvements and correctness of the new commits. 
4. To (speculatively) prevent the bad commits that break the existing functionality or degrade the performance of the BLEEP.


The current version of BLEEP only provides a single experiment configuration (PoW + SpanningTree-based Gossip for 200 node). 
In recent future, it should be updated for more experiment options and more automatic testing architecture. 
The participation from the BLEEP developers for updating the regression testing is highly recommended. (Also including the updates for this wiki page)

Currently, you should manually follow the instruction for the test. We have a plan to leverage existing CI(Continuous Integration, such as Travis CI) for automatic BLEEP's regression testing in near future. 

## Manual regression test

We assumed that BLEEP is correctly installed in your machine. 
(Follow the [install instruction](https://github.com/kaistshadow/blockchain-sim/wiki/1.-Install-BLEEP))

In BLEEP repository directory(`blockchain-sim`), do as follows.
```bash
cd regtest
python test.py Consensus\[pow-tree\]-Gossip\[SP\]-200node-2sec.xml
```

The result stat can be obtained from console (stdout).
```
Execute shadow experiment
** Starting Shadow v1.12.1-13-g3b29407 2019-07-23 (built 2019-08-14) with GLib v2.42.1 and IGraph v0.6.5
** Stopping Shadow, returning code 0 (success)
elapsed_millisec=4083
```

As shown in above, the experiment running time is 4083 milliseconds.

In local machine (i7-6700, 16GB), the results are as follows.

|virtual sec (sec)  |  running time (sec) |
|-------------------|---------------------|
|2|3.74|
|100|17.88|
|200|31.96|
|300|49.41|
|400|61.67|
|500|76.17|
|1000|149.63|
|10000|1469.86|
|20000|2915.98|

Compared to original PoW (which leverage list-based data structure), the running time is greatly reduced for long experiments. (for 10000 vsec experiment, time is reduced from 2372.48 to 1469.86. for 20000 vsec experiment, time is reduced from 7508.07 to 2915.98)


You can visualize the event of the experiment using browser. 
(Clement. You may need to update the script or javascript sources for proper visualization. Current version of regtest may not work well for visualization)

## Configuration infos

### Consensus[pow-tree]-Gossip[SP]-200node
It uses PoW consensus with tree-based data structure. 
It uses spanning tree gossip overlay which is static and initialized using xml configuration.

New options should be appended hereafter.

## Using Continuous Integration
... need to be done ...