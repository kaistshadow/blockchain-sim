import os
from subprocess import check_output
import argparse
import sys

if __name__ == '__main__':

    if len(sys.argv) != 4:
        print "Command line input error"
        print "<num><datadir><plugin>"
        sys.exit (1)

    num = int(sys.argv[1])
    datadir = str(sys.argv[2])
    plugin  = str(sys.argv[3])

    for i in range(0, num):
        outputfile = "./%s/hosts/bleep%d/stdout-bleep%d.%s.1000.log" % (datadir, i, i, plugin)
        os.system("echo '\n<<%d>>'" % i)
        os.system("cat %s" % (outputfile))
