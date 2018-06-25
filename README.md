# Project Overview
Blockchain-sim is a project at infant stage to build a efficient and realistic simulation/emulation framework for blockchain applications.

The core aims of the Blockchain-sim are as follows.

* Simulating the real blockchain application with real application binary (e.g., bitcoind)

* Synthesizing the various patterns of transactions or blocks with easy-to-setup interface

* Simulating the propagation of transactions or blocks on simulated P2P network

* High performance testing that enables the simulation of more-than-thousand nodes on a (single) commodity machine

Blockchain-sim is now under development on top of the open-source discrete-event network simulator, called Shadow, that have been mainly utilized for simulating Tor. 

# How to run

Currently, we only support bitcoin (v0.16.0, v0.15.0) and the simulation for basic P2P network connection. Arbitrary generation of the transactions or blocks and its propagation are not yet tested.

To install the necessary parts (Shadow network simulator and bitcoin parts) and to run the basic experiments (P2P network connection of the bitcoin nodes), we built a python script.

```
python setup.py
```

Please use Ubuntu 14.04.05 LTS. We confirmed the unresolved segmentation fault errors on Ubuntu 16.04 LTS.
