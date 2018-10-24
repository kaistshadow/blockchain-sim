import os
from subprocess import check_output
import argparse
import sys

if __name__ == '__main__':
    datadir = "centralized-broadcast-datadir"
    shadow_configfile = "centralized-broadcast.xml"
    shadow_plugin = "PEER_CENTRALIZED_POWCONSENSUS"
    shadow_inject_plugin = "INJECTOR_CENTRALIZED_POWCONSENSUS"

    ## run shadow for test
    returnCode = os.system("~/.shadow/bin/shadow -d %s %s" % (datadir, shadow_configfile))
    if returnCode != 0:
        print "test failed"
        sys.exit(-1)
