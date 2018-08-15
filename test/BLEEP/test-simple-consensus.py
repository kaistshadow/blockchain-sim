import os
from subprocess import check_output
import argparse
import sys

if __name__ == '__main__':
    datadir = "simple-consensus-datadir"
    shadow_configfile = "simple-consensus.xml"
    shadow_plugin = "PEER_SIMPLECONSENSUS"
    shadow_inject_plugin = "INJECTOR_SIMPLE"

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
        test_pass = False
        f = open(outputfile0)
        for line in f:
            if "connection established" in line:
                test_pass = True
        if not test_pass:
            print "test failed for node 0"
            sys.exit(-1)

        test_pass = False
        f = open(outputfile0)
        for line in f:
            if "The string is: This_is_transaction_generated_by_injector" in line:
                test_pass = True
        if not test_pass:
            print "test failed for node 0"
            sys.exit(-1)

    ## 2. check results of peer node 1 (connection establishment?)
    outputfile1 = "./%s/hosts/bleep1/stdout-bleep1.%s.1000.log" % (datadir, shadow_plugin)
    test_pass = False
    if os.path.exists(outputfile1):
        f = open(outputfile1)
        for line in f:
            if "connection established" in line:
                test_pass = True
        if not test_pass:
            print "test failed for node 1"
            sys.exit(-1)

    ## 3. check results of peer node 3 (connection establishment? tx gossipping?)
    # outputfile3 = "./%s/hosts/bleep3/stdout-bleep3.%s.1000.log" % (datadir, shadow_plugin)
    # test_pass = False
    # if os.path.exists(outputfile3):
    #     f = open(outputfile3)
    #     for line in f:
    #         if "connection established" in line:
    #             test_pass = True
    #     if not test_pass:
    #         print "test failed for node 3"
    #         sys.exit(-1)

    #     test_pass = False
    #     f = open(outputfile3)
    #     for line in f:
    #         if "The string is: This_is_transaction_generated_by_injector" in line:
    #             test_pass = True
    #     if not test_pass:
    #         print "test failed for node 3"
    #         sys.exit(-1)


    ## 3. check results of injector node 4 (connection establishment? sent transaction?)
    outputfile5 = "./%s/hosts/bleep5/stdout-bleep5.%s.1000.log" % (datadir, shadow_inject_plugin)
    test_pass = False
    if os.path.exists(outputfile5):
        test_pass = False
        f = open(outputfile5)
        for line in f:
            if "connection established" in line:
                test_pass = True
        if not test_pass:
            print "test failed for node 5 (injector)"
            sys.exit(-1)

        test_pass = False
        f = open(outputfile5)
        for line in f:
            if "sented string" in line:
                test_pass = True
        if not test_pass:
            print "test failed for node 5 (injector)"
            sys.exit(-1)

    print "test passed for all nodes"
    sys.exit(0)

    
