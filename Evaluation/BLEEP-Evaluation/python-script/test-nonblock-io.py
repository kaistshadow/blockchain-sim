import os
from subprocess import check_output
import argparse
import sys

if __name__ == '__main__':
    datadir = "nonblock-io-datadir"
    shadow_configfile = "nonblock-io.xml"
    shadow_plugin = "PEER_NONBLOCK_IO"

    os.system("rm -rf %s" % datadir)

    ## run shadow for test
    returnCode = os.system("~/.shadow/bin/shadow -d %s %s" % (datadir, shadow_configfile))
    if returnCode != 0:
        print "test failed"
        sys.exit(-1)

    ## check results of shadow
    outputfile0 = "./%s/hosts/bleep0/stdout-bleep0.%s.1000.log" % (datadir, shadow_plugin)
    test_pass = False
    if os.path.exists(outputfile0):
        f = open(outputfile0)
        for line in f:
            if "connection established" in line:
                test_pass = True
        if not test_pass:
            print "test failed for node 0"
            sys.exit(-1)


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

    print "test passed for all nodes"
    sys.exit(0)

            

        
    
