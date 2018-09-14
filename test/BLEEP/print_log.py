import os
from subprocess import check_output
import argparse
import sys

if __name__ == '__main__':
    
    if len (sys.argv) != 2 :
        print "Command line input error"
        sys.exit (1)

    num = int(sys.argv[1])
    datadir = "shadow.data"
    shadow_plugin = "plugin"
    
    for i in range(1,num+1):
        outputfile = "./%s/hosts/bleep%d/stdout-bleep%d.plugin.1000.log" % (datadir, i, i)
        os.system("echo '\n<<%d>>'" % i)
        os.system("cat %s" % (outputfile))
