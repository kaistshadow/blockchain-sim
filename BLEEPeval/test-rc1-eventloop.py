import os
from subprocess import check_output, Popen, PIPE
import argparse
import sys

if __name__ == '__main__':
    datadir = "rc1-eventloop-datadir"
    shadow_configfile = "config-examples/rc1-eventloop.xml"

    os.system("rm -rf %s" % datadir)

    print sys.argv

    shadow = Popen([os.path.expanduser("~/.shadow/bin/shadow"), "-d", datadir, shadow_configfile], stdout=PIPE)

    shadow_stdout_filename = "shadow.output"
    shadow_stdout_file = open(shadow_stdout_filename, 'w')
    while shadow.poll() is None:
        l = shadow.stdout.readline()
        print l.strip()
        shadow_stdout_file.write(l)
    for l in shadow.stdout:
        print l.strip()
        shadow_stdout_file.write(l)
    shadow_stdout_file.close()

    
    os.system("mv %s ./%s/%s" % (shadow_stdout_filename, datadir, shadow_stdout_filename))
    shadow_stdout_file = open("./%s/%s" % (datadir, shadow_stdout_filename),'r')

    shadow_returnCode = shadow.returncode
    for line in shadow_stdout_file:
        if "critical" in line:
            print "critical error is occurred during shadow simulation"
            sys.exit(-1)

    if shadow_returnCode != 0:
        print "test failed"
        sys.exit(-1)

    test_pass = True
