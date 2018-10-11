import os
from subprocess import check_output
import argparse
import sys

if __name__ == '__main__':
    datadir = "stellar-consensus-datadir"
    shadow_configfile = "stellar-consensus.xml"
    shadow_plugin = "PEER_STELLARCONSENSUS"
    shadow_inject_plugin = "INJECTOR_STELLARCONSENSUS"

    os.system("rm -rf %s" % datadir)

    ## run shadow for test  (Currently, stellar (and its test) is not implemented)
    returnCode = os.system("~/.shadow/bin/shadow -d %s %s" % (datadir, shadow_configfile))
    if returnCode != 0:
        print "test failed"
        sys.exit(-1)


