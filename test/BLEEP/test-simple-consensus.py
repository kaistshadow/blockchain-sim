import os
from subprocess import check_output
import argparse
import sys

if __name__ == '__main__':
    ## run shadow for test
    returnCode = os.system("~/.shadow/bin/shadow -d simple-consensus-datadir simple_consensus.xml")
    if returnCode != 0:
        print "test failed"
        sys.exit(-1)

    ## check results of shadow
    outputfile = "./simple-consensus-datadir/hosts/bleep9/stdout-bleep9.BLEEP_PEER_SIMPLE_CONSENSUS.1000.log"
    if os.path.exists(outputfile):
        f = open(outputfile)
        for line in f:
            if "Get Broadcast Request" in line:
                print "test success"
                sys.exit(0)
    print "test failed"
    sys.exit(-1)
        
    
