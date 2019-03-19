import os
from subprocess import check_output
import argparse
import sys

if __name__ == '__main__':
    datadir = "plumtree-broadcast-datadir"
    shadow_configfile = "plumtree-broadcast.xml"
    shadow_plugin = "PEER_PLUMTREE"
    # shadow_inject_plugin = "INJECTOR_SIMPLECONSENSUS"

    ## run shadow for test
    returnCode = os.system("~/.shadow/bin/shadow -d %s %s" % (datadir, shadow_configfile))
    if returnCode != 0:
        print "test failed"
        sys.exit(-1)
