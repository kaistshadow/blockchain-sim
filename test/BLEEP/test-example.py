import os
from subprocess import check_output
import argparse
import sys

if __name__ == '__main__':
    ## run shadow for test
    returnCode = os.system("~/.shadow/bin/shadow -d example-datadir example.xml")
    if returnCode != 0:
        print "test failed"
        sys.exit(-1)

    ## check results of shadow
    outputfile = "./example-datadir/hosts/bleep1/stdout-bleep1.BLEEP_PEER_EXAMPLE.1000.log"
    if os.path.exists(outputfile):
        f = open(outputfile)
        for line in f:
            if "inPeer" in line:
                print "test success"
                sys.exit(0)
    print "test failed"
    sys.exit(-1)
        
    
