import os
from subprocess import check_output, Popen, PIPE
import argparse
import sys


if __name__ == '__main__':
    datadir = "rc1-datadir"
    shadow_configfile = "rc1.xml"
    shadow_plugin = "NODE_PROXYMODEL"

    shadow = Popen([os.path.expanduser("~/.shadow/bin/shadow"), "-d", datadir, shadow_configfile], stdout=PIPE)

    shadow_stdout = []
    while shadow.poll() is None:
        l = shadow.stdout.readline()
        print l.strip()
        shadow_stdout.append(l.strip())

    shadow_returnCode = shadow.returncode
    for line in shadow_stdout:
        if "critical" in shadow_stdout:
            print "critical error is occurred during shadow simulation"
            sys.exit(-1)
    if shadow_returnCode != 0:
        print "test failed"
        sys.exit(-1)

    test_pass = True
