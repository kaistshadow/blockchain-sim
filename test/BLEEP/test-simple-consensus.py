import os
from subprocess import check_output
import argparse
import sys

if __name__ == '__main__':
    datadir = "simple-consensus-datadir"
    shadow_configfile = "simple-consensus.xml"
    shadow_plugin = "PEER_SIMPLECONSENSUS"
    shadow_inject_plugin = "INJECTOR_SIMPLECONSENSUS"

    os.system("rm -rf %s" % datadir)

    ## run shadow for test
    returnCode = os.system("~/.shadow/bin/shadow -d %s %s" % (datadir, shadow_configfile))
    if returnCode != 0:
        print "test failed"
        sys.exit(-1)

    ################### check results of shadow

    ## 1. check results of peer node 0 (connection establishment?, transaction received?)
    outputfile0 = "./%s/hosts/bleep0/stdout-bleep0.%s.1000.log" % (datadir, shadow_plugin)
    test_pass = False
    if os.path.exists(outputfile0):
        # test_pass = False
        # f = open(outputfile0)
        # for line in f:
        #     if "connection established" in line:
        #         test_pass = True
        # if not test_pass:
        #     print "test failed for node 0"
        #     sys.exit(-1)

        test_pass = False
        f = open(outputfile0)
        for line in f:
            if "Transaction(0 sends 12.34 to 1) is inserted into TxPool" in line:
                test_pass = True
        if not test_pass:
            print "test failed for node 0"
            sys.exit(-1)

    ## 2. check results of peer node 1 (connection establishment?)
    # outputfile1 = "./%s/hosts/bleep1/stdout-bleep1.%s.1000.log" % (datadir, shadow_plugin)
    # test_pass = False
    # if os.path.exists(outputfile1):
    #     f = open(outputfile1)
    #     for line in f:
    #         if "connection established" in line:
    #             test_pass = True
    #     if not test_pass:
    #         print "test failed for node 1"
    #         sys.exit(-1)

    ## 3. check results of peer node 3 (connection establishment? tx gossipping?)
    outputfile3 = "./%s/hosts/bleep3/stdout-bleep3.%s.1000.log" % (datadir, shadow_plugin)
    test_pass = False
    if os.path.exists(outputfile3):
        f = open(outputfile3)
        for line in f:
            if "Transaction(0 sends 12.34 to 1) is inserted into TxPool" in line:
                test_pass = True
        if not test_pass:
            print "test failed for node 3: tx is not transferred!"
            sys.exit(-1)


    ## 4. check results of injector node 4 (connection establishment? sent transaction?)
    outputfile4 = "./%s/hosts/bleep4/stdout-bleep4.%s.1000.log" % (datadir, shadow_inject_plugin)
    # test_pass = False
    # if os.path.exists(outputfile4):
        # test_pass = False
        # f = open(outputfile4)
        # for line in f:
        #     if "connection established" in line:
        #         test_pass = True
        # if not test_pass:
        #     print "test failed for node 4 (injector)"
        #     sys.exit(-1)

        # test_pass = False
        # f = open(outputfile4)
        # for line in f:
        #     if "sented string" in line:
        #         test_pass = True
        # if not test_pass:
        #     print "test failed for node 4 (injector)"
        #     sys.exit(-1)

    ################### Test added for block propagation
    ## 5. check results of peer node 3 (block gossipping?)
    outputfile3 = "./%s/hosts/bleep3/stdout-bleep3.%s.1000.log" % (datadir, shadow_plugin)
    test_pass = False
    if os.path.exists(outputfile3):
        f = open(outputfile3)
        for line in f:
            if "Following block is received" in line:
                test_pass = True

        test_pass = False
        f = open(outputfile3)
        for line in f:
            if "injected block 0:Block has following transactions" in line:
                test_pass = True
        if not test_pass:
            print "test failed for node 3: injected block is not transferred!"
            sys.exit(-1)

    ################### Test added for simple consensus
    ## check results of injector (transaction injected?)
    outputfile4 = "./%s/hosts/bleep4/stdout-bleep4.%s.1000.log" % (datadir, shadow_inject_plugin)
    if os.path.exists(outputfile4):
        txs = ["0 sends 12.34 to 1", "0 sends 10 to 2", "1 sends 10 to 3", "2 sends 11 to 0",
               "1 sends 12.34 to 0"]
        test_pass = [False for i in range(len(txs))]
        f = open(outputfile4)
        for line in f:
            for tx_i in range(len(txs)):
                if txs[tx_i] in line:
                    test_pass[tx_i] = True
        if not all(test_pass):
            print "test failed for node 4 (injector)"
            sys.exit(-1)


    ## check block consensus by node 0
    outputfile0 = "./%s/hosts/bleep0/stdout-bleep0.%s.1000.log" % (datadir, shadow_plugin)
    if os.path.exists(outputfile0):
        if any("Consensus on block" in line for line in open(outputfile0).readlines()):
            pass
        else:
            print "test failed for node 0: No consensus is occured"
            sys.exit(-1)
        
        consensus_block_lines = filter( lambda x: "consensus block:" in x, open(outputfile0).readlines())

        txs = ["0 sends 12.34 to 1", "0 sends 10 to 2", "1 sends 10 to 3", "2 sends 11 to 0",
               "1 sends 12.34 to 0"]
        test_pass = [False for i in range(len(txs))]
        for line in consensus_block_lines:
            for tx_i in range(len(txs)):
                if txs[tx_i] in line:
                    test_pass[tx_i] = True
        if not all(test_pass):
            print "test failed for node 0: No valid consensus."
            sys.exit(-1)
    else:
        print "Test failed(No output file)"
        sys.exit(-1)


    print "test passed for all nodes"
    sys.exit(0)

    
